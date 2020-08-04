/**
 * @file	drive_handle_base.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <string.h>

#include "drive_handle_base.h"

#include "intl_utils.h"

namespace jcu {
namespace dparm {

std::string DriveHandleBase::readString(const unsigned char *buffer, int length, bool trim_right) {
  std::string out;
  int out_len = 0;
  out.resize(length);

  for (int i = 0; i < length; i++) {
    out[i] = buffer[i];
    if (out[i])
      out_len++;
    else
      break;
  }

  if (trim_right) {
    while ((out_len > 0) && (out[out_len - 1] == 0x20)) {
      out_len--;
    }
  }

  out.resize(out_len);
  return out;
}

std::string DriveHandleBase::readStringRange(const unsigned char *buffer, int begin, int end, bool trim_right) {
  int length = end - begin + 1;
  return readString(buffer + begin, length, trim_right);
}

std::string DriveHandleBase::fixAtaStringOrder(const unsigned char *buffer, int length, bool trim_right) {
  std::string out;
  int out_len = 0;
  out.resize(length);

  for (int i = 0; i < length; i+= 2) {
    out[i] = buffer[i + 1];
    out[i + 1] = buffer[i];
    if (out[i])
      out_len++;
    else
      break;
    if (out[i + 1])
      out_len++;
    else
      break;
  }

  if (trim_right) {
    while ((out_len > 0) && (out[out_len - 1] == 0x20)) {
      out_len--;
    }
  }

  out.resize(out_len);
  return out;
}

uint64_t DriveHandleBase::fixAtaUint64Order(const void *buffer) {
  const unsigned char *ptr = (const unsigned char *)buffer;
  uint64_t out = 0;
  for(int i=0; i<4; i++) {
    out |= ptr[i * 2] << (i * 2 * 8);
    out |= ptr[i * 2 + 1] << ((i * 2 + 1) * 8);
  }
  return out;
}

void DriveHandleBase::afterOpen() {
  auto driver_handle = getDriverHandle();
  drive_info_.reset();
  drive_info_.driving_type = driver_handle->getDrivingType();
  parseIdentifyDevice();
  opalDiscovery0();
}

int DriveHandleBase::parseIdentifyDevice() {
  auto driver_handle = getDriverHandle();

  if (driver_handle->getDrivingType() == kDrivingAtapi) {
    ata::ata_identify_device_data data;
    auto raw = driver_handle->getAtaIdentifyDeviceBuf();
    int ssd_check_weight = 0;

    if (raw.size() < sizeof(data)) {
      return 1;
    }

    memcpy(&data, raw.data(), sizeof(data));

    drive_info_.ata_major_version = data.major_revision;
    drive_info_.ata_minor_version = data.minor_revision;

    if (data.nominal_media_rotation_rate == 0 || data.nominal_media_rotation_rate == 1) {
      ssd_check_weight++;
    }
    if (data.data_set_management_feature.supports_trim) {
      ssd_check_weight++;
    }

    drive_info_.ssd_check_weight = ssd_check_weight;
    drive_info_.is_ssd = ssd_check_weight > 0;

    drive_info_.model = fixAtaStringOrder(data.model_number, sizeof(data.model_number), true);
    drive_info_.firmware_revision = fixAtaStringOrder(data.firmware_revision, sizeof(data.firmware_revision), true);
    drive_info_.serial = fixAtaStringOrder(data.serial_number, sizeof(data.serial_number), true);
    if (data.sanitize_feature_supported) {
      drive_info_.support_sanitize_block_erase = data.block_erase_ext_command_supported;
      drive_info_.support_sanitize_crypto_erase = data.crypto_scramble_ext_command_supported;
      drive_info_.support_sanitize_overwrite = data.overwrite_ext_command_supported;
    }else{
      drive_info_.support_sanitize_block_erase = false;
      drive_info_.support_sanitize_crypto_erase = false;
      drive_info_.support_sanitize_overwrite = false;
    }
  }else if (driver_handle->getDrivingType() == kDrivingNvme) {
    auto raw = driver_handle->getNvmeIdentifyDeviceBuf();

    if (raw.size() < 4096) {
      return 1;
    }

    drive_info_.nvme_major_version = raw.data()[82]; // 83:82
    drive_info_.nvme_minor_version = raw.data()[81];
    drive_info_.nvme_tertiary_version = raw.data()[80];

    drive_info_.serial = readStringRange(raw.data(), 4, 23, true);
    drive_info_.model = readStringRange(raw.data(), 24, 63, true);
    drive_info_.firmware_revision = readStringRange(raw.data(), 64, 71, true);

    drive_info_.ssd_check_weight = 0;
    drive_info_.is_ssd = true;

    {
      unsigned char sanitize_value = raw.data()[328];
      drive_info_.support_sanitize_crypto_erase = (sanitize_value & 0x01) != 0;
      drive_info_.support_sanitize_block_erase = (sanitize_value & 0) != 0;
      drive_info_.support_sanitize_overwrite = (sanitize_value & 0x04) != 0;
    }
  }

  return 0;
}

DparmResult DriveHandleBase::doOpalCommand(int rw, int dma, uint8_t protocol, uint16_t com_id, void *buffer, uint32_t len) {
  ata::ata_tf_t tf = {0};

  tf.lob.feat = protocol;
  tf.lob.nsect = len / 512;
  if (rw) {
    tf.command = dma ? ata::ATA_OP_TRUSTED_SEND_DMA : ata::ATA_OP_TRUSTED_SEND;
  } else {
    tf.command = dma ? ata::ATA_OP_TRUSTED_RECV_DMA : ata::ATA_OP_TRUSTED_RECV;
  }
  tf.lob.lbam = (uint8_t)com_id;
  tf.lob.lbah = (uint8_t)(com_id >> 8U);
  return this->getDriverHandle()->doTaskfileCmd(rw, dma, &tf, buffer, len, 15);
}

void HEX_DUMPS(const void *data, int length) {
  const unsigned char *u = (const unsigned char*)data;
  for(int i=0; i<length; i++) {
    printf("%02x ", u[i]);
  }
  printf("\n");
}

DparmResult DriveHandleBase::opalDiscovery0() {
  std::vector<unsigned char> buffer(tcg::MIN_BUFFER_LENGTH + tcg::IO_BUFFER_ALIGNMENT);
  unsigned char *buffer_ptr = (unsigned char *)((uintptr_t)(buffer.data() + tcg::IO_BUFFER_ALIGNMENT) & (uintptr_t)~(tcg::IO_BUFFER_ALIGNMENT - 1));

  DparmResult dr;

  dr = doOpalCommand(0, 0, 0x01, 0x0001, buffer_ptr, tcg::MIN_BUFFER_LENGTH);
  if (!dr.isOk()) {
    return dr;
  }

  tcg::discovery0_header_t *discovery_header = (tcg::discovery0_header_t *)buffer_ptr;
  const unsigned char *discovery_cptr = buffer_ptr;
  const unsigned char *discovery_dend = buffer_ptr + SWAP32(discovery_header->length);
  const unsigned char *discovery_aend = buffer.data() + buffer.size();

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

  printf("doOpalCommand : %d / %d\n", dr.code, dr.sys_error);



  HEX_DUMPS(buffer.data(), buffer.size());

  return dr;
}

} // namespace dparm
} // namespace jcu
