/**
 * @file	driver_base.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <sys/ioctl.h>
#include <linux/fs.h>

#include "driver_base.h"

namespace jcu {
namespace dparm {
namespace plat_linux {

void LinuxDriverHandle::mergeDriveInfo(DriveInfo &drive_info) const {
  uint64_t size = 0;
  if (ioctl(getFD(), BLKGETSIZE64, &size) != -1) {
    drive_info.total_capacity = size;
  }
}

} // namespace plat_win
} // namespace dparm
} // namespace jcu
