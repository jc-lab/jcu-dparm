/**
 * @file	nvme_win_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <windows.h>
#include <ntddscsi.h>

#include "nvme_win_driver.h"
#include "driver_utils.h"

#include <jcu-dparm/nvme_types.h>

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


// Reference: https://svn.openfabrics.org/svnrepo/nvmewin/releases/IOCTL_Sample_App/nvme.h

/* Section 4.2, Figure 6 */
typedef struct _NVMe_COMMAND_DWORD_0
{
  /* [Opcode] This field indicates the opcode of the command to be executed */
  UCHAR    OPC;

  /*
   * [Fused Operation] In a fused operation, a complex command is created by
   * "fusing” together two simpler commands. Refer to section 6.1. This field
   * indicates whether this command is part of a fused operation and if so,
   * which command it is in the sequence. Value 00b Normal Operation, Value
   * 01b == Fused operation, first command, Value 10b == Fused operation,
   * second command, Value 11b == Reserved.
   */
  UCHAR    FUSE           :2;
  UCHAR    Reserved       :6;

  /*
   * [Command Identifier] This field indicates a unique identifier for the
   * command when combined with the Submission Queue identifier.
   */
  USHORT   CID;
} NVMe_COMMAND_DWORD_0, *PNVMe_COMMAND_DWORD_0;

/*
 * Section 4.2, Figure 7
 */
typedef struct _NVMe_COMMAND
{
  /*
   * [Command Dword 0] This field is common to all commands and is defined
   * in Figure 6.
   */
  NVMe_COMMAND_DWORD_0    CDW0;

  /*
   * [Namespace Identifier] This field indicates the namespace that this
   * command applies to. If the namespace is not used for the command, then
   * this field shall be cleared to 0h. If a command shall be applied to all
   * namespaces on the device, then this value shall be set to FFFFFFFFh.
   */
  ULONG                   NSID;

  /* DWORD 2, 3 */
  ULONGLONG               Reserved;

  /*
   * [Metadata Pointer] This field contains the address of a contiguous
   * physical buffer of metadata. This field is only used if metadata is not
   * interleaved with the LBA data, as specified in the Format NVM command.
   * This field shall be Dword aligned.
   */
  ULONGLONG               MPTR;

  /* [PRP Entry 1] This field contains the first PRP entry for the command. */
  ULONGLONG               PRP1;

  /*
   * [PRP Entry 2] This field contains the second PRP entry for the command.
   * If the data transfer spans more than two memory pages, then this field is
   * a PRP List pointer.
   */
  ULONGLONG               PRP2;

  /* [Command Dword 10] This field is command specific Dword 10. */
  union {
    ULONG               CDW10;
    /*
     * Defined in Admin and NVM Vendor Specific Command format.
     * Number of DWORDs in PRP, data transfer (in Figure 8).
     */
    ULONG               NDP;
  };

  /* [Command Dword 11] This field is command specific Dword 11. */
  union {
    ULONG               CDW11;
    /*
     * Defined in Admin and NVM Vendor Specific Command format.
     * Number of DWORDs in MPTR, Metadata transfer (in Figure 8).
     */
    ULONG               NDM;
  };

  /* [Command Dword 12] This field is command specific Dword 12. */
  ULONG                   CDW12;

  /* [Command Dword 13] This field is command specific Dword 13. */
  ULONG                   CDW13;

  /* [Command Dword 14] This field is command specific Dword 14. */
  ULONG                   CDW14;

  /* [Command Dword 15] This field is command specific Dword 15. */
  ULONG                   CDW15;
} NVMe_COMMAND, *PNVMe_COMMAND;


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
   std::string device_path_;
  HANDLE handle_;

 public:
  std::string getDriverName() const override {
    return "WindowsNvmeWinDriver";
  }

  NvmeWinDriverHandle(const char *device_path, HANDLE handle, NVME_PASS_THROUGH_IOCTL *identify_device_ioctl)
  : device_path_(device_path), handle_(handle)
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
    if (handle_ && (handle_ != INVALID_HANDLE_VALUE)) {
      ::CloseHandle(handle_);
      handle_ = nullptr;
    }
  }

  const std::string &getDevicePath() const override {
    return device_path_;
  }

  bool driverIsNvmeAdminPassthruSupported() const override {
    return true;
  }

  DparmReturn<int> doNvmeAdminPassthru(nvme::nvme_admin_cmd_t *cmd) override {
    NVME_PASS_THROUGH_IOCTL nptwb;
    PNVMe_COMMAND pcommand = (PNVMe_COMMAND)nptwb.NVMeCmd;

    DWORD length = sizeof(nptwb);
    DWORD dwReturned;

    ZeroMemory(&nptwb, sizeof(NVME_PASS_THROUGH_IOCTL));

    nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
    nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
    memcpy((UCHAR*)(&nptwb.SrbIoCtrl.Signature[0]), NVME_SIG_STR, NVME_SIG_STR_LEN);
    nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
    nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);
    nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
    nptwb.ReturnBufferLen = sizeof(nptwb);
    nptwb.Direction = NVME_FROM_DEV_TO_HOST;

    pcommand->CDW0.OPC = cmd->opcode;
    pcommand->NSID = cmd->nsid;
    pcommand->CDW10 = cmd->cdw10;
    pcommand->CDW11 = cmd->cdw11;
    pcommand->CDW12 = cmd->cdw12;
    pcommand->CDW13 = cmd->cdw13;
    pcommand->CDW14 = cmd->cdw14;
    pcommand->CDW15 = cmd->cdw15;

    if(!DeviceIoControl(handle_, IOCTL_SCSI_MINIPORT, &nptwb, length, &nptwb, length, &dwReturned, NULL)) {
      return { DPARME_IOCTL_FAILED, (int)::GetLastError(), 0 };
    }

    memcpy(cmd->addr, nptwb.DataBuffer, cmd->data_len);

    return { DPARME_OK, 0, 0 };
  }

  bool driverIsNvmeIoPassthruSupported() const override {
    return true;
  }

  DparmReturn<int> doNvmeIoPassthru(nvme::nvme_passthru_cmd_t *cmd) override {
    return DriveDriverHandle::doNvmeIoPassthru(cmd);
  }

  bool driverIsNvmeIoSupported() const override {
    return true;
  }

  DparmReturn<int> doNvmeIo(nvme::nvme_user_io_t *io) override {
    return DriveDriverHandle::doNvmeIo(io);
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
    PNVMe_COMMAND pcommand = (PNVMe_COMMAND)nptwb.NVMeCmd;

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

    pcommand->CDW0.OPC = nvme::NVME_ADMIN_OP_IDENTIFY;
    // https://nvmexpress.org/wp-content/uploads/NVM_Express_Revision_1.3.pdf
    // Page 112
    // The Identify Controller data structure is returned to the host for this controller.
    pcommand->CDW10 = 1;

    if(!DeviceIoControl(scsi_handle, IOCTL_SCSI_MINIPORT, &nptwb, length, &nptwb, length, &dwReturned, NULL)) {
      werr = (int)::GetLastError();
      // 1117 ERROR_IO_DEVICE
      break;
    }

    std::unique_ptr<NvmeWinDriverHandle> driver_handle(new NvmeWinDriverHandle(path, scsi_handle, &nptwb));
    return { DPARME_OK, 0, 0, std::move(driver_handle) };
  } while(0);

  if (scsi_handle && (scsi_handle != INVALID_HANDLE_VALUE)) {
    ::CloseHandle(scsi_handle);
  }
  return { DPARME_SYS, werr };
}

DparmReturn<std::unique_ptr<WindowsDriverHandle>> NvmeWinDriver::open(const WindowsPhysicalDrive &drive_info) {
  return this->open(drive_info.physical_disk_path.c_str());
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
