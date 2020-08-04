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

  virtual DparmResult doOpalCommand(int rw, int dma, uint8_t protocol, uint16_t com_id, void *buffer, uint32_t len) = 0;

  virtual DparmReturn<SanitizeCmdResult> doSanitizeCmd(SanitizeFeature feature) = 0;

//  virtual int doTaskfileCmd(int rw, int dma, struct ata_tf *tf, void *data, unsigned int data_bytes, unsigned int timeout_secs) = 0;
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_DRIVE_HANDLE_H_
