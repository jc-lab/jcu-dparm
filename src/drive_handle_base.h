/**
 * @file	drive_handle_base.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_DRIVE_HANDLE_BASE_H_
#define JCU_DPARM_SRC_DRIVE_HANDLE_BASE_H_

#include <string>

#include <jcu-dparm/drive_handle.h>
#include <jcu-dparm/ata_types.h>
#include <jcu-dparm/tcg/tcg_types.h>

#include "drive_driver_handle.h"

namespace jcu {
namespace dparm {

class DriveHandleBase : public DriveHandle {
 protected:
  DriveFactoryOptions options_;

  std::string device_path_;
  DriveInfo drive_info_;

  virtual DriveDriverHandle *getDriverHandle() const = 0;

  const DriveInfo& getDriveInfo() const override {
    return drive_info_;
  }

  const std::vector<unsigned char> getAtaIdentifyDeviceRaw() const {
    return getDriverHandle()->getAtaIdentifyDeviceBuf();
  }

  const std::vector<unsigned char> getNvmeIdentifyDeviceRaw() const {
    return getDriverHandle()->getNvmeIdentifyDeviceBuf();
  }

  DriveHandleBase(const DriveFactoryOptions& options, const std::string& device_path, const DparmResult& open_result);

  int dbgprintf(const char* fmt, ...);

  std::string readString(const unsigned char *buffer, int length, bool trim_right = true);

  std::string readStringRange(const unsigned char *buffer, int begin, int end, bool trim_right = true);

  std::string fixAtaStringOrder(const unsigned char *buffer, int length, bool trim_right = true);

  std::string trimString(const std::string &input);

  uint64_t fixAtaUint64Order(const void *buffer);

  void afterOpen();
  int parseIdentifyDevice();

 public:
  DparmResult doTaskfileCmd(
      int rw,
      int dma,
      ata::ata_tf_t *tf,
      void *data,
      unsigned int data_bytes,
      unsigned int timeout_secs
  ) override {
    return getDriverHandle()->doTaskfileCmd(rw, dma, tf, data, data_bytes, timeout_secs);
  }

  DparmReturn<int> doNvmeAdminPassthru(nvme::nvme_admin_cmd_t* cmd) override {
    return getDriverHandle()->doNvmeAdminPassthru(cmd);
  }

  DparmReturn<int> doNvmeIoPassthru(nvme::nvme_passthru_cmd_t* cmd) override {
    return getDriverHandle()->doNvmeIoPassthru(cmd);
  }

  DparmReturn<int> doNvmeIo(nvme::nvme_user_io_t* io) override {
    return getDriverHandle()->doNvmeIo(io);
  }

  DparmResult doNvmeGetLogPageCmd(
      uint32_t nsid, uint8_t log_id,
      uint8_t lsp, uint64_t lpo, uint16_t lsi,
      bool rae, uint8_t uuid_ix,
      uint32_t data_len, void *data) override;

  DparmResult doNvmeGetLogPage(uint32_t nsid, uint8_t log_id, bool rae, uint32_t data_len, void *data) override;

  DparmReturn<SanitizeCmdResult> doSanitizeCmd(const SanitizeOptions& options) override;
  DparmReturn<SanitizeEstimates> getSanitizeEstimates() override;
  DparmResult doNvmeGetSanitizeStatus(SanitizeCmdResult* sanitize_result, SanitizeEstimates* estimates);

  DparmResult doSecurityCommand(int rw, int dma, uint8_t protocol, uint16_t com_id, void *buffer, uint32_t len) override;

  DparmResult tcgDiscovery0();

// public:
//  DrivingType getDrivingType() override {
//    return driving_type_;
//  }
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_DRIVE_HANDLE_BASE_H_
