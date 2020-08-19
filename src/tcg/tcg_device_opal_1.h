/**
 * @file	tcg_device_opal_1.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_TCG_TCG_DEVICE_OPAL_1_H_
#define JCU_DPARM_SRC_TCG_TCG_DEVICE_OPAL_1_H_

#include "tcg_device_opal_base.h"

namespace jcu {
namespace dparm {
namespace tcg {

class TcgDeviceOpal1 : public TcgDeviceOpalBase {
 public:
  TcgDeviceOpal1(DriveHandleBase *drive_handle);
  TcgDeviceType getDeviceType() const override;
  uint16_t getBaseComId() const override;
  uint16_t getNumComIds() const override;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_TCG_TCG_DEVICE_OPAL_1_H_
