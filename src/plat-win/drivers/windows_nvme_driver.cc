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

#include "windows_nvme_ioctl.h"

#include "scsi_driver.h"

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

class WindowsNvmeDriverHandle : public WindowsDriverHandle {
 private:
  std::string device_path_;
  HANDLE handle_;

 public:
  std::string getDriverName() const override {
    return "WindowsNvmeDriver";
  }

  WindowsNvmeDriverHandle(const char *path, HANDLE handle, windows10::TStorageQueryWithBuffer *nptwb)
  : device_path_(path), handle_(handle)
  {
    driving_type_ = kDrivingNvme;
    nvme_identify_device_buf_.insert(
        nvme_identify_device_buf_.end(),
        &nptwb->buffer[0],
        &nptwb->buffer[sizeof(nptwb->buffer)]);
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

  DparmResult doSecurityCommand(uint8_t protocol, uint16_t com_id, int rw, void *buffer, uint32_t len, int timeout) override {
    return ScsiDriver::doSecurityCommandImpl(handle_, protocol, com_id, rw, buffer, len, timeout);
  }

  bool driverHasSpecificNvmeGetLogPage() const override {
    return true;
  }

  DparmResult doNvmeGetLogPage(uint32_t nsid, uint8_t log_id, bool rae, uint32_t data_len, void *data) override  {
    windows10::TStorageQueryWithBuffer nptwb;
    DWORD dwReturned = 0;
    int werr = 0;

    ZeroMemory(&nptwb, sizeof(nptwb));

    nptwb.protocol_specific.ProtocolType = ProtocolTypeNvme;
    nptwb.protocol_specific.DataType = NVMeDataTypeLogPage;
    nptwb.protocol_specific.ProtocolDataRequestValue = log_id;
    nptwb.protocol_specific.ProtocolDataRequestSubValue = 0;
    nptwb.protocol_specific.ProtocolDataRequestSubValue2 = 0;
    nptwb.protocol_specific.ProtocolDataOffset = sizeof(nptwb.protocol_specific);
    nptwb.protocol_specific.ProtocolDataLength = (sizeof(nptwb.buffer) > data_len) ? data_len : sizeof(nptwb.buffer);
    nptwb.query.PropertyId = StorageAdapterProtocolSpecificProperty;
    nptwb.query.QueryType = PropertyStandardQuery;

    if (!DeviceIoControl(handle_, IOCTL_STORAGE_QUERY_PROPERTY,
                         &nptwb, sizeof(nptwb), &nptwb, sizeof(nptwb), &dwReturned, NULL)) {
      werr = (int) ::GetLastError();
      return { DPARME_SYS, werr };
    }

    memcpy(data, nptwb.buffer, nptwb.protocol_specific.ProtocolDataLength);

    return { DPARME_OK, 0 };
  }
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
    if (!drive_handle || (drive_handle == INVALID_HANDLE_VALUE)) {
      werr = (int) ::GetLastError();
      break;
    }

    windows10::TStorageQueryWithBuffer nptwb;
    DWORD dwReturned = 0;

    ZeroMemory(&nptwb, sizeof(nptwb));

    nptwb.protocol_specific.ProtocolType = ProtocolTypeNvme;
    nptwb.protocol_specific.DataType = NVMeDataTypeIdentify;
    nptwb.protocol_specific.ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
    nptwb.protocol_specific.ProtocolDataRequestSubValue = 0;
    nptwb.protocol_specific.ProtocolDataOffset = sizeof(nptwb.protocol_specific);
    nptwb.protocol_specific.ProtocolDataLength = sizeof(nptwb.buffer);
    nptwb.query.PropertyId = StorageAdapterProtocolSpecificProperty;
    nptwb.query.QueryType = PropertyStandardQuery;

    if (!DeviceIoControl(drive_handle, IOCTL_STORAGE_QUERY_PROPERTY,
                         &nptwb, sizeof(nptwb), &nptwb, sizeof(nptwb), &dwReturned, NULL)) {
      werr = (int) ::GetLastError();
      break;
    }

    std::unique_ptr<WindowsNvmeDriverHandle> driver_handle(new WindowsNvmeDriverHandle(path, drive_handle, &nptwb));
    return {DPARME_OK, 0, 0, std::move(driver_handle)};
  } while (0);

  if (drive_handle && (drive_handle != INVALID_HANDLE_VALUE)) {
    ::CloseHandle(drive_handle);
  }
  return {DPARME_SYS, werr};
}

DparmReturn<std::unique_ptr<WindowsDriverHandle>> WindowsNvmeDriver::open(const WindowsPhysicalDrive& drive_info) {
  return this->open(drive_info.physical_disk_path.c_str());
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
