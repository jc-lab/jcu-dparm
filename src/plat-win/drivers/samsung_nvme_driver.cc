/**
 * @file	samsung_nvme_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "samsung_nvme_driver.h"
#include "driver_utils.h"

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

class SamsungNvmeDriverHandle : public WindowsDriverHandle {
 private:
  HANDLE handle_;

 public:
  HANDLE getHandle() const override {
    return handle_;
  }

  void close() override {

  }
};

DparmReturn<std::unique_ptr<WindowsDriverHandle>> SamsungNvmeDriver::open(const char *path) {
  return { DPARME_SYS, 0 };
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
