/**
 * @file	sg_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>

#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <sys/ioctl.h>

#include "sg_driver.h"
#include "../../intl_utils.h"

#include "../sgio.h"

#include <jcu-dparm/ata_types.h>
#include <jcu-dparm/ata_utils.h>

namespace jcu {
namespace dparm {
namespace plat_linux {
namespace drivers {

class SgDriverHandle : public LinuxDriverHandle {
 private:
  scsi_sg_device dev_;
  std::string path_;

 public:
  std::string getDriverName() const override {
    return "LinuxSgDriver";
  }

  SgDriverHandle(const scsi_sg_device& dev, const std::string& path, ata::ata_identify_device_data_t *identify_device_data)
      : dev_(dev), path_(path) {
    const unsigned char *raw_identify_device_data = (const unsigned char *)identify_device_data;
    driving_type_ = kDrivingAtapi;

    ata_identify_device_buf_.insert(
        ata_identify_device_buf_.end(),
        &raw_identify_device_data[0],
        &raw_identify_device_data[sizeof(ata::ata_identify_device_data_t)]
    );
  }

  int getFD() const override {
    return dev_.fd;
  }

  void close() override {
    if (dev_.fd > 0) {
      ::close(dev_.fd);
      dev_.fd = 0;
    }
  }

  int reopenWritable() override {
    int new_fd = ::open(path_.c_str(), O_RDWR | O_NONBLOCK);
    if (new_fd < 0) {
      return new_fd;
    }
    ::close(dev_.fd);
    dev_.fd = new_fd;
    return 0;
  }

  bool driverIsAtaCmdSupported() const override {
    return true;
  }

  DparmResult doAtaCmd(
      int rw,
      unsigned char* cdb,
      unsigned int cdb_bytes,
      void *data,
      unsigned int data_bytes,
      int pack_id,
      unsigned int timeout_secs,
      unsigned char *sense_buf,
      unsigned int sense_buf_bytes
  ) override {
    int rc = do_sg_ata(&dev_, rw, cdb, cdb_bytes, data, data_bytes, pack_id, timeout_secs, sense_buf, sense_buf_bytes);
    if (rc > 0) {
      return { DPARME_ATA_FAILED, rc };
    }else if (rc < 0 ) {
      return { DPARME_SYS, errno };
    }
    return { DPARME_OK, 0 };
  }

  bool driverIsTaskfileCmdSupported() const override {
    return true;
  }

  DparmResult doTaskfileCmd(
      int rw,
      int dma,
      ata::ata_tf_t *tf,
      void *data,
      unsigned int data_bytes,
      unsigned int timeout_secs
  ) override {
    unsigned char sense_data[32] = { 0 };
    if (dma < 0) {
      dma = ata::is_dma(tf->command);
    }
    int rc = sg16(&dev_, rw, dma, tf, data, data_bytes, timeout_secs, sense_data, sizeof(sense_data));

    if (rc > 0) {
      return { DPARME_ATA_FAILED, rc };
    }else if (rc < 0 ) {
      return { DPARME_SYS, errno };
    }
    return { DPARME_OK, 0 };
  }

  /**
   * Reference: https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
   */
  DparmReturn<InquiryDeviceResult> inquiryDeviceInfo() override {
    InquiryDeviceResult info = {};

    for (int i=0; i < 2; i++) {
      int result;
      struct sg_io_hdr io_hdr;
      unsigned char payload_buffer[192] = {0};
      unsigned char sense[32] = {0};

      // Standard INQUIRY
      unsigned char inq_cmd[] = {
          INQUIRY, 0, 0, 0, sizeof(payload_buffer), 0
      };
      if (i == 1) {
        inq_cmd[1] = 1;
        inq_cmd[2] = 0x80;
      }

      memset(&io_hdr, 0, sizeof(io_hdr));
      io_hdr.interface_id = 'S';
      io_hdr.cmdp = inq_cmd;
      io_hdr.cmd_len = sizeof(inq_cmd);
      io_hdr.dxferp = payload_buffer;
      io_hdr.dxfer_len = sizeof(payload_buffer);
      io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
      io_hdr.sbp = sense;
      io_hdr.mx_sb_len = sizeof(sense);
      io_hdr.timeout = 5000;

      result = ioctl(dev_.fd, SG_IO, &io_hdr);
      if (result < 0) {
        return {DPARME_IOCTL_FAILED, errno};
      }

      if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        return {DPARME_IOCTL_FAILED, 0, (int32_t) io_hdr.info};
      }

      // fixed length... It may not be the full name.

      if (i == 0) {
        info.vendor_identification = intl::trimString(intl::readString(&payload_buffer[8], 8));
        info.product_identification = intl::trimString(intl::readString(&payload_buffer[16], 16));
        info.product_revision_level = intl::trimString(intl::readString(&payload_buffer[32], 4));
      } else {
        info.drive_serial_number = intl::trimString(intl::readString(&payload_buffer[4], payload_buffer[3]));
      }
    }

    return { DPARME_OK, 0, 0, info };
  }
};

DparmReturn<std::unique_ptr<LinuxDriverHandle>> SgDriver::open(const char *path) {
  std::string strpath(path);
  DparmResult result;
  scsi_sg_device dev = {0};
  unsigned char sense_data[32];

  do {
    dev.fd = ::open(path, O_RDONLY | O_NONBLOCK);
    if (dev.fd == -1) {
      result = { DPARME_SYS, errno };
      break;
    }

    dev.debug_puts = options_.debug_puts;

    dev.verbose = 1;
    apt_detect(&dev, 1);

    ata::ata_tf_t tf = {0};
    ata::ata_identify_device_data_t temp = {0};
    tf.command = 0xec;
    if (sg16(&dev, 0, 0, &tf, &temp, sizeof(temp), 3, sense_data, sizeof(sense_data)) == -1) {
      result = { DPARME_SYS, dev.last_errno };
      break;
    }

    std::unique_ptr<SgDriverHandle> driver_handle(new SgDriverHandle(dev, strpath, &temp));
    return {DPARME_OK, 0, 0, std::move(driver_handle)};
  } while (0);

  if (dev.fd > 0) {
    ::close(dev.fd);
  }

  return { result.code, result.sys_error };
}

} // namespace dparm
} // namespace plat_linux
} // namespace dparm
} // namespace jcu
