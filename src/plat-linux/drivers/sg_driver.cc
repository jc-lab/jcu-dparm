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

#include "sg_driver.h"
#include "driver_utils.h"

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

 public:
  SgDriverHandle(const scsi_sg_device& dev, ata::ata_identify_device_data *identify_device_data)
      : dev_(dev) {
    const unsigned char *raw_identify_device_data = (const unsigned char *)identify_device_data;
    driving_type_ = kDrivingAtapi;

    ata_identify_device_buf_.insert(
        ata_identify_device_buf_.end(),
        &raw_identify_device_data[0],
        &raw_identify_device_data[sizeof(ata::ata_identify_device_data)]
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
};

DparmReturn<std::unique_ptr<LinuxDriverHandle>> SgDriver::open(const char *path) {
  std::string strpath(path);
  DparmResult result;
  scsi_sg_device dev = {0};
  int sys_error;
  unsigned char sense_data[32];

  do {
    dev.fd = ::open(path, O_RDWR);
    if (dev.fd == -1) {
      result = { DPARME_SYS, errno };
      break;
    }

    dev.verbose = 1;
    apt_detect(&dev, 1);

    ata::ata_tf_t tf = {0};
    ata::ata_identify_device_data temp = {0};
    tf.command = 0xec;
    sys_error = sg16(&dev, 0, 0, &tf, &temp, sizeof(temp), 3, sense_data, sizeof(sense_data));
    if (sys_error) {
      result = { DPARME_SYS, sys_error };
      break;
    }

    std::unique_ptr<SgDriverHandle> driver_handle(new SgDriverHandle(dev, &temp));
    return {DPARME_OK, 0, std::move(driver_handle)};
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
