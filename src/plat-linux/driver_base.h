/**
 * @file	driver_base.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_LINUX_DRIVER_BASE_H_
#define JCU_DPARM_SRC_PLAT_LINUX_DRIVER_BASE_H_

#include <memory>
#include <vector>

#include <jcu-dparm/err.h>

#include "../drive_driver_handle.h"

namespace jcu {
namespace dparm {
namespace plat_linux {

class LinuxDriverHandle : public DriveDriverHandle {
 public:
  virtual ~LinuxDriverHandle() {}
  virtual int getFD() const = 0;
};

class DriverBase {
 public:
  virtual ~DriverBase() {}
  virtual DparmReturn<std::unique_ptr<LinuxDriverHandle>> open(const char* path) = 0;
};

} // namespace plat_win
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_LINUX_DRIVER_BASE_H_