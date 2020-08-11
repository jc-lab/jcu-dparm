/**
 * @file	scsi_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#define _NTSCSI_USER_MODE_ 1
#include <windows.h>
#include <ntddscsi.h>
#include <scsi.h>

#include "ata_driver.h"
#include "driver_utils.h"

#include <jcu-dparm/ata_types.h>

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

extern "C" {

union _ATA_PASS_THROUGH_UNION {
  struct _ATA_PASS_THROUGH_EX ex;
  struct _ATA_PASS_THROUGH_DIRECT direct;
};

}

static DparmResult doTaskfileCmdImpl(
    HANDLE handle,
    int rw,
    int dma,
    ata::ata_tf_t *tf,
    void *data,
    unsigned int data_bytes,
    unsigned int timeout_secs
) {
  int result = 0;
  bool is_success = false;

  for (int retry_i = 0; retry_i < 2 && !is_success; retry_i++) {
    union _ATA_PASS_THROUGH_UNION ata_params = {0};
    DWORD read_bytes = 0;

    if (data && data_bytes && !rw)
      memset(data, 0, data_bytes);

    ata_params.direct.TimeOutValue = timeout_secs;
    ata_params.direct.AtaFlags = rw ? ATA_FLAGS_DATA_OUT : ATA_FLAGS_DATA_IN;

    ata_params.direct.PathId = 0;
    ata_params.direct.TargetId = 0;
    ata_params.direct.Lun = 0;

    if (tf->is_lba48) {
      ata_params.direct.AtaFlags |= ATA_FLAGS_48BIT_COMMAND;

      ata_params.direct.PreviousTaskFile[0] = tf->hob.feat;
      ata_params.direct.PreviousTaskFile[1] = tf->hob.nsect;
      ata_params.direct.PreviousTaskFile[2] = tf->hob.lbal;
      ata_params.direct.PreviousTaskFile[3] = tf->hob.lbam;
      ata_params.direct.PreviousTaskFile[4] = tf->hob.lbah;
      ata_params.direct.PreviousTaskFile[5] = 0;
      ata_params.direct.PreviousTaskFile[6] = 0;
      ata_params.direct.PreviousTaskFile[7] = 0;
    }

    ata_params.direct.CurrentTaskFile[0] = tf->lob.feat;
    ata_params.direct.CurrentTaskFile[1] = tf->lob.nsect;
    ata_params.direct.CurrentTaskFile[2] = tf->lob.lbal;
    ata_params.direct.CurrentTaskFile[3] = tf->lob.lbam;
    ata_params.direct.CurrentTaskFile[4] = tf->lob.lbah;
    ata_params.direct.CurrentTaskFile[5] = tf->dev;
    ata_params.direct.CurrentTaskFile[6] = tf->command;
    ata_params.direct.CurrentTaskFile[7] = 0; // Always zero

    if (retry_i == 0) {
      ata_params.direct.Length = sizeof(ata_params.direct);

      ata_params.direct.DataBuffer = data;
      ata_params.direct.DataTransferLength = data_bytes;

      is_success = ::DeviceIoControl(
          handle,
          IOCTL_ATA_PASS_THROUGH_DIRECT,
          &ata_params, sizeof(ata_params), &ata_params, sizeof(ata_params),
          &read_bytes, NULL);
      result = (int) ::GetLastError();
    } else {
      int buf_data_offset = sizeof(ata_params);
      std::vector<uint8_t> ata_buf(buf_data_offset + data_bytes);

      ata_params.ex.Length = sizeof(ata_params.ex);
      ata_params.ex.DataTransferLength = data_bytes;
      ata_params.ex.DataBufferOffset = data ? buf_data_offset : 0;

      memcpy(ata_buf.data(), &ata_params, sizeof(ata_params));

      if (rw && data && data_bytes) {
        memcpy(ata_buf.data() + buf_data_offset, data, data_bytes);
      }

      is_success = ::DeviceIoControl(
          handle,
          IOCTL_ATA_PASS_THROUGH,
          ata_buf.data(), ata_buf.size(), ata_buf.data(), ata_buf.size(),
          &read_bytes, NULL);
      result = (int) ::GetLastError();

      if (is_success) {
        if (!rw && data && data_bytes) {
          memcpy(data, ata_buf.data() + buf_data_offset, data_bytes);
        }
      }
    }

    // Check ATA status
    if (ata_params.direct.CurrentTaskFile[6]/*Status*/ & (0x01/*Err*/|0x08/*DRQ*/)) {
      return { DPARME_ATA_FAILED, 0 }; // OR Not supported
    }
  }

  return { is_success ? DPARME_OK : DPARME_SYS, result };
}

class AtaDrvierHandle : public WindowsDriverHandle {
 private:
  std::string device_path_;
  HANDLE handle_;

 public:
  AtaDrvierHandle(const char *device_path, HANDLE handle, ata::ata_identify_device_data_t *identify_device_data)
      : device_path_(device_path), handle_(handle)
  {
    const unsigned char *raw_identify_device_data = (const unsigned char *)identify_device_data;
    driving_type_ = kDrivingAtapi;

    ata_identify_device_buf_.insert(
        ata_identify_device_buf_.end(),
        &raw_identify_device_data[0],
        &raw_identify_device_data[sizeof(ata::ata_identify_device_data_t)]
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

  DparmResult doTaskfileCmd(
      int rw,
      int dma,
      ata::ata_tf_t *tf,
      void *data,
      unsigned int data_bytes,
      unsigned int timeout_secs
  ) override {
    return doTaskfileCmdImpl(handle_, rw, dma, tf, data, data_bytes, timeout_secs);
  }
};

DparmReturn<std::unique_ptr<WindowsDriverHandle>> AtaDriver::open(const char *path) {
  std::string strpath(path);
  std::basic_string<TCHAR> drive_path(strpath.cbegin(), strpath.cend());
  HANDLE drive_handle = nullptr;
  DparmResult result;

  do {
    ata::ata_identify_device_data_t temp = {0};

    drive_handle = CreateFile(
        drive_path.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
    );
    if (!drive_handle || (drive_handle == INVALID_HANDLE_VALUE)) {
      result = {DPARME_SYS, (int)::GetLastError()};
      break;
    }

    ata::ata_tf_t tf = {0};
    tf.command = 0xec;
    result = doTaskfileCmdImpl(drive_handle, 0, 0, &tf, &temp, sizeof(temp), 3);
    if (!result.isOk()) {
      break;
    }

    std::unique_ptr<AtaDrvierHandle> driver_handle(new AtaDrvierHandle(path, drive_handle, &temp));
    return {DPARME_OK, 0, std::move(driver_handle)};
  } while (0);

  if (drive_handle && (drive_handle != INVALID_HANDLE_VALUE)) {
    ::CloseHandle(drive_handle);
  }

  return { result.code, result.sys_error };
}

DparmReturn<std::unique_ptr<WindowsDriverHandle>> AtaDriver::open(const WindowsPhysicalDrive &drive_info) {
  return this->open(drive_info.physical_disk_path.c_str());
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu
