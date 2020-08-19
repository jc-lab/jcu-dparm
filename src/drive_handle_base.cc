/**
 * @file	drive_handle_base.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <string.h>
#include <stdarg.h>

#include "drive_handle_base.h"

#include "intl_utils.h"

#include "tcg/tcg_device_general.h"
#include "tcg/tcg_device_opal_1.h"
#include "tcg/tcg_device_opal_2.h"
#include "tcg/tcg_device_enterprise.h"

namespace jcu {
namespace dparm {

DriveHandleBase::DriveHandleBase(const DriveFactoryOptions& options, const std::string& device_path, const DparmResult& open_result)
    : options_(options), device_path_(device_path)
{
  drive_info_.device_path = device_path_;
  drive_info_.open_result = open_result;
}

int DriveHandleBase::dbgprintf(const char* fmt, ...) {
  std::vector<char> buffer(strlen(fmt) + 256);
  va_list args;
  va_start(args, fmt);
  int length = vsnprintf(buffer.data(), buffer.size(), fmt, args);
  va_end(args);
  if (options_.debug_puts) {
    return options_.debug_puts(std::string(buffer.data(), length));
  }
  return 0;
}

void DriveHandleBase::afterOpen() {
  auto driver_handle = getDriverHandle();
  drive_info_.driving_type = driver_handle->getDrivingType();
  parseIdentifyDevice();
  if (drive_info_.serial.empty()) {
    auto res = driver_handle->inquiryDeviceInfo();
    if (res.isOk()) {
      drive_info_.serial = res.value.drive_serial_number;
      drive_info_.model = res.value.product_identification;
    }
  }
  tcgDiscovery0();
}

int DriveHandleBase::parseIdentifyDevice() {
  auto driver_handle = getDriverHandle();

  driver_handle->mergeDriveInfo(drive_info_);

  if (driver_handle->getDrivingType() == kDrivingAtapi) {
    ata::ata_identify_device_data_t data;
    auto raw = driver_handle->getAtaIdentifyDeviceBuf();
    int ssd_check_weight = 0;

    if (raw.size() < sizeof(data)) {
      return 1;
    }

    memcpy(&data, raw.data(), sizeof(data));

    drive_info_.ata_identify = data;

    if (data.nominal_media_rotation_rate == 0 || data.nominal_media_rotation_rate == 1) {
      ssd_check_weight++;
    }
    if (data.data_set_management_feature.supports_trim) {
      ssd_check_weight++;
    }

    drive_info_.ssd_check_weight = ssd_check_weight;
    drive_info_.is_ssd = ssd_check_weight > 0;

    drive_info_.model = intl::trimString(intl::fixAtaStringOrder(data.model_number, sizeof(data.model_number), true));
    drive_info_.firmware_revision = intl::trimString(intl::fixAtaStringOrder(data.firmware_revision, sizeof(data.firmware_revision), true));
    for (int i = 0; i < sizeof(drive_info_.raw_serial); i++) {
      drive_info_.raw_serial[i] = data.serial_number[i + 1];
      drive_info_.raw_serial[i + 1] = data.serial_number[i];
    }
    drive_info_.serial = intl::trimString(intl::fixAtaStringOrder(data.serial_number, sizeof(data.serial_number), true));
    if (data.sanitize_feature_supported) {
      if (data.security_status.security_enabled) {
        drive_info_.support_sanitize_block_erase = data.block_erase_ext_command_supported ? 1 : 0;
        drive_info_.support_sanitize_crypto_erase = data.crypto_scramble_ext_command_supported ? 1 : 0;
        drive_info_.support_sanitize_overwrite = data.overwrite_ext_command_supported ? 1 : 0;
      } else {
        drive_info_.support_sanitize_block_erase = data.block_erase_ext_command_supported ? 2 : 0;
        drive_info_.support_sanitize_crypto_erase = data.crypto_scramble_ext_command_supported ? 2 : 0;
        drive_info_.support_sanitize_overwrite = data.overwrite_ext_command_supported ? 2 : 0;
      }
    }else{
      drive_info_.support_sanitize_block_erase = 0;
      drive_info_.support_sanitize_crypto_erase = 0;
      drive_info_.support_sanitize_overwrite = 0;
    }
  }else if (driver_handle->getDrivingType() == kDrivingNvme) {
    nvme::nvme_identify_controller_t data;
    auto raw = driver_handle->getNvmeIdentifyDeviceBuf();

    if (raw.size() < 4096) {
      return 1;
    }

    memcpy(&data, raw.data(), sizeof(data));
    drive_info_.nvme_identify_ctrl = data;

    memcpy(drive_info_.raw_serial, data.sn, sizeof(drive_info_.raw_serial));
    drive_info_.serial = intl::readStringRange((const unsigned char *) data.sn, 0, sizeof(data.sn), true);
    drive_info_.model = intl::readStringRange((const unsigned char *)data.mn, 0, sizeof(data.mn), true);
    drive_info_.firmware_revision = intl::readStringRange((const unsigned char *)data.fr, 0, sizeof(data.fr), true);

    drive_info_.ssd_check_weight = 0;
    drive_info_.is_ssd = true;

    {
      drive_info_.support_sanitize_crypto_erase = (data.sanicap & 0x01) != 0;
      drive_info_.support_sanitize_block_erase = (data.sanicap & 0x02) != 0;
      drive_info_.support_sanitize_overwrite = (data.sanicap & 0x04) != 0;
    }
  }

  return 0;
}

DparmResult DriveHandleBase::doNvmeGetLogPageCmd(
    uint32_t nsid, uint8_t log_id,
    uint8_t lsp, uint64_t lpo, uint16_t lsi,
    bool rae, uint8_t uuid_ix,
    uint32_t data_len, void *data) {
  uint32_t numd = (data_len >> 2U) - 1;
  uint32_t numdh = (numd >> 16U) & 0xffffU;
  uint32_t numdl = numd & 0xffffU;
  uint32_t cdw10 = log_id | (numdl << 16U) | (rae ? 1U << 15U : 0) | (((uint32_t)lsp) << 8U);

  nvme::nvme_admin_cmd_t cmd = { 0 };
  cmd.opcode = nvme::NVME_ADMIN_OP_GET_LOG_PAGE;
  cmd.nsid = nsid;
  cmd.addr = (uint64_t)data;
  cmd.data_len = data_len;
  cmd.cdw10 = cdw10;
  cmd.cdw11 = numdh | (lsi << 16U);
  cmd.cdw12 = (uint32_t)(lpo);
  cmd.cdw13 = (uint32_t)(lpo >> 32U);
  cmd.cdw14 = uuid_ix;

  return getDriverHandle()->doNvmeAdminPassthru(&cmd);
}

DparmResult DriveHandleBase::doNvmeGetLogPage(uint32_t nsid, uint8_t log_id, bool rae, uint32_t data_len, void *data) {
  uint32_t offset = 0, xfer_len = data_len;
  uint8_t *ptr = (uint8_t*)data;
  DparmResult ret;

  /*
   * 4k is the smallest possible transfer unit, so by
   * restricting ourselves for 4k transfers we avoid having
   * to check the MDTS value of the controller.
   */
  do {
    xfer_len = data_len - offset;
    if (xfer_len > 4096)
      xfer_len = 4096;

    ret = doNvmeGetLogPageCmd(nsid, log_id, nvme::NVME_NO_LOG_LSP, offset, 0, rae, 0, xfer_len, ptr);
    if (!ret.isOk())
      break;

    offset += xfer_len;
    ptr += xfer_len;
  } while (offset < data_len);

  return ret;
}

DparmResult DriveHandleBase::doSecurityCommand(int rw, int dma, uint8_t protocol, uint16_t com_id, void *buffer, uint32_t len) {
  auto driver_handle = getDriverHandle();
  DparmResult res = driver_handle->doSecurityCommand(protocol, com_id, rw, buffer, len, 5);
  if (res.isOk()) {
    return res;
  }
  if (driver_handle->isNvmeCommandSupport()) {
    nvme::nvme_admin_cmd_t cmd = { 0 };
    cmd.opcode = (uint8_t)(rw ? nvme::NVME_ADMIN_OP_SECURITY_SEND : nvme::NVME_ADMIN_OP_SECURITY_RECV);
    cmd.addr = (uint64_t)buffer;
    cmd.data_len = len;
    cmd.cdw10 = ((((uint32_t)protocol) & 0xff) << 24) | ((((uint32_t)com_id) & 0xffff) << 8);
    cmd.cdw11 = len;
    return driver_handle->doNvmeAdminPassthru(&cmd);
  } else if (driver_handle->isAtaCommandSupport()) {
    ata::ata_tf_t tf = {0};

    tf.lob.feat = protocol;
    tf.lob.nsect = len / 512;
    if (rw) {
      tf.command = dma ? ata::ATA_OP_TRUSTED_SEND_DMA : ata::ATA_OP_TRUSTED_SEND;
    } else {
      tf.command = dma ? ata::ATA_OP_TRUSTED_RECV_DMA : ata::ATA_OP_TRUSTED_RECV;
    }
    tf.lob.lbam = (uint8_t) com_id;
    tf.lob.lbah = (uint8_t) (com_id >> 8U);
    return this->getDriverHandle()->doTaskfileCmd(rw, dma, &tf, buffer, len, 15);
  }
  return { DPARME_NOT_SUPPORTED, 0 };
}

DparmResult DriveHandleBase::tcgDiscovery0() {
  std::vector<unsigned char> buffer(tcg::MIN_BUFFER_LENGTH + tcg::IO_BUFFER_ALIGNMENT);
  unsigned char *buffer_ptr = (unsigned char *)((uintptr_t)(buffer.data() + tcg::IO_BUFFER_ALIGNMENT) & (uintptr_t)~(tcg::IO_BUFFER_ALIGNMENT - 1));

  DparmResult dr;

  dr = doSecurityCommand(0, 0, 0x01, 0x0001, buffer_ptr, tcg::MIN_BUFFER_LENGTH);
  if (!dr.isOk()) {
    if (dr.code == DPARME_NOT_SUPPORTED) {
      drive_info_.tcg_support = 0;
    } else {
      drive_info_.tcg_support = -1;
    }
    return dr;
  }

  tcg::discovery0_header_t *discovery_header = (tcg::discovery0_header_t *)buffer_ptr;
  const unsigned char *discovery_cptr = buffer_ptr;
  const unsigned char *discovery_dend = buffer_ptr + SWAP32(discovery_header->length);
  const unsigned char *discovery_aend = buffer.data() + buffer.size();

  drive_info_.tcg_support = 1;

  if (discovery_aend < discovery_dend) {
    return { DPARME_ILLEGAL_DATA, 0 };
  }

  discovery_cptr += sizeof(*discovery_header);
  while (discovery_cptr < discovery_dend) {
    union tcg::_discovery0_feature_u *cur = (union tcg::_discovery0_feature_u *)discovery_cptr;
    uint16_t feature_code = SWAP16(cur->basic.feature_code);
    std::vector<unsigned char> item_buffer;

    item_buffer.insert(
        item_buffer.end(),
        discovery_cptr,
        discovery_cptr + cur->basic.length + 4
    );

    switch(feature_code) {
      case tcg::kFcTPer:
        drive_info_.tcg_tper = true;
        break;
      case tcg::kFcLocking:
        drive_info_.tcg_locking = true;
        break;
      case tcg::kFcGeometryReporting:
        drive_info_.tcg_geomerty_reporting = true;
        break;
      case tcg::kFcOpalSscV100:
        drive_info_.tcg_opal_v100 = true;
        break;
      case tcg::kFcOpalSscV200:
        drive_info_.tcg_opal_v200 = true;
        break;
      case tcg::kFcEnterprise:
        drive_info_.tcg_enterprise = true;
        break;
      case tcg::kFcSingleUser:
        drive_info_.tcg_single_user_mode = true;
        break;
      case tcg::kFcDataStore:
        drive_info_.tcg_datastore = true;
        break;
    }

    drive_info_.tcg_raw_features.emplace(feature_code, item_buffer);
    discovery_cptr += cur->basic.length + 4;
  }

  return dr;
}

tcg::TcgDevice *DriveHandleBase::getTcgDevice() {
  if (!tcg_device_) {
    if (drive_info_.tcg_support) {
      if (drive_info_.tcg_opal_v200) {
        tcg_device_.reset(new tcg::TcgDeviceOpal2(this));
      } else if (drive_info_.tcg_opal_v100) {
        tcg_device_.reset(new tcg::TcgDeviceOpal1(this));
      } else if (drive_info_.tcg_enterprise) {
        tcg_device_.reset(new tcg::TcgDeviceEnterprise(this));
      } else {
        tcg_device_.reset(new tcg::TcgDeviceGeneric(this));
      }
    }
  }
  return tcg_device_.get();
}

} // namespace dparm
} // namespace jcu
