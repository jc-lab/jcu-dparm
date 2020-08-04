/**
 * @file	nvme_win_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "nvme_win_driver.h"
#include "driver_utils.h"

#include <windows.h>
#include <ntddscsi.h>

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

#define NVME_STORPORT_DRIVER 0xE000
#define NVME_PASS_THROUGH_SRB_IO_CODE \
    CTL_CODE( NVME_STORPORT_DRIVER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define NVME_SIG_STR "NvmeMini"
#define NVME_SIG_STR_LEN 8
#define NVME_FROM_DEV_TO_HOST 2
#define NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE 6
#define NVME_IOCTL_CMD_DW_SIZE 16
#define NVME_IOCTL_COMPLETE_DW_SIZE 4
#define NVME_PT_TIMEOUT 40

struct NVME_PASS_THROUGH_IOCTL {
  SRB_IO_CONTROL SrbIoCtrl;
  DWORD          VendorSpecific[NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE];
  DWORD          NVMeCmd[NVME_IOCTL_CMD_DW_SIZE];
  DWORD          CplEntry[NVME_IOCTL_COMPLETE_DW_SIZE];
  DWORD          Direction;
  DWORD          QueueId;
  DWORD          DataBufferLen;
  DWORD          MetaDataLen;
  DWORD          ReturnBufferLen;
  UCHAR          DataBuffer[4096];
};

class NvmeWinDriverHandle : public WindowsDriverHandle {
 private:
  HANDLE handle_;

 public:
  NvmeWinDriverHandle(HANDLE handle, NVME_PASS_THROUGH_IOCTL* identify_device_ioctl)
  : handle_(handle)
  {
    driving_type_ = kDrivingNvme;
    nvme_identify_device_buf_.insert(
        nvme_identify_device_buf_.end(),
        &identify_device_ioctl->DataBuffer[0],
        &identify_device_ioctl->DataBuffer[sizeof(identify_device_ioctl->DataBuffer)]
        );
  }

  HANDLE getHandle() const override {
    return handle_;
  }

  void close() override {

  }
};

DparmReturn<std::unique_ptr<WindowsDriverHandle>> NvmeWinDriver::open(const char *path) {
  std::string strpath(path);
  std::basic_string<TCHAR> drive_path(strpath.cbegin(), strpath.cend());
  std::basic_string<TCHAR> scsi_path;
  HANDLE scsi_handle = nullptr;
  int werr = getScsiPath(&scsi_path, drive_path);
  if (werr) {
    return { DPARME_SYS, werr };
  }

  do {
    NVME_PASS_THROUGH_IOCTL nptwb;
    DWORD length = sizeof(nptwb);
    DWORD dwReturned;

    scsi_handle = ::CreateFile(
        scsi_path.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (!scsi_handle || (scsi_handle == INVALID_HANDLE_VALUE)) {
      werr = (int)::GetLastError();
      break;
    }

    ZeroMemory(&nptwb, sizeof(NVME_PASS_THROUGH_IOCTL));

    nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
    nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
    memcpy((UCHAR*)(&nptwb.SrbIoCtrl.Signature[0]), NVME_SIG_STR, NVME_SIG_STR_LEN);
    nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
    nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);
    nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
    nptwb.ReturnBufferLen = sizeof(nptwb);
    nptwb.Direction = NVME_FROM_DEV_TO_HOST;

    nptwb.NVMeCmd[0] = 6;  // Identify

    // https://nvmexpress.org/wp-content/uploads/NVM_Express_Revision_1.3.pdf
    // Page 112
    // The Identify Controller data structure is returned to the host for this controller.
    nptwb.NVMeCmd[10] = 1;

    if(!DeviceIoControl(scsi_handle, IOCTL_SCSI_MINIPORT, &nptwb, length, &nptwb, length, &dwReturned, NULL)) {
      werr = (int)::GetLastError();
      break;
    }

    std::unique_ptr<NvmeWinDriverHandle> driver_handle(new NvmeWinDriverHandle(scsi_handle, &nptwb));
    return { DPARME_OK, 0, std::move(driver_handle) };
  } while(0);

  if (scsi_handle && (scsi_handle != INVALID_HANDLE_VALUE)) {
    ::CloseHandle(scsi_handle);
  }
  return { DPARME_SYS, werr };
}


} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
