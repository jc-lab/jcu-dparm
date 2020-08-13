/**
 * @file	drive_handle.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_DRIVE_HANDLE_H_
#define JCU_DPARM_DRIVE_HANDLE_H_

#include <vector>

#include "./types.h"
#include "./err.h"

namespace jcu {
namespace dparm {

class DriveHandle {
 public:
  virtual bool isOpen() const = 0;
  virtual DparmResult getError() const = 0;
  virtual void close() = 0;
  virtual const DriveInfo& getDriveInfo() const = 0;

  virtual const std::vector<unsigned char> getAtaIdentifyDeviceRaw() const = 0;
  virtual const std::vector<unsigned char> getNvmeIdentifyDeviceRaw() const = 0;

  /* ATA Low-level methods */
  virtual DparmResult doTaskfileCmd(
      int rw,
      int dma,
      ata::ata_tf_t *tf,
      void *data,
      unsigned int data_bytes,
      unsigned int timeout_secs
  ) = 0;

  /* NVMe Low-level methods */
  virtual DparmReturn<int> doNvmeAdminPassthru(nvme::nvme_admin_cmd_t* cmd) = 0;
  virtual DparmReturn<int> doNvmeIoPassthru(nvme::nvme_passthru_cmd_t* cmd) = 0;
  virtual DparmReturn<int> doNvmeIo(nvme::nvme_user_io_t* io) = 0;
  virtual DparmResult doNvmeGetLogPageCmd(
      uint32_t nsid, uint8_t log_id,
      uint8_t lsp, uint64_t lpo, uint16_t lsi,
      bool rae, uint8_t uuid_ix,
      uint32_t data_len, void *data) = 0;
  virtual DparmResult doNvmeGetLogPage(uint32_t nsid, uint8_t log_id, bool rae, uint32_t data_len, void *data) = 0;

  /* Security Low-level methods */
  virtual DparmResult doSecurityCommand(int rw, int dma, uint8_t protocol, uint16_t com_id, void *buffer, uint32_t len) = 0;

  /* High-level methods */
  virtual DparmReturn<SanitizeCmdResult> doSanitizeCmd(const SanitizeOptions& options) = 0;
  virtual DparmReturn<SanitizeEstimates> getSanitizeEstimates() = 0;
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_DRIVE_HANDLE_H_
