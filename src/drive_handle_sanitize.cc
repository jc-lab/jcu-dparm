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
#include <jcu-dparm/ata_utils.h>

namespace jcu {
namespace dparm {

DparmReturn<SanitizeCmdResult> DriveHandleBase::doSanitizeCmd(SanitizeFeature feature) {
  auto driver_handle = getDriverHandle();
  DparmResult dr;
  if (driver_handle->getDrivingType() == kDrivingAtapi) {
    ata::ata_tf_t tf;
    uint64_t lba = 0;

    // lba, unsigned int nsect
    ata::tf_init(&tf, ata::ATA_OP_SANITIZE, 0, 0);
    switch (feature) {
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
        lba = ata::SANITIZE_OVERWRITE_KEY;
        break;
    }
    tf.lob.lbal    = lba;
    tf.lob.lbam    = lba >>  8;
    tf.lob.lbah    = lba >> 16;
    tf.hob.lbal    = lba >> 24;
    tf.hob.lbam    = lba >> 32;
    tf.hob.lbah    = lba >> 40;
    tf.is_lba48 = 1;
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

    return { DPARME_OK, 0, {
        tf.hob.nsect,
        (SanitizeState)sanitize_state,
        progress
    } };
  }
  return { DPARME_NOT_SUPPORTED, 0, {} };
}

} // namespace dparm
} // namespace jcu
