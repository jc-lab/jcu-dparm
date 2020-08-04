/**
 * @file	windows_nvme_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "windows_nvme_driver.h"
#include "driver_utils.h"

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

class WindowsNvmeDriverHandle : public WindowsDriverHandle {
 private:
  HANDLE handle_;

 public:
  HANDLE getHandle() const override {
    return handle_;
  }

  void close() override {

  }
};

struct _WINNVME_STORAGE_QUERY_WITH_BUFFER {
  STORAGE_PROPERTY_QUERY query;
  STORAGE_PROTOCOL_SPECIFIC_DATA protocol_specific;
  unsigned char buffer[4096];
};

DparmReturn<std::unique_ptr<WindowsDriverHandle>> WindowsNvmeDriver::open(const char *path) {
  std::string strpath(path);
  std::basic_string<TCHAR> drive_path(strpath.cbegin(), strpath.cend());
  int werr = 0;
  HANDLE drive_handle;

  do {
    drive_handle = CreateFile(
        drive_path.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
    );
    struct _WINNVME_STORAGE_QUERY_WITH_BUFFER nptwb;
    DWORD dwReturned = 0;

    ZeroMemory(&nptwb, sizeof(nptwb));

    nptwb.protocol_specific.ProtocolType = ProtocolTypeNvme;
    nptwb.protocol_specific.DataType = NVMeDataTypeIdentify;
    nptwb.protocol_specific.ProtocolDataOffset = sizeof(nptwb.protocol_specific);
    nptwb.protocol_specific.ProtocolDataLength = sizeof(nptwb.buffer);
    nptwb.query.PropertyId = StorageAdapterProtocolSpecificProperty;
    nptwb.query.QueryType = PropertyStandardQuery;

    if (!DeviceIoControl(drive_handle, IOCTL_STORAGE_QUERY_PROPERTY,
                         &nptwb, sizeof(nptwb), &nptwb, sizeof(nptwb), &dwReturned, NULL)) {
      werr = (int) ::GetLastError();
      break;
    }

  } while (0);

  if (drive_handle && (drive_handle != INVALID_HANDLE_VALUE)) {
    ::CloseHandle(drive_handle);
  }
  return {DPARME_SYS, werr};
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
