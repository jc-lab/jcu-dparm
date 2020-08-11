/**
 * @file	scsi_driver.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_WIN_DRIVERS_SCSI_DRIVER_H_
#define JCU_DPARM_SRC_PLAT_WIN_DRIVERS_SCSI_DRIVER_H_

#include "../driver_base.h"

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

class ScsiDriver : public DriverBase {
 public:
  DparmReturn<std::unique_ptr<WindowsDriverHandle>> open(const char *path) override;
  DparmReturn<std::unique_ptr<WindowsDriverHandle>> open(const WindowsPhysicalDrive &drive_info) override;

  static DparmResult doSecurityCommandImpl(HANDLE handle, uint8_t protocol, uint16_t com_id, int rw, void *buffer, uint32_t len, int timeout_secs);
};

} // naemspace drivers
} // namespace plat_win
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_WIN_DRIVERS_SCSI_DRIVER_H_
