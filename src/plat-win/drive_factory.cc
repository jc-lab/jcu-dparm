/**
 * @file	drive_factory.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <map>
#include <list>

#include <jcu-dparm/drive_factory.h>
#include <jcu-dparm/drive_handle.h>

#include "../drive_driver_handle.h"
#include "../drive_handle_base.h"

#include "driver_base.h"
#include "drivers/scsi_driver.h"
#include "drivers/nvme_win_driver.h"
#include "drivers/samsung_nvme_driver.h"
#include "drivers/windows_nvme_driver.h"

#include "physical_drive_finder.h"

namespace jcu {
namespace dparm {

namespace plat_win {

class Win32DriveHandle : public DriveHandleBase {
 private:
  std::unique_ptr<WindowsDriverHandle> handle_;
  DparmResult last_error_;

 protected:
  DriveDriverHandle *getDriverHandle() const override {
    return handle_.get();
  }

 public:
  Win32DriveHandle(const std::string &path, std::unique_ptr<WindowsDriverHandle> handle, DparmResult open_result)
      : DriveHandleBase(path, open_result), handle_(std::move(handle)), last_error_(open_result) {
  }

  void init() {
    afterOpen();
  }

  bool isOpen() const override {
    return handle_.operator bool();
  }

  DparmResult getError() const override {
    return last_error_;
  }

  void close() override {
    if (isOpen()) {
      handle_->close();
      handle_.reset();
    }
  }
};

class Win32DriveFactory : public DriveFactory {
 private:
  std::list<std::unique_ptr<DriverBase>> drivers_;

 public:
  Win32DriveFactory() {
    drivers_.emplace_back(std::unique_ptr<DriverBase>(new drivers::NvmeWinDriver()));
    drivers_.emplace_back(std::unique_ptr<DriverBase>(new drivers::SamsungNvmeDriver()));
    drivers_.emplace_back(std::unique_ptr<DriverBase>(new drivers::WindowsNvmeDriver()));
    drivers_.emplace_back(std::unique_ptr<DriverBase>(new drivers::ScsiDriver()));
  }

  std::unique_ptr<DriveHandle> open(const char *drive_path) const override {
    DparmReturn<std::unique_ptr<WindowsDriverHandle>> driver_handle;

    for (auto drv_it = drivers_.cbegin(); drv_it != drivers_.cend(); drv_it++) {
      driver_handle = std::move((*drv_it)->open(drive_path));
      if (driver_handle.isOk()) {
        break;
      }
    }

    std::unique_ptr<Win32DriveHandle> drive_handle(new Win32DriveHandle(drive_path, std::move(driver_handle.value), driver_handle));
    if (driver_handle.isOk()) {
      drive_handle->init();
    }
    return std::move(drive_handle);
  }

  int enumDrives(std::list<DriveInfo>& result_list) const override {
    int rc;
    std::list<WindowsPhysicalDrive> devices;
    rc = enumPhysicalDrives(devices);
    if (rc)
      return rc;

    for (auto it = devices.cbegin(); it != devices.cend(); it++) {
      auto handle = open(it->device_path.c_str());
      result_list.emplace_back(handle->getDriveInfo());
    }

    return rc;
  }
};

struct Win32SingletoneLoader {
  Win32SingletoneLoader() {
    DriveFactory::getSystemFactory();
  }
};

static Win32SingletoneLoader _loader();

}; // namespace plat_win

DriveFactory *DriveFactory::getSystemFactory() {
  static plat_win::Win32DriveFactory INSTANCE;
  return &INSTANCE;
}

} // namespace dparm
} // namespace jcu
