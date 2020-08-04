/**
 * @file	drive_driver_handle.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_DRIVE_DRIVER_HANDLE_H_
#define JCU_DPARM_DRIVE_DRIVER_HANDLE_H_

#include <stdint.h>
#include <vector>

#include <jcu-dparm/types.h>
#include <jcu-dparm/ata_types.h>

namespace jcu {
namespace dparm {

class DriveDriverHandle {
 protected:
  DrivingType driving_type_;
  std::vector<unsigned char> ata_identify_device_buf_;
  std::vector<unsigned char> nvme_identify_device_buf_;

 public:
  virtual ~DriveDriverHandle() {}
  virtual void close() = 0;
  virtual DparmResult doTaskfileCmd(
      int rw,
      int dma,
      ata::ata_tf_t *tf,
      void *data,
      unsigned int data_bytes,
      unsigned int timeout_secs
  ) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  DrivingType getDrivingType() const {
    return driving_type_;
  }

  bool isAtaCommandSupport() const {
    return driving_type_ == kDrivingAtapi;
  }

  bool isNvmeCommandSupport() const {
    return driving_type_ == kDrivingNvme;
  }

  const std::vector<unsigned char> &getAtaIdentifyDeviceBuf() const {
    return ata_identify_device_buf_;
  }

  const std::vector<unsigned char> &getNvmeIdentifyDeviceBuf() const {
    return nvme_identify_device_buf_;
  }
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_DRIVE_DRIVER_HANDLE_H_
