/**
 * @file	nvme_driver.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/10
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_LINUX_DRIVERS_NVME_DRIVER_H_
#define JCU_DPARM_SRC_PLAT_LINUX_DRIVERS_NVME_DRIVER_H_

#include "../driver_base.h"

namespace jcu {
namespace dparm {
namespace plat_linux {
namespace drivers {

class NvmeDriver : public DriverBase {
 public:
  NvmeDriver(const DriveFactoryOptions& options) : DriverBase(options) {}
  DparmReturn<std::unique_ptr<LinuxDriverHandle>> open(const char *path) override;
};

} // naemspace drivers
} // namespace plat_linux
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_LINUX_DRIVERS_NVME_DRIVER_H_
