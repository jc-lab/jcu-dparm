/**
 * @file	nvme_ioctl_ioctl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/10
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_LINUX_DRIVERS_NVME_IOCTL_H_
#define JCU_DPARM_SRC_PLAT_LINUX_DRIVERS_NVME_IOCTL_H_

#include <linux/types.h>
#include <sys/ioctl.h>

#include <jcu-dparm/nvme_types.h>

namespace jcu {
namespace dparm {
namespace plat_linux {
namespace drivers {


#pragma pack(push, 1)
typedef struct nvme_ioctl_user_io {
  uint8_t opcode;
  uint8_t flags;
  uint16_t control;
  uint16_t nblocks;
  uint16_t rsvd;
  uint64_t metadata;
  uint64_t addr;
  uint64_t slba;
  uint32_t dsmgmt;
  uint32_t reftag;
  uint16_t apptag;
  uint16_t appmask;
} nvme_ioctl_user_io_t;

typedef struct nvme_ioctl_passthru_cmd {
  uint8_t opcode;
  uint8_t flags;
  uint16_t rsvd1;
  uint32_t nsid;
  uint32_t cdw2;
  uint32_t cdw3;
  uint64_t metadata;
  uint64_t addr;
  uint32_t metadata_len;
  uint32_t data_len;
  uint32_t cdw10;
  uint32_t cdw11;
  uint32_t cdw12;
  uint32_t cdw13;
  uint32_t cdw14;
  uint32_t cdw15;
  uint32_t timeout_ms;
  uint32_t result;
} nvme_ioctl_passthru_cmd_t;

typedef struct nvme_ioctl_passthru_cmd nvme_ioctl_admin_cmd_t;

typedef struct nvme_ioctl_passthru_cmd64 {
  uint8_t opcode;
  uint8_t flags;
  uint16_t rsvd1;
  uint32_t nsid;
  uint32_t cdw2;
  uint32_t cdw3;
  uint64_t metadata;
  uint64_t addr;
  uint32_t metadata_len;
  uint32_t data_len;
  uint32_t cdw10;
  uint32_t cdw11;
  uint32_t cdw12;
  uint32_t cdw13;
  uint32_t cdw14;
  uint32_t cdw15;
  uint32_t timeout_ms;
  uint32_t rsvd2;
  uint64_t result;
} nvme_ioctl_passthru_cmd64_t;
#pragma pack(pop)

enum NvmeIoctl {
  NVME_IOCTL_ID = _IO('N', 0x40),
  NVME_IOCTL_ADMIN_CMD = _IOWR('N', 0x41, nvme_ioctl_admin_cmd_t),
  NVME_IOCTL_SUBMIT_IO = _IOW('N', 0x42, nvme_ioctl_user_io_t),
  NVME_IOCTL_IO_CMD = _IOWR('N', 0x43, nvme_ioctl_passthru_cmd),
  NVME_IOCTL_RESET = _IO('N', 0x44),
  NVME_IOCTL_SUBSYS_RESET = _IO('N', 0x45),
  NVME_IOCTL_RESCAN = _IO('N', 0x46),
  NVME_IOCTL_ADMIN64_CMD = _IOWR('N', 0x47, nvme_ioctl_passthru_cmd64_t),
  NVME_IOCTL_IO64_CMD = _IOWR('N', 0x48, nvme_ioctl_passthru_cmd64_t),
};

} // namespace drivers
} // namespace plat_linux
} // namespace dparm
} // namespace jcu

#endif /* JCU_DPARM_SRC_PLAT_LINUX_DRIVERS_NVME_IOCTL_H_ */
