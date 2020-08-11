/**
 * @file	physical_drive_finder.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/07
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <vector>

#include <tchar.h>
#include <windows.h>

#include <setupapi.h>
#include <winioctl.h>

#pragma comment(lib, "cfgmgr32.lib")
#pragma comment(lib, "setupapi.lib")

#include "physical_drive_finder.h"

namespace jcu {
namespace dparm {
namespace plat_win {

int enumPhysicalDrives(std::list<WindowsPhysicalDrive> &devices) {
  int result_code = 0;

  HDEVINFO dev_info_handle = nullptr;
  GUID disk_class_guid = GUID_DEVINTERFACE_DISK;
  DWORD dev_index;

  std::vector<unsigned char> dev_info_detail_buf;

  do {
    // Get the handle to the device information set for installed
    // disk class devices. Returns only devices that are currently
    // present in the system and have an enabled disk device
    // interface.
    //
    dev_info_handle = ::SetupDiGetClassDevs(
        &disk_class_guid,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (dev_info_handle == INVALID_HANDLE_VALUE) {
      result_code = (int)::GetLastError();
      break;
    }

    PSP_DEVICE_INTERFACE_DETAIL_DATA dev_interface_detail_ptr = NULL;

    for (dev_index = 0;; dev_index++) {
      SP_DEVINFO_DATA devinfodata;
      HANDLE drive_handle = NULL;

      ZeroMemory(&devinfodata, sizeof(devinfodata));
      devinfodata.cbSize = sizeof(devinfodata);

      if (!SetupDiEnumDeviceInfo(dev_info_handle, dev_index, &devinfodata))
        break;

      do {
        DWORD temp_result;
        DWORD dev_interface_detail_size = 0;
        STORAGE_DEVICE_NUMBER stor_dev_num;
        DWORD bytes_returned;

        SP_DEVICE_INTERFACE_DATA dev_interface_data;
        ZeroMemory(&dev_interface_data, sizeof(dev_interface_data));
        dev_interface_data.cbSize = sizeof(dev_interface_data);

        if (!SetupDiEnumInterfaceDevice(dev_info_handle, NULL, &disk_class_guid, dev_index, &dev_interface_data)) {
          break;
        }

        SetupDiGetDeviceInterfaceDetail(dev_info_handle,
                                        &dev_interface_data,
                                        NULL,
                                        0,
                                        &dev_interface_detail_size,
                                        NULL);

        if (dev_info_detail_buf.size() < dev_interface_detail_size) {
          dev_info_detail_buf.resize(dev_interface_detail_size + 128);
        }
        ZeroMemory(dev_info_detail_buf.data(), dev_info_detail_buf.size());

        dev_interface_detail_ptr = (PSP_DEVICE_INTERFACE_DETAIL_DATA) dev_info_detail_buf.data();
        dev_interface_detail_ptr->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(dev_info_handle,
                                             &dev_interface_data,
                                             dev_interface_detail_ptr,
                                             dev_interface_detail_size,
                                             NULL, NULL)) {
          break;
        }

        std::basic_string<TCHAR> device_path(dev_interface_detail_ptr->DevicePath);
        drive_handle = CreateFile(
            dev_interface_detail_ptr->DevicePath,
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_READONLY,
            NULL);
        if (drive_handle == INVALID_HANDLE_VALUE) {
          break;
        }

        if (!DeviceIoControl(drive_handle,
                             IOCTL_STORAGE_GET_DEVICE_NUMBER,
                             NULL,
                             0,
                             &stor_dev_num,
                             sizeof(STORAGE_DEVICE_NUMBER),
                             &bytes_returned,
                             NULL)) {
          break;
        }

        WindowsPhysicalDrive drive_item = {
            (int)stor_dev_num.DeviceNumber, std::string(device_path.cbegin(), device_path.cend())
        };
        devices.emplace_back(drive_item);
      } while (0);

      if (drive_handle && (drive_handle != INVALID_HANDLE_VALUE)) {
        ::CloseHandle(drive_handle);
      }
    }
  } while (0);

  if (dev_info_handle && (dev_info_handle != INVALID_HANDLE_VALUE)) {
    ::SetupDiDestroyDeviceInfoList(dev_info_handle);
  }

  return result_code;
}

} // namespace plat_win
} // namespace dparm
} // namespace jcu
