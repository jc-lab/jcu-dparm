/**
 * @file	drive_handle_sanitize.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <string.h>

#include "drive_handle_base.h"

#include <jcu-dparm/ata_types.h>
#include <jcu-dparm/nvme_types.h>
#include <jcu-dparm/ata_utils.h>

namespace jcu {
namespace dparm {

static int32_t ataEstToSeconds(uint16_t time_required, uint16_t extended_time_reported) {
  if (time_required == 0) {
    return -1;
  }
  if (extended_time_reported) {
    if (time_required < 32767) {
      return time_required * 2;
    } else {
      return -3;
    }
  } else {
    if (time_required < 255) {
      return time_required * 2 * 60;
    } else {
      return -2;
    }
  }
}

static int32_t nvmeEstToSeconds(uint32_t value) {
  if (value == 0xffffffffU) {
    return -1;
  }
  return (int32_t)value;
}

DparmReturn<SanitizeCmdResult> DriveHandleBase::doSanitizeCmd(const SanitizeOptions& options) {
  auto driver_handle = getDriverHandle();
  auto driving_type = driver_handle->getDrivingType();
  DparmResult dr;

  if (driving_type == kDrivingAtapi) {
    ata::ata_tf_t tf;
    uint64_t lba = 0;

    /*
     * ATA Command Set - 4 (ACS-4)
     *
     * COUNT(nsect)
     *  15   : ZONED NO RESET (7.32.2.3.2)
     *  14:8 : Reserved
     *  7
     *  L___ (OVERWRITE) : INVERT PATTERN BETWEEN OVERWRITE PASSES bit (7.32.4.3.2)
     *  6
     *  L___ (OVERWRITE) : DEFINITIVE ENDING PATTERN bit (7.32.4.3.3)
     *  5    : Reserved
     *  4    : FAILURE MODE bit – (7.32.2.3.3)
     *  3:0
     *  L___ (OVERWRITE) : OVERWRITE PASS COUNT field – (7.32.4.3.4)
     * */

    ata::tf_init(&tf, ata::ATA_OP_SANITIZE, 0, 0);
    switch (options.feature) {
      case kSanitizeStatus:
        tf.lob.feat = ata::SANITIZE_STATUS_EXT;
        break;
      case kSanitizeAntiFreezeLock:
        tf.lob.feat = ata::SANITIZE_ANTIFREEZE_LOCK_EXT;
        lba = ata::SANITIZE_ANTIFREEZE_LOCK_KEY;
        break;
      case kSanitizeFreezeLock:
        tf.lob.feat = ata::SANITIZE_FREEZE_LOCK_EXT;
        lba = ata::SANITIZE_FREEZE_LOCK_KEY;
        break;
      case kSanitizeCryptoScramble:
        tf.lob.feat = ata::SANITIZE_CRYPTO_SCRAMBLE_EXT;
        lba = ata::SANITIZE_CRYPTO_SCRAMBLE_KEY;
        break;
      case kSanitizeBlockErase:
        tf.lob.feat = ata::SANITIZE_BLOCK_ERASE_EXT;
        lba = ata::SANITIZE_BLOCK_ERASE_KEY;
        break;
      case kSanitizeOverwrite:
        tf.lob.feat = ata::SANITIZE_OVERWRITE_EXT;
        lba = (((uint64_t)ata::SANITIZE_OVERWRITE_KEY) << 32U) | (options.overwrite_pattern);
        tf.lob.nsect |= (options.overwrite_pass & 0x0f);
        tf.lob.nsect |= 0x80; // INVERT PATTERN BETWEEN OVERWRITE PASSES
        break;
    }
    tf.lob.lbal    = (uint8_t)(lba);
    tf.lob.lbam    = (uint8_t)(lba >>  8U);
    tf.lob.lbah    = (uint8_t)(lba >> 16U);
    tf.hob.lbal    = (uint8_t)(lba >> 24U);
    tf.hob.lbam    = (uint8_t)(lba >> 32U);
    tf.hob.lbah    = (uint8_t)(lba >> 40U);
    tf.is_lba48 = 1;

    if (options.no_deallocate) {
      tf.hob.nsect |= 0x80; // ZONED NO RESET
    }

    dr = driver_handle->doTaskfileCmd(0, -1, &tf, nullptr, 0, 15);
    if (!dr.isOk()) {
      return { dr.code, dr.sys_error, {} };
    }

    int sanitize_state = 0;
    uint64_t lba_progress = (((uint16_t)tf.lob.lbal) << 0) | (((uint16_t)tf.lob.lbam) << 8);
    float progress = (lba_progress != 0xffff) ? ((float)lba_progress / 65536.0f) : -1;
    if (tf.hob.nsect & ata::SANITIZE_FLAG_OPERATION_SUCCEEDED) {
      sanitize_state |= kSanitizeIdle | kSanitizeOperationSucceeded;
    }
    if (tf.hob.nsect & ata::SANITIZE_FLAG_OPERATION_IN_PROGRESS) {
      sanitize_state |= kSanitizeOperationInProgress;
    }
    if (tf.hob.nsect & ata::SANITIZE_FLAG_DEVICE_IN_FROZEN) {
      sanitize_state |= kSanitizeInFrozen;
    }
    if (tf.hob.nsect & ata::SANITIZE_FLAG_ANTIFREEZE_BIT) {
      sanitize_state |= kSanitizeAntiFreezed;
    }

    return { DPARME_OK, 0, tf.status, {
        tf.hob.nsect,
        (SanitizeState)sanitize_state,
        progress
    } };
  }
  else if (driving_type == kDrivingNvme) {
    SanitizeCmdResult sanitize_status = {0};
    nvme::nvme_admin_cmd_t cmd = {0};

    cmd.opcode = nvme::NVME_ADMIN_OP_SANITIZE_NVM;

    if (options.no_deallocate) {
      cmd.cdw10 |= nvme::NVME_SANITIZE_NO_DEALLOC;
    }

    // NO AUSE (Allow Unstricted Sanitize Exit)
    switch (options.feature) {
      case kSanitizeStatus:
        dr = doNvmeGetSanitizeStatus(&sanitize_status, nullptr);
        return { dr, sanitize_status };
      case kSanitizeCryptoScramble:
        cmd.cdw10 |= nvme::NVME_SANITIZE_ACT_CRYPTO_ERASE;
        break;
      case kSanitizeBlockErase:
        cmd.cdw10 |= nvme::NVME_SANITIZE_ACT_BLOCK_ERASE;
        break;
      case kSanitizeOverwrite:
        cmd.cdw10 |= nvme::NVME_SANITIZE_ACT_OVERWRITE;
        if (options.overwrite_pass > 0) {
          cmd.cdw10 |= (options.overwrite_pass & 0x0fU) << (nvme::NVME_SANITIZE_OWPASS_SHIFT);
        } else {
          cmd.cdw10 |= (1U) << (nvme::NVME_SANITIZE_OWPASS_SHIFT);
        }
        cmd.cdw11 = options.overwrite_pattern;
        break;
      default:
        return { DPARME_NOT_SUPPORTED, 0, {} };
    }

    auto nvme_result = driver_handle->doNvmeAdminPassthru(&cmd);
    return { nvme_result.code, nvme_result.sys_error, nvme_result.drive_status, {} };
  }
  return { DPARME_NOT_SUPPORTED, 0, {} };
}

DparmReturn<SanitizeEstimates> DriveHandleBase::getSanitizeEstimates() {
  auto driver_handle = getDriverHandle();
  auto driving_type = driver_handle->getDrivingType();
  DparmResult dr;
  SanitizeEstimates ests;

  if (driving_type == kDrivingAtapi) {
    auto const& drive_info = getDriveInfo();
    if (drive_info.ata_identify.security_status.security_supported) {
      auto const& val = drive_info.ata_identify.normal_security_erase_unit;
      ests.security_erase = ataEstToSeconds(val.time_required, val.extended_time_reported);
    }
    if (drive_info.ata_identify.security_status.enhanced_security_erase_supported) {
      auto const& val = drive_info.ata_identify.enhanced_security_erase_unit;
      ests.enhanced_security_erase = ataEstToSeconds(val.time_required, val.extended_time_reported);
    }
    return { DPARME_OK, 0, 0, ests };
  }
  else if (driving_type == kDrivingNvme) {
    dr = doNvmeGetSanitizeStatus(nullptr, &ests);
    return { dr, ests };
  }
  return { DPARME_NOT_SUPPORTED, 0, {} };
}

DparmResult DriveHandleBase::doNvmeGetSanitizeStatus(SanitizeCmdResult* sanitize_result, SanitizeEstimates* estimates) {
  nvme::nvme_sanitize_log_page_t page = {0};
  DparmResult cmd_result = doNvmeGetLogPage(0, 0x81, false, sizeof(page), &page);
  if (cmd_result.isOk()) {
    if (sanitize_result) {
      switch (page.status & 0x7U) {
        case 0:
          sanitize_result->sanitize_states = kSanitizeIdle;
          break;
        case 1:
          sanitize_result->sanitize_states = (SanitizeState) (kSanitizeIdle | kSanitizeOperationSucceeded);
          break;
        case 2:
          sanitize_result->sanitize_states = kSanitizeOperationInProgress;
          sanitize_result->progress = ((float) page.progress) / 65536.0f;
          break;
        case 3:
          sanitize_result->sanitize_states = (SanitizeState) (kSanitizeIdle | kSanitizeOperationFailed);
          break;
      }
    }
    if (estimates) {
      estimates->block_erase_time = nvmeEstToSeconds(page.est_block_erase_time);
      estimates->crypto_erase_time = nvmeEstToSeconds(page.est_crypto_erase_time);
      estimates->overwrite_time = nvmeEstToSeconds(page.est_overwrite_time);
    }
  }
  return cmd_result;
}

} // namespace dparm
} // namespace jcu
