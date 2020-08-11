/**
 * @file	nvme_types.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/10
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_NVME_TYPES_H_
#define JCU_DPARM_NVME_TYPES_H_

#include <stdint.h>

namespace jcu {
namespace dparm {
namespace nvme {

extern "C" {

typedef uint8_t u8_t;
typedef uint16_t le16_t;
typedef uint32_t le32_t;

enum NvmeAdminOpCode {
  NVME_ADMIN_OP_DELETE_SQ		= 0x00,
  NVME_ADMIN_OP_CREATE_SQ		= 0x01,
  NVME_ADMIN_OP_GET_LOG_PAGE		= 0x02,
  NVME_ADMIN_OP_DELETE_CQ		= 0x04,
  NVME_ADMIN_OP_CREATE_CQ		= 0x05,
  NVME_ADMIN_OP_IDENTIFY		= 0x06,
  NVME_ADMIN_OP_ABORT_CMD		= 0x08,
  NVME_ADMIN_OP_SET_FEATURES		= 0x09,
  NVME_ADMIN_OP_GET_FEATURES		= 0x0A,
  NVME_ADMIN_OP_ASYNC_EVENT		= 0x0C,
  NVME_ADMIN_OP_NS_MGMT		= 0x0D,
  NVME_ADMIN_OP_ACTIVATE_FW		= 0x10,
  NVME_ADMIN_OP_DOWNLOAD_FW		= 0x11,
  NVME_ADMIN_OP_DEV_SELF_TEST	= 0x14,
  NVME_ADMIN_OP_NS_ATTACH		= 0x15,
  NVME_ADMIN_OP_KEEP_ALIVE		= 0x18,
  NVME_ADMIN_OP_DIRECTIVE_SEND	= 0x19,
  NVME_ADMIN_OP_DIRECTIVE_RECV	= 0x1A,
  NVME_ADMIN_OP_VIRTUAL_MGMT		= 0x1C,
  NVME_ADMIN_OP_NVME_MI_SEND		= 0x1D,
  NVME_ADMIN_OP_NVME_MI_RECV		= 0x1E,
  NVME_ADMIN_OP_DBBUF		= 0x7C,
  NVME_ADMIN_OP_FORMAT_NVM		= 0x80,
  NVME_ADMIN_OP_SECURITY_SEND	= 0x81,
  NVME_ADMIN_OP_SECURITY_RECV	= 0x82,
  NVME_ADMIN_OP_SANITIZE_NVM		= 0x84,
  NVME_ADMIN_OP_GET_LBA_STATUS	= 0x86,
};

#pragma pack(push, 1)
typedef struct nvme_user_io {
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
} nvme_user_io_t;

typedef struct nvme_passthru_cmd {
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
} nvme_passthru_cmd_t;

typedef struct nvme_passthru_cmd nvme_admin_cmd_t;

typedef struct nvme_passthru_cmd64 {
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
} nvme_passthru_cmd64_t;

typedef struct nvme_identify_power_state {
  le16_t			max_power;	/* centiwatts */
  u8_t			rsvd2;
  u8_t			flags;
  le32_t			entry_lat;	/* microseconds */
  le32_t			exit_lat;	/* microseconds */
  u8_t			read_tput;
  u8_t			read_lat;
  u8_t			write_tput;
  u8_t			write_lat;
  le16_t			idle_power;
  u8_t			idle_scale;
  u8_t			rsvd19;
  le16_t			active_power;
  u8_t			active_work_scale;
  u8_t			rsvd23[9];
} nvme_identify_power_state_t;

typedef struct nvme_identify_controller {
  le16_t			vid;
  le16_t			ssvid;
  char			sn[20];
  char			mn[40];
  char			fr[8];
  u8_t			rab;
  u8_t			ieee[3];
  u8_t			cmic;
  u8_t			mdts;
  le16_t			cntlid;
  le32_t			ver;
  le32_t			rtd3r;
  le32_t			rtd3e;
  le32_t			oaes;
  le32_t			ctratt;
  le16_t			rrls;
  u8_t			rsvd102[9];
  u8_t			cntrltype;
  char			fguid[16];
  le16_t			crdt1;
  le16_t			crdt2;
  le16_t			crdt3;
  u8_t			rsvd134[122];
  le16_t			oacs;
  u8_t			acl;
  u8_t			aerl;
  u8_t			frmw;
  u8_t			lpa;
  u8_t			elpe;
  u8_t			npss;
  u8_t			avscc;
  u8_t			apsta;
  le16_t			wctemp;
  le16_t			cctemp;
  le16_t			mtfa;
  le32_t			hmpre;
  le32_t			hmmin;
  u8_t			tnvmcap[16];
  u8_t			unvmcap[16];
  le32_t			rpmbs;
  le16_t			edstt;
  u8_t			dsto;
  u8_t			fwug;
  le16_t			kas;
  le16_t			hctma;
  le16_t			mntmt;
  le16_t			mxtmt;
  le32_t			sanicap;
  le32_t			hmminds;
  le16_t			hmmaxd;
  le16_t			nsetidmax;
  le16_t			endgidmax;
  u8_t			anatt;
  u8_t			anacap;
  le32_t			anagrpmax;
  le32_t			nanagrpid;
  le32_t			pels;
  u8_t			rsvd356[156];
  u8_t			sqes;
  u8_t			cqes;
  le16_t			maxcmd;
  le32_t			nn;
  le16_t			oncs;
  le16_t			fuses;
  u8_t			fna;
  u8_t			vwc;
  le16_t			awun;
  le16_t			awupf;
  u8_t			icsvscc;
  u8_t			nwpc;
  le16_t			acwu;
  le16_t			ocfs;
  le32_t			sgls;
  le32_t			mnan;
  u8_t			rsvd544[224];
  char			subnqn[256];
  u8_t			rsvd1024[768];
  le32_t			ioccsz;
  le32_t			iorcsz;
  le16_t			icdoff;
  u8_t			ctrattr;
  u8_t			msdbd;
  u8_t			rsvd1804[244];
  nvme_identify_power_state_t	psd[32];
  u8_t			vs[1024];
} nvme_identify_controller_t;

#pragma pack(pop)

} // extern "C"

} // namespace nvme
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_NVME_TYPES_H_
