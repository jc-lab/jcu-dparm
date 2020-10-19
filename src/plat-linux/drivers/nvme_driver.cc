/**
 * @file	nvme_driver.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/10
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <unistd.h>
#include <sys/fcntl.h>

#include "nvme_driver.h"
#include "nvme_ioctl.h"

namespace jcu {
namespace dparm {
namespace plat_linux {
namespace drivers {

class NvmeDriverHandle : public LinuxDriverHandle {
 private:
  int fd_;
  int ns_id_;

 public:
  NvmeDriverHandle(int fd, int nsid)
      : fd_(fd), ns_id_(nsid) {
    driving_type_ = kDrivingNvme;
  }

  int getFD() const override {
    return fd_;
  }

  void close() override {
    if (fd_ > 0) {
      ::close(fd_);
      fd_ = 0;
    }
  }

  DparmReturn<int> readIdentify() {
    std::vector<unsigned char> identify_buf(4096);
    nvme::nvme_admin_cmd_t identify_cmd = { 0 };
    identify_cmd.opcode = nvme::NVME_ADMIN_OP_IDENTIFY;
    identify_cmd.nsid = 0;
    identify_cmd.addr = (uint64_t)identify_buf.data();
    identify_cmd.data_len = 4096;
    identify_cmd.cdw10 = 1;
    identify_cmd.cdw11 = 0;
    DparmReturn<int> result = doNvmeAdminPassthru(&identify_cmd);
    if (result.isOk()) {
      nvme_identify_device_buf_.swap(identify_buf);
    }
    return result;
  }

  bool driverIsNvmeAdminPassthruSupported() const override {
    return true;
  }

  DparmReturn<int> doNvmeAdminPassthru(nvme::nvme_admin_cmd_t *cmd) override {
    nvme_ioctl_admin_cmd_t data = {0};
    data.opcode = cmd->opcode;
    data.flags = cmd->flags;
    data.rsvd1 = cmd->rsvd1;
    data.nsid = cmd->nsid;
    data.cdw2 = cmd->cdw2;
    data.cdw3 = cmd->cdw3;
    data.metadata = cmd->metadata;
    data.addr = cmd->addr;
    data.metadata_len = cmd->metadata_len;
    data.data_len = cmd->data_len;
    data.cdw10 = cmd->cdw10;
    data.cdw11 = cmd->cdw11;
    data.cdw12 = cmd->cdw12;
    data.cdw13 = cmd->cdw13;
    data.cdw14 = cmd->cdw14;
    data.cdw15 = cmd->cdw15;
    data.timeout_ms = cmd->timeout_ms;
    data.result = cmd->result;
    int rc = ioctl(fd_, NVME_IOCTL_ADMIN_CMD, &data);
    if (rc == -1) {
      return { DPARME_IOCTL_FAILED, errno };
    }
    if (rc) {
      return { DPARME_NVME_FAILED, 0, rc, {} };
    }
    return { DPARME_OK, 0, rc, rc };
  }

  bool driverIsNvmeIoPassthruSupported() const override {
    return true;
  }

  DparmReturn<int> doNvmeIoPassthru(nvme::nvme_passthru_cmd_t *cmd) override {
    nvme_ioctl_passthru_cmd_t data = {0};
    data.opcode = cmd->opcode;
    data.flags = cmd->flags;
    data.rsvd1 = cmd->rsvd1;
    data.nsid = cmd->nsid;
    data.cdw2 = cmd->cdw2;
    data.cdw3 = cmd->cdw3;
    data.metadata = cmd->metadata;
    data.addr = cmd->addr;
    data.metadata_len = cmd->metadata_len;
    data.data_len = cmd->data_len;
    data.cdw10 = cmd->cdw10;
    data.cdw11 = cmd->cdw11;
    data.cdw12 = cmd->cdw12;
    data.cdw13 = cmd->cdw13;
    data.cdw14 = cmd->cdw14;
    data.cdw15 = cmd->cdw15;
    data.timeout_ms = cmd->timeout_ms;
    data.result = cmd->result;
    int rc = ioctl(fd_, NVME_IOCTL_IO_CMD, &data);
    if (rc == -1) {
      return { DPARME_IOCTL_FAILED, errno };
    }
    if (rc) {
      return { DPARME_NVME_FAILED, 0, rc, {} };
    }
    return { DPARME_OK, 0, rc, rc };
  }

  bool driverIsNvmeIoSupported() const override {
    return true;
  }

  DparmReturn<int> doNvmeIo(nvme::nvme_user_io_t *io) override {
    nvme_ioctl_user_io_t data = {0};
    data.opcode = io->opcode;
    data.flags = io->flags;
    data.control = io->control;
    data.nblocks = io->nblocks;
    data.rsvd = io->rsvd;
    data.metadata = io->metadata;
    data.addr = io->addr;
    data.slba = io->slba;
    data.dsmgmt = io->dsmgmt;
    data.reftag = io->reftag;
    data.apptag = io->apptag;
    data.appmask = io->appmask;
    int rc = ioctl(fd_, NVME_IOCTL_SUBMIT_IO, &data);
    if (rc == -1) {
      return { DPARME_IOCTL_FAILED, errno };
    }
    if (rc) {
      return { DPARME_NVME_FAILED, 0, rc, {} };
    }
    return { DPARME_OK, 0, rc, rc };
  }
};

DparmReturn<std::unique_ptr<LinuxDriverHandle>> NvmeDriver::open(const char *path) {
  std::string strpath(path);
  DparmResult result;
  int fd;

  do {
    int nsid;

    fd = ::open(path, O_RDWR);
    if (fd == -1) {
      result = { DPARME_SYS, errno };
      break;
    }

    if ((nsid = ioctl(fd, NVME_IOCTL_ID)) == -1) {
      result = { DPARME_SYS, errno };
      break;
    }

    std::unique_ptr<NvmeDriverHandle> driver_handle(new NvmeDriverHandle(fd, nsid));
    auto identify_result = driver_handle->readIdentify();
    result = identify_result;
    return {result.code, result.sys_error, result.drive_status, std::move(driver_handle)};
  } while (0);

  if (fd > 0) {
    ::close(fd);
  }

  return { result.code, result.sys_error };
}

} // namespace dparm
} // namespace plat_linux
} // namespace dparm
} // namespace jcu
