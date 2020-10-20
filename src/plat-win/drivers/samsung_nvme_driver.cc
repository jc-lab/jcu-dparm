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
  std::string device_path_;
  HANDLE handle_;

 public:
  std::string getDriverName() const override {
    return "WindowsSamsungNvmeDriver";
  }

  HANDLE getHandle() const override {
    return handle_;
  }

  void close() override {
    if (handle_ && (handle_ != INVALID_HANDLE_VALUE)) {
      ::CloseHandle(handle_);
      handle_ = nullptr;
    }
  }

  const std::string &getDevicePath() const override {
    return device_path_;
  }
};

DparmReturn<std::unique_ptr<WindowsDriverHandle>> SamsungNvmeDriver::open(const char *path) {
  return { DPARME_SYS, 0 };
}

DparmReturn<std::unique_ptr<WindowsDriverHandle>> SamsungNvmeDriver::open(const WindowsPhysicalDrive &drive_info) {
  return this->open(drive_info.physical_disk_path.c_str());
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
