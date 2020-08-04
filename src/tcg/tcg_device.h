/**
 * @file	tcg_device.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_TCG_TCG_DEVICE_H_
#define JCU_DPARM_SRC_TCG_TCG_DEVICE_H_

#include <jcu-dparm/err.h>

namespace jcu {
namespace dparm {

class DriveHandle;

namespace tcg {

class DtaCommand;
class DtaResponse;

class TcgDevice {
 public:
  TcgDevice(DriveHandle* drive_handle) {}
  virtual ~TcgDevice() {}
  virtual DparmResult exec(const DtaCommand &cmd, DtaResponse& resp, uint8_t protocol = 0x01) = 0;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_TCG_TCG_DEVICE_H_
