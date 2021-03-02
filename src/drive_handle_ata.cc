/**
 * @file	drive_handle_ata.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2021/03/02
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "drive_handle_base.h"

#include <jcu-dparm/ata_types.h>
#include <jcu-dparm/ata_utils.h>

#include "intl_utils.h"

namespace jcu {
namespace dparm {

uint64_t DriveHandleBase::getAtaLbaCapacity() {
  auto driver_handle = getDriverHandle();
  const auto& ata_identify = drive_info_.ata_identify;
  uint64_t capacity;

  if (driver_handle->getDrivingType() != kDrivingAtapi) {
    return 0;
  }

  if (ata_identify.capabilities.lba_supported) {
    capacity = ata_identify.user_addressable_sectors;
    if (
        ata_identify.command_set_support.word_valid83 == 0x01 &&
            ata_identify.command_set_active.big_lba
        ) {
      capacity = ((uint64_t)ata_identify.max48bit_lba[0]) | ((uint64_t)ata_identify.max48bit_lba[1]) << 32;
    }
  } else {
    capacity = ata_identify.current_sector_capacity;
  }

  return capacity;
}

DparmReturn<std::vector<uint16_t>> DriveHandleBase::readDcoIdentify() {
  auto driver_handle = getDriverHandle();
  DparmResult dr;
  ata::ata_tf_t tf;
  std::vector<uint16_t> data_buf(512 / sizeof(uint16_t));

  if (driver_handle->getDrivingType() != kDrivingAtapi) {
    return {DPARME_NOT_SUPPORTED, 0 };
  }

  ata::tf_init(&tf, ata::ATA_OP_DCO, 0, 1);
  tf.lob.feat = 0xc2;

  dr = driver_handle->doTaskfileCmd(0, -1, &tf, data_buf.data(), data_buf.size() * sizeof(uint16_t), 15);

  return { dr, data_buf };
}

DparmReturn<uint64_t> DriveHandleBase::readNativeMaxSectors() {
  auto driver_handle = getDriverHandle();
  const auto& ata_identify = drive_info_.ata_identify;
  DparmResult dr;
  ata::ata_tf_t tf;
  uint64_t max_sectors = 0;

  if (driver_handle->getDrivingType() != kDrivingAtapi) {
    return {DPARME_NOT_SUPPORTED, 0 };
  }

  if (
      ata_identify.command_set_support.big_lba &&
      ata_identify.command_set_support.word_valid83 == 0x01 &&
      ata_identify.command_set_active.big_lba
      ) {
    ata::tf_init(&tf, ata::ATA_OP_READ_NATIVE_MAX_EXT, 0, 0);
    tf.dev = 0x40;
  } else {
    ata::tf_init(&tf, ata::ATA_OP_READ_NATIVE_MAX, 0, 0);
    tf.dev = 0x01;
  }

  dr = driver_handle->doTaskfileCmd(0, -1, &tf, NULL, 0, 15);
  if (tf.command == ata::ATA_OP_READ_NATIVE_MAX) {
    max_sectors = ((((uint64_t)tf.dev) & 0x0f) << 24) | (((uint64_t)tf.lob.lbah) << 16) | (((uint64_t)tf.lob.lbam) << 8) | (((uint64_t)tf.lob.lbal)) + 1;
  } else {
    max_sectors = ((((uint64_t)tf.hob.lbah) << 16) | (((uint64_t)tf.hob.lbam) << 8) | (((uint64_t)tf.hob.lbal))) << 24;
    max_sectors |= (((uint64_t)tf.lob.lbah) << 16) | (((uint64_t)tf.lob.lbam) << 8) | (((uint64_t)tf.lob.lbal)) + 1;
  }

  return { dr, max_sectors };
}

} // namespace dparm
} // namespace jcu
