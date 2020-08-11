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

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include <jcu-dparm/drive_factory.h>
#include <jcu-dparm/drive_handle.h>

#include "../drive_driver_handle.h"
#include "../drive_handle_base.h"

#include "driver_base.h"
#include "drivers/sg_driver.h"
#include "drivers/nvme_driver.h"

namespace jcu {
namespace dparm {

namespace plat_linux {

class LinuxDriveHandle : public DriveHandleBase {
 private:
  std::unique_ptr<LinuxDriverHandle> handle_;
  DparmResult last_error_;

 protected:
  DriveDriverHandle *getDriverHandle() const override {
    return handle_.get();
  }

 public:
  LinuxDriveHandle(const std::string& path, std::unique_ptr<LinuxDriverHandle> handle, DparmResult open_result)
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

class LinuxDriveFactory : public DriveFactory {
 private:
  std::list<std::unique_ptr<DriverBase>> drivers_;

 public:
  LinuxDriveFactory() {
    drivers_.emplace_back(std::unique_ptr<DriverBase>(new drivers::NvmeDriver()));
    drivers_.emplace_back(std::unique_ptr<DriverBase>(new drivers::SgDriver()));
  }

  std::unique_ptr<DriveHandle> open(const char *drive_path) const override {
    DparmReturn<std::unique_ptr<LinuxDriverHandle>> driver_handle;

    for (auto drv_it = drivers_.cbegin(); drv_it != drivers_.cend(); drv_it++) {
      driver_handle = std::move((*drv_it)->open(drive_path));
      if (driver_handle.isOk()) {
        break;
      }
    }

    std::unique_ptr<LinuxDriveHandle> drive_handle(new LinuxDriveHandle(drive_path, std::move(driver_handle.value), driver_handle));
    if (driver_handle.isOk()) {
      drive_handle->init();
    }
    return std::move(drive_handle);
  }

  int enumDrives(std::list<DriveInfo> &result_list) const override {
    DIR* block_dir = opendir("/sys/block/");
    struct dirent* entry;
    if (!block_dir) {
      return errno;
    }

    while((entry = readdir(block_dir)) != nullptr) {
      struct stat s = {0};
      std::string devpath = "/dev/";
      devpath.append(entry->d_name);
      if (!strstr(entry->d_name, "loop") && (stat(devpath.c_str(), &s) != -1)) {
        if (S_ISBLK(s.st_mode)) {
          auto handle = open(devpath.c_str());
          result_list.emplace_back(handle->getDriveInfo());
        }
      }
    }
    closedir(block_dir);
    return 0;
  }
};

struct LinuxSingletoneLoader {
  LinuxSingletoneLoader() {
    DriveFactory::getSystemFactory();
  }
};

static LinuxSingletoneLoader _loader();

}; // namespace plat_linux

DriveFactory* DriveFactory::getSystemFactory() {
  static plat_linux::LinuxDriveFactory INSTANCE;
  return &INSTANCE;
}

} // namespace dparm
} // namespace jcu