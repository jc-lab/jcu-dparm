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
#include <memory>

#include <jcu-dparm/drive_handle.h>
#include <jcu-dparm/ata_types.h>
#include <jcu-dparm/tcg/tcg_types.h>
#include <jcu-dparm/tcg/tcg_device.h>

#include "drive_driver_handle.h"

namespace jcu {
namespace dparm {

class DriveHandleBase : public DriveHandle {
 protected:
  DriveFactoryOptions options_;

  std::string device_path_;
  DriveInfo drive_info_;

  std::unique_ptr<tcg::TcgDevice> tcg_device_;

  virtual DriveDriverHandle *getDriverHandle() const = 0;

  const std::vector<unsigned char> getAtaIdentifyDeviceRaw() const {
    return getDriverHandle()->getAtaIdentifyDeviceBuf();
  }

  const std::vector<unsigned char> getNvmeIdentifyDeviceRaw() const {
    return getDriverHandle()->getNvmeIdentifyDeviceBuf();
  }

  DriveHandleBase(const DriveFactoryOptions& options, const std::string& device_path, const DparmResult& open_result);

  int dbgprintf(const char* fmt, ...);

  void afterOpen();
  int parseIdentifyDevice();

 public:
  std::string getDriverName() const {
    return getDriverHandle()->getDriverName();
  }

  const DriveInfo& getDriveInfo() const override {
    return drive_info_;
  }

  bool driverIsTaskfileCmdSupported() const override {
    return getDriverHandle()->driverIsTaskfileCmdSupported();
  }

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

  bool driverIsAtaCmdSupported() const override {
    return getDriverHandle()->driverIsAtaCmdSupported();
  }

  DparmResult doAtaCmd(
      int rw,
      unsigned char* cdb,
      unsigned int cdb_bytes,
      void *data,
      unsigned int data_bytes,
      int pack_id,
      unsigned int timeout_secs,
      unsigned char *sense_buf,
      unsigned int sense_buf_bytes
  ) override {
    return getDriverHandle()->doAtaCmd(rw, cdb, cdb_bytes, data, data_bytes, pack_id, timeout_secs, sense_buf, sense_buf_bytes);
  }

  bool driverIsNvmeAdminPassthruSupported() const {
    return getDriverHandle()->driverIsNvmeAdminPassthruSupported();
  }

  DparmReturn<int> doNvmeAdminPassthru(nvme::nvme_admin_cmd_t* cmd) override {
    return getDriverHandle()->doNvmeAdminPassthru(cmd);
  }

  bool driverIsNvmeIoPassthruSupported() const override {
    return getDriverHandle()->driverIsNvmeIoPassthruSupported();
  }

  DparmReturn<int> doNvmeIoPassthru(nvme::nvme_passthru_cmd_t* cmd) override {
    return getDriverHandle()->doNvmeIoPassthru(cmd);
  }

  bool driverIsNvmeIoSupported() const override {
    return getDriverHandle()->driverIsNvmeIoSupported();
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

  tcg::TcgDevice* getTcgDevice() override;

  DparmReturn<nvme::nvme_smart_log_page_t> readNvmeSmartLogPage() override;
  DparmReturn<SMARTStatus> readAtaSmartStatus() override;

// public:
//  DrivingType getDrivingType() override {
//    return driving_type_;
//  }
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_DRIVE_HANDLE_BASE_H_
