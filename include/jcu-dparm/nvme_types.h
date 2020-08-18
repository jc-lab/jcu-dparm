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

/**
 * NVM_Express_Revision_1.3.pdf
 * Figure 31 : Status Code - Generic Command Status values
 */
enum NvmeStatusCode {
  NVME_SC_SUCCESS			= 0x0,
  NVME_SC_INVALID_OPCODE		= 0x1,
  NVME_SC_INVALID_FIELD		= 0x2,
  NVME_SC_CMDID_CONFLICT		= 0x3,
  NVME_SC_DATA_XFER_ERROR		= 0x4,
  NVME_SC_POWER_LOSS		= 0x5,
  NVME_SC_INTERNAL		= 0x6,
  NVME_SC_ABORT_REQ		= 0x7,
  NVME_SC_ABORT_QUEUE		= 0x8,
  NVME_SC_FUSED_FAIL		= 0x9,
  NVME_SC_FUSED_MISSING		= 0xa,
  NVME_SC_INVALID_NS		= 0xb,
  NVME_SC_CMD_SEQ_ERROR		= 0xc,
  NVME_SC_SGL_INVALID_LAST	= 0xd,
  NVME_SC_SGL_INVALID_COUNT	= 0xe,
  NVME_SC_SGL_INVALID_DATA	= 0xf,
  NVME_SC_SGL_INVALID_METADATA	= 0x10,
  NVME_SC_SGL_INVALID_TYPE	= 0x11,
  NVME_SC_CMB_INVALID_USE		= 0x12,
  NVME_SC_PRP_INVALID_OFFSET	= 0x13,
  NVME_SC_ATOMIC_WRITE_UNIT_EXCEEDED= 0x14,
  NVME_SC_OPERATION_DENIED	= 0x15,
  NVME_SC_SGL_INVALID_OFFSET	= 0x16,

  NVME_SC_INCONSISTENT_HOST_ID= 0x18,
  NVME_SC_KEEP_ALIVE_EXPIRED	= 0x19,
  NVME_SC_KEEP_ALIVE_INVALID	= 0x1A,
  NVME_SC_PREEMPT_ABORT		= 0x1B,
  NVME_SC_SANITIZE_FAILED		= 0x1C,
  NVME_SC_SANITIZE_IN_PROGRESS	= 0x1D,

  NVME_SC_NS_WRITE_PROTECTED	= 0x20,
  NVME_SC_CMD_INTERRUPTED		= 0x21,
  NVME_SC_TRANSIENT_TRANSPORT	= 0x22,

  NVME_SC_LBA_RANGE		= 0x80,
  NVME_SC_CAP_EXCEEDED		= 0x81,
  NVME_SC_NS_NOT_READY		= 0x82,
  NVME_SC_RESERVATION_CONFLICT	= 0x83,
  NVME_SC_FORMAT_IN_PROGRESS	= 0x84,

  /*
   * Command Specific Status:
   */
  NVME_SC_CQ_INVALID		= 0x100,
  NVME_SC_QID_INVALID		= 0x101,
  NVME_SC_QUEUE_SIZE		= 0x102,
  NVME_SC_ABORT_LIMIT		= 0x103,
  NVME_SC_ABORT_MISSING		= 0x104,
  NVME_SC_ASYNC_LIMIT		= 0x105,
  NVME_SC_FIRMWARE_SLOT		= 0x106,
  NVME_SC_FIRMWARE_IMAGE		= 0x107,
  NVME_SC_INVALID_VECTOR		= 0x108,
  NVME_SC_INVALID_LOG_PAGE	= 0x109,
  NVME_SC_INVALID_FORMAT		= 0x10a,
  NVME_SC_FW_NEEDS_CONV_RESET	= 0x10b,
  NVME_SC_INVALID_QUEUE		= 0x10c,
  NVME_SC_FEATURE_NOT_SAVEABLE	= 0x10d,
  NVME_SC_FEATURE_NOT_CHANGEABLE	= 0x10e,
  NVME_SC_FEATURE_NOT_PER_NS	= 0x10f,
  NVME_SC_FW_NEEDS_SUBSYS_RESET	= 0x110,
  NVME_SC_FW_NEEDS_RESET		= 0x111,
  NVME_SC_FW_NEEDS_MAX_TIME	= 0x112,
  NVME_SC_FW_ACTIVATE_PROHIBITED	= 0x113,
  NVME_SC_OVERLAPPING_RANGE	= 0x114,
  NVME_SC_NS_INSUFFICIENT_CAP	= 0x115,
  NVME_SC_NS_ID_UNAVAILABLE	= 0x116,
  NVME_SC_NS_ALREADY_ATTACHED	= 0x118,
  NVME_SC_NS_IS_PRIVATE		= 0x119,
  NVME_SC_NS_NOT_ATTACHED		= 0x11a,
  NVME_SC_THIN_PROV_NOT_SUPP	= 0x11b,
  NVME_SC_CTRL_LIST_INVALID	= 0x11c,
  NVME_SC_DEVICE_SELF_TEST_IN_PROGRESS= 0x11d,
  NVME_SC_BP_WRITE_PROHIBITED	= 0x11e,
  NVME_SC_INVALID_CTRL_ID		= 0x11f,
  NVME_SC_INVALID_SECONDARY_CTRL_STATE= 0x120,
  NVME_SC_INVALID_NUM_CTRL_RESOURCE	= 0x121,
  NVME_SC_INVALID_RESOURCE_ID	= 0x122,
  NVME_SC_PMR_SAN_PROHIBITED	= 0x123,
  NVME_SC_ANA_INVALID_GROUP_ID= 0x124,
  NVME_SC_ANA_ATTACH_FAIL		= 0x125,

  /*
   * Command Set Specific - Namespace Types commands:
   */
  NVME_SC_IOCS_NOT_SUPPORTED		= 0x129,
  NVME_SC_IOCS_NOT_ENABLED		= 0x12A,
  NVME_SC_IOCS_COMBINATION_REJECTED	= 0x12B,
  NVME_SC_INVALID_IOCS			= 0x12C,

  /*
   * I/O Command Set Specific - NVM commands:
   */
  NVME_SC_BAD_ATTRIBUTES		= 0x180,
  NVME_SC_INVALID_PI		= 0x181,
  NVME_SC_READ_ONLY		= 0x182,
  NVME_SC_CMD_SIZE_LIMIT_EXCEEDED = 0x183,

  /*
   * I/O Command Set Specific - Fabrics commands:
   */
  NVME_SC_CONNECT_FORMAT		= 0x180,
  NVME_SC_CONNECT_CTRL_BUSY	= 0x181,
  NVME_SC_CONNECT_INVALID_PARAM	= 0x182,
  NVME_SC_CONNECT_RESTART_DISC	= 0x183,
  NVME_SC_CONNECT_INVALID_HOST	= 0x184,

  NVME_SC_DISCOVERY_RESTART	= 0x190,
  NVME_SC_AUTH_REQUIRED		= 0x191,

  /*
   * I/O Command Set Specific - Zoned Namespace commands:
   */
  NVME_SC_ZONE_BOUNDARY_ERROR		= 0x1B8,
  NVME_SC_ZONE_IS_FULL			= 0x1B9,
  NVME_SC_ZONE_IS_READ_ONLY		= 0x1BA,
  NVME_SC_ZONE_IS_OFFLINE			= 0x1BB,
  NVME_SC_ZONE_INVALID_WRITE		= 0x1BC,
  NVME_SC_TOO_MANY_ACTIVE_ZONES		= 0x1BD,
  NVME_SC_TOO_MANY_OPEN_ZONES		= 0x1BE,
  NVME_SC_ZONE_INVALID_STATE_TRANSITION	= 0x1BF,

  /*
   * Media and Data Integrity Errors:
   */
  NVME_SC_WRITE_FAULT		= 0x280,
  NVME_SC_READ_ERROR		= 0x281,
  NVME_SC_GUARD_CHECK		= 0x282,
  NVME_SC_APPTAG_CHECK		= 0x283,
  NVME_SC_REFTAG_CHECK		= 0x284,
  NVME_SC_COMPARE_FAILED		= 0x285,
  NVME_SC_ACCESS_DENIED		= 0x286,
  NVME_SC_UNWRITTEN_BLOCK		= 0x287,

  /*
   * Path-related Errors:
   */
  NVME_SC_ANA_PERSISTENT_LOSS	= 0x301,
  NVME_SC_ANA_INACCESSIBLE	= 0x302,
  NVME_SC_ANA_TRANSITION		= 0x303,

  NVME_SC_CRD			= 0x1800,
  NVME_SC_DNR			= 0x4000,
};

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

/**
 * NVM_Express_Revision_1.3.pdf
 * Figure 86 : Get Log Page - Command Dword 10
 */
enum NvmeGetLogPageCommand {
  NVME_NO_LOG_LSP       = 0x0,
  NVME_NO_LOG_LPO       = 0x0,
  NVME_LOG_ANA_LSP_RGO  = 0x1,
  NVME_TELEM_LSP_CREATE = 0x1,
};

/**
 * NVM_Express_Revision_1.3.pdf
 * Figure 178 : Sanitize - Command Dword 10
 */
enum NvmeSanitizeCommand {
  NVME_SANITIZE_NO_DEALLOC	= 0x00000200,
  NVME_SANITIZE_OIPBP		= 0x00000100,
  NVME_SANITIZE_OWPASS_SHIFT	= 0x00000004, /* 07:04 */
  NVME_SANITIZE_AUSE		= 0x00000008,
  NVME_SANITIZE_ACT_CRYPTO_ERASE	= 0x00000004,
  NVME_SANITIZE_ACT_OVERWRITE	= 0x00000003,
  NVME_SANITIZE_ACT_BLOCK_ERASE	= 0x00000002,
  NVME_SANITIZE_ACT_EXIT		= 0x00000001,

  /* Sanitize Monitor/Log */
  NVME_SANITIZE_LOG_DATA_LEN		= 0x0014,
  NVME_SANITIZE_LOG_GLOBAL_DATA_ERASED	= 0x0100,
  NVME_SANITIZE_LOG_NUM_CMPLTED_PASS_MASK	= 0x00F8,
  NVME_SANITIZE_LOG_STATUS_MASK		= 0x0007,
  NVME_SANITIZE_LOG_NEVER_SANITIZED	= 0x0000,
  NVME_SANITIZE_LOG_COMPLETED_SUCCESS	= 0x0001,
  NVME_SANITIZE_LOG_IN_PROGESS		= 0x0002,
  NVME_SANITIZE_LOG_COMPLETED_FAILED	= 0x0003,
  NVME_SANITIZE_LOG_ND_COMPLETED_SUCCESS	= 0x0004,
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
  union {
    le32_t ver;
    struct {
      uint8_t tertiary_version;
      uint8_t minor_version;
      uint16_t major_version;
    };
  };
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

/**
 * NVM_Express_Revision_1.3.pdf
 * 5.14.1.9.2 Sanitize Status (Log Identifier 81h)
 */
typedef struct nvme_sanitize_log_page {
  le16_t progress;
  le16_t status;
  le32_t cdw10_info;
  le32_t est_overwrite_time;
  le32_t est_block_erase_time;
  le32_t est_crypto_erase_time;
  // extended
  le32_t est_overwrite_time_with_no_deallocate;
  le32_t est_block_erase_time_with_no_deallocate;
  le32_t est_crypto_erase_time_with_no_deallocate;
} nvme_sanitize_log_page_t;

#pragma pack(pop)

} // extern "C"

} // namespace nvme
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_NVME_TYPES_H_
