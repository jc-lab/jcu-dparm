/**
 * @file	nvme_win_driver.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 * @brief     nvmewin driver or intel nvme driver
 */

#ifndef JCU_DPARM_SRC_PLAT_WIN_DRIVERS_NVME_WIN_DRIVER_H_
#define JCU_DPARM_SRC_PLAT_WIN_DRIVERS_NVME_WIN_DRIVER_H_

#include "../driver_base.h"

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

class NvmeWinDriver : public DriverBase {
 public:
  DparmReturn<std::unique_ptr<WindowsDriverHandle>> open(const char *path) override;
  DparmReturn<std::unique_ptr<WindowsDriverHandle>> open(const WindowsPhysicalDrive &drive_info) override;
};

} // naemspace drivers
} // namespace plat_win
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_WIN_DRIVERS_NVME_WIN_DRIVER_H_
