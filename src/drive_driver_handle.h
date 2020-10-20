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
#include <jcu-dparm/nvme_types.h>

namespace jcu {
namespace dparm {

struct InquiryDeviceResult {
  std::string vendor_identification;
  std::string product_identification;
  std::string product_revision_level;
  std::string drive_serial_number;
};

class DriveDriverHandle {
 protected:
  DrivingType driving_type_;
  std::vector<unsigned char> ata_identify_device_buf_;
  std::vector<unsigned char> nvme_identify_device_buf_;

 public:
  virtual ~DriveDriverHandle() {}
  virtual void close() = 0;

  virtual std::string getDriverName() const = 0;

  virtual void mergeDriveInfo(DriveInfo& drive_info) const = 0;

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

  virtual DparmReturn<InquiryDeviceResult> inquiryDeviceInfo() {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  virtual bool driverIsAtaCmdSupported() const {
    return false;
  }

  virtual DparmResult doAtaCmd(
      int rw,
      unsigned char* cdb,
      unsigned int cdb_bytes,
      void *data,
      unsigned int data_bytes,
      int pack_id,
      unsigned int timeout_secs,
      unsigned char *sense_buf,
      unsigned int sense_buf_bytes
      ) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  virtual bool driverIsTaskfileCmdSupported() const {
    return false;
  }

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

  virtual bool driverIsNvmeAdminPassthruSupported() const {
    return false;
  }

  virtual DparmReturn<int> doNvmeAdminPassthru(nvme::nvme_admin_cmd_t* cmd) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  virtual bool driverIsNvmeIoPassthruSupported() const {
    return false;
  }

  virtual DparmReturn<int> doNvmeIoPassthru(nvme::nvme_passthru_cmd_t* cmd) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  virtual bool driverIsNvmeIoSupported() const {
    return false;
  }

  virtual DparmReturn<int> doNvmeIo(nvme::nvme_user_io_t* io) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  virtual DparmResult doSecurityCommand(uint8_t protocol, uint16_t com_id, int rw, void *buffer, uint32_t len, int timeout) {
    return { DPARME_NOT_IMPL, 0 };
  }
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_DRIVE_DRIVER_HANDLE_H_
