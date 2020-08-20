/**
 * @file	tcg_device_generic.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_TCG_TCG_DEVICE_GENERIC_H_
#define JCU_DPARM_SRC_TCG_TCG_DEVICE_GENERIC_H_

#include <jcu-dparm/tcg/tcg_device.h>

namespace jcu {
namespace dparm {

class DriveHandleBase;

namespace tcg {

class TcgDeviceGeneric : public TcgDevice {
 protected:
  DriveHandleBase* drive_handle_;

 public:
  TcgDeviceGeneric(DriveHandleBase *drive_handle);
  DriveHandle* getDriveHandle() const override;
  TcgDeviceType getDeviceType() const override;
  bool isAnySSC() const override;
  bool isLockingSupported() const override;
  bool isLockingEnabled() const override;
  bool isLocked() const override;
  bool isMBREnabled() const override;
  bool isMBRDone() const override;
  bool isMediaEncryption() const override;
  uint16_t getBaseComId() const override;
  uint16_t getNumComIds() const override;
  std::unique_ptr<TcgSession> createSession() override;
  std::unique_ptr<TcgCommand> createCommand() override;
  std::unique_ptr<TcgResponse> createResponse() override;
  DparmResult exec(const TcgCommand &cmd, TcgResponse &resp, uint8_t protocol) override;
  DparmReturn<OpalStatusCode> revertTPer(const std::string &password, uint8_t is_psid, uint8_t is_admin_sp) override;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_TCG_TCG_DEVICE_GENERIC_H_
