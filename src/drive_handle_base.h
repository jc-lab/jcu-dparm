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

  DriveHandleBase(const std::string& device_path, const DparmResult& open_result);

  std::string readString(const unsigned char *buffer, int length, bool trim_right = true);

  std::string readStringRange(const unsigned char *buffer, int begin, int end, bool trim_right = true);

  std::string fixAtaStringOrder(const unsigned char *buffer, int length, bool trim_right = true);

  std::string trimString(const std::string &input);

  uint64_t fixAtaUint64Order(const void *buffer);

  void afterOpen();
  int parseIdentifyDevice();

  DparmReturn<SanitizeCmdResult> doSanitizeCmd(SanitizeFeature feature);

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
