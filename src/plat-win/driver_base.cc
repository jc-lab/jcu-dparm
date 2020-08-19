/**
 * @file	driver_base.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "driver_base.h"

namespace jcu {
namespace dparm {
namespace plat_win {

void WindowsDriverHandle::mergeDriveInfo(DriveInfo &drive_info) const {
  STORAGE_DEVICE_NUMBER stor_dev_num = {0};
  DWORD bytes_returned = 0;
  if (DeviceIoControl(
          getHandle(),
          IOCTL_STORAGE_GET_DEVICE_NUMBER,
          NULL,
          0,
          &stor_dev_num,
          sizeof(STORAGE_DEVICE_NUMBER),
          &bytes_returned,
          NULL)) {
    drive_info.windows_dev_num = stor_dev_num.DeviceNumber;
  }
}

} // namespace plat_win
} // namespace dparm
} // namespace jcu
