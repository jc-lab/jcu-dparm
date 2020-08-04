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

#include "scsi_driver.h"
#include "driver_utils.h"

#include <jcu-dparm/ata_types.h>

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

extern "C" {

union _ATA_PASS_THROUGH_EX_UNION {
  struct _ATA_PASS_THROUGH_EX ex;
  struct _ATA_PASS_THROUGH_DIRECT direct;
  unsigned char footer[24];
};

struct _SCSI_PASS_THROUGH_PACK {
  union _SCSI_PASS_THROUGH_UNION {
    struct _SCSI_PASS_THROUGH ex;
    struct _SCSI_PASS_THROUGH_DIRECT direct;
  } a;
  unsigned char sense_buf[24];
};

union _SCSI_CDB_UNION {
  void *ptr;
  struct CDB::_CDB10 *cdb10;
  struct CDB::_CDB16 *cdb16;
  struct CDB::_ATA_PASSTHROUGH12 *ata12;
  struct CDB::_ATA_PASSTHROUGH16 *ata16;
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

  struct _SENSE_DATA sense_data = {0};

  for (int retry_i = 0; retry_i < 2 && !is_success; retry_i++) {
    struct _SCSI_PASS_THROUGH_PACK scsi_params = {0};
    DWORD read_bytes = 0;

    union _SCSI_CDB_UNION cdb = {&scsi_params.a.direct.Cdb};

    if (data && data_bytes && !rw)
      memset(data, 0, data_bytes);

    scsi_params.a.direct.TimeOutValue = timeout_secs;
    scsi_params.a.direct.DataIn = rw ? SCSI_IOCTL_DATA_OUT : SCSI_IOCTL_DATA_IN;

    scsi_params.a.direct.PathId = 0;
    scsi_params.a.direct.TargetId = 0;
    scsi_params.a.direct.Lun = 0;

    scsi_params.a.direct.SenseInfoLength = 24;
    scsi_params.a.direct.SenseInfoOffset = offsetof(
        struct _SCSI_PASS_THROUGH_PACK, sense_buf);

    if (tf->is_lba48) {
      scsi_params.a.direct.CdbLength = sizeof(*cdb.ata16);
      cdb.ata16->OperationCode = SCSIOP_ATA_PASSTHROUGH16;
      cdb.ata16->Features7_0 = tf->lob.feat;
      cdb.ata16->Features15_8 = tf->hob.feat;
      cdb.ata16->SectorCount7_0 = tf->lob.nsect;
      cdb.ata16->SectorCount15_8 = tf->hob.nsect;
      cdb.ata16->LbaLow7_0 = tf->lob.lbal;
      cdb.ata16->LbaLow15_8 = tf->hob.lbal;
      cdb.ata16->LbaMid7_0 = tf->lob.lbam;
      cdb.ata16->LbaMid15_8 = tf->hob.lbam;
      cdb.ata16->LbaHigh7_0 = tf->lob.lbah;
      cdb.ata16->LbaHigh15_8 = tf->hob.lbah;
      cdb.ata16->Device = tf->dev;
      cdb.ata16->Command = tf->command;
      cdb.ata16->Control = 0; // Always zero
    } else {
      scsi_params.a.direct.CdbLength = sizeof(*cdb.ata12);
      cdb.ata12->OperationCode = SCSIOP_ATA_PASSTHROUGH12;
      cdb.ata12->Features = tf->lob.feat;
      cdb.ata12->SectorCount = tf->lob.nsect;
      cdb.ata12->LbaLow = tf->lob.lbal;
      cdb.ata12->LbaMid = tf->lob.lbam;
      cdb.ata12->LbaHigh = tf->lob.lbah;
      cdb.ata12->Device = tf->dev;
      cdb.ata12->Command = tf->command;
      cdb.ata12->Control = 0; // Always zero
    }

    if (retry_i == 0) {
      scsi_params.a.direct.Length = sizeof(scsi_params.a.direct);

      scsi_params.a.direct.DataBuffer = data;
      scsi_params.a.direct.DataTransferLength = data_bytes;

      is_success = ::DeviceIoControl(
          handle,
          IOCTL_SCSI_PASS_THROUGH_DIRECT,
          &scsi_params, sizeof(scsi_params), &scsi_params, sizeof(scsi_params),
          &read_bytes, NULL);
      result = (int) ::GetLastError();
    } else {
      int buf_data_offset = sizeof(scsi_params);
      std::vector<uint8_t> ata_buf(buf_data_offset + data_bytes);

      scsi_params.a.ex.Length = sizeof(scsi_params.a.ex);
      scsi_params.a.ex.DataTransferLength = data_bytes;
      scsi_params.a.ex.DataBufferOffset = data ? buf_data_offset : 0;

      memcpy(ata_buf.data(), &scsi_params, sizeof(scsi_params));

      if (rw && data && data_bytes) {
        memcpy(ata_buf.data() + buf_data_offset, data, data_bytes);
      }

      is_success = ::DeviceIoControl(
          handle,
          IOCTL_SCSI_PASS_THROUGH,
          ata_buf.data(), ata_buf.size(), ata_buf.data(), ata_buf.size(),
          &read_bytes, NULL);
      result = (int) ::GetLastError();

      if (is_success) {
        if (!rw && data && data_bytes) {
          memcpy(data, ata_buf.data() + buf_data_offset, data_bytes);
        }
      }
    }

    if (is_success) {
      memcpy(&sense_data, &scsi_params.sense_buf, sizeof(sense_data));
    }
  }

  if (is_success && data && data_bytes) {
    if (sense_data.Valid && sense_data.SenseKey) {
      return { DPARME_NOT_SUPPORTED, 0 };
    }
  }

  return { is_success ? DPARME_OK : DPARME_SYS, result };
}

class ScsiDriverHandle : public WindowsDriverHandle {
 private:
  HANDLE handle_;

 public:
  ScsiDriverHandle(HANDLE handle, ata::ata_identify_device_data *identify_device_data)
      : handle_(handle) {
    const unsigned char *raw_identify_device_data = (const unsigned char *)identify_device_data;
    driving_type_ = kDrivingAtapi;

    ata_identify_device_buf_.insert(
        ata_identify_device_buf_.end(),
        &raw_identify_device_data[0],
        &raw_identify_device_data[sizeof(ata::ata_identify_device_data)]
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

DparmReturn<std::unique_ptr<WindowsDriverHandle>> ScsiDriver::open(const char *path) {
  std::string strpath(path);
  std::basic_string<TCHAR> drive_path(strpath.cbegin(), strpath.cend());
  HANDLE drive_handle = nullptr;
  DparmResult result;

  do {
    ata::ata_identify_device_data temp = {0};

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

    std::unique_ptr<ScsiDriverHandle> driver_handle(new ScsiDriverHandle(drive_handle, &temp));
    return {DPARME_OK, 0, std::move(driver_handle)};
  } while (0);

  if (drive_handle && (drive_handle != INVALID_HANDLE_VALUE)) {
    ::CloseHandle(drive_handle);
  }

  return { result.code, result.sys_error };
}

} // namespace dparm
} // namespace plat_win
} // namespace dparm
} // namespace jcu