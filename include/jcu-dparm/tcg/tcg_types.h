/**
 * @file	tcg_types.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_TYPES_H_
#define JCU_DPARM_TCG_TCG_TYPES_H_

#include <stdint.h>

namespace jcu {
namespace dparm {
namespace tcg {

enum TcgConstants {
  IO_BUFFER_ALIGNMENT = 1024,
  MAX_BUFFER_LENGTH = 61440,
  MIN_BUFFER_LENGTH = 2048
};

enum FeatureCode {
  kFcTPer = 0x0001,
  kFcLocking = 0x0002,
  kFcGeometryReporting = 0x0003,
  kFcEnterprise =0x0100,
  kFcDataStore = 0x0202,
  kFcSingleUser = 0x0201,
  kFcOpalSscV100 = 0x0200,
  kFcOpalSscV200 = 0x0203
};

extern "C" {

#pragma pack(push, 1)

/**
 * The Discovery 0 Header
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_SSC_v2.01_rev1.00.pdf
 * 3.1.1.1
 */
typedef struct _discovery0_header {
  uint32_t length; /**< the lenght of the header 48 in 2.00.100*/
  uint32_t revision; /**< revision of the header 1 in 2.00.100 */
  uint32_t reserved01;
  uint32_t reserved02;
  // the remainder of the structure is vendor specific and will not be addressed now
  unsigned char reserved03[16];
} discovery0_header_t;

typedef struct _discovery0_basic_feature {
  uint16_t feature_code;
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
} discovery0_basic_feature_t;

/**
 * The Discovery 0 - TPer Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_SSC_v2.01_rev1.00.pdf
 * 3.1.1.2
 */
typedef struct _discovery0_tper_feature {
  uint16_t feature_code; /* 0x0001 in 2.00.100 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  /* big endian
  uint8_t reserved01 : 1;
  uint8_t com_id_management : 1;
  uint8_t reserved02 : 1;
  uint8_t streaming : 1;
  uint8_t buffer_management : 1;
  uint8_t acknack : 1;
  uint8_t async : 1;
  uint8_t sync : 1;
   */
  uint8_t sync: 1;
  uint8_t async: 1;
  uint8_t acknack: 1;
  uint8_t buffer_management: 1;
  uint8_t streaming: 1;
  uint8_t reserved02: 1;
  uint8_t com_id_management: 1;
  uint8_t reserved01: 1;

  uint32_t reserved03;
  uint32_t reserved04;
  uint32_t reserved05;
} discovery0_tper_feature_t;

/**
 * The Discovery 0 - Locking Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_SSC_v2.01_rev1.00.pdf
 * 3.1.1.3
 */
typedef struct _discovery0_locking_feature {
  uint16_t feature_code; /* 0x0002 in 2.00.100 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  /* Big endian
  uint8_t reserved01 : 1;
  uint8_t reserved02 : 1;
  uint8_t mbr_done : 1;
  uint8_t mbr_enabled : 1;
  uint8_t media_encryption : 1;
  uint8_t locked : 1;
  uint8_t locking_enabled : 1;
  uint8_t locking_supported : 1;
   */
  uint8_t locking_supported: 1;
  uint8_t locking_enabled: 1;
  uint8_t locked: 1;
  uint8_t media_encryption: 1;
  uint8_t mbr_enabled: 1;
  uint8_t mbr_done: 1;
  uint8_t reserved01: 1;
  uint8_t reserved02: 1;

  uint32_t reserved03;
  uint32_t reserved04;
  uint32_t reserved05;
} discovery0_locking_feature_t;

/**
 * The Discovery 0 - Geometry Reporting Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_SSC_v2.01_rev1.00.pdf
 * 3.1.1.4
 */
typedef struct _discovery0_geometry_reporting_feature {
  uint16_t feature_code; /* 0x0003 in 2.00.100 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  /* big Endian
  uint8_t reserved01 : 7;
  uint8_t align : 1;
   */
  uint8_t align: 1;
  uint8_t reserved01: 7;
  uint8_t reserved02[7];
  uint32_t logical_block_size;
  uint64_t alignment_granularity;
  uint64_t lowest_aligned_lba;
} discovery0_geometry_reporting_feature_t;

/**
 * The Discovery 0 - Opal SSC Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/Opal_SSC_1.00_rev3.00-Final.pdf
 * 3.1.1.4
 */
typedef struct _discovery0_opal_ssc_feature_v100 {
  uint16_t feature_code; /* 0x0200 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  uint16_t base_com_id;
  uint16_t number_com_ids;
} discovery0_opal_ssc_feature_v100_t;


/**
 * The Discovery 0 -
 * Opal SSC V2.00 Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/Opal_SSC_1.00_rev3.00-Final.pdf
 * 3.1.1.4
 */
typedef struct _discovery0_opal_ssc_feature_v200 {
  uint16_t feature_code; /* 0x0203 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  uint16_t base_com_id;
  uint16_t num_com_ids;
  /* big endian
  uint8_t reserved01 : 7;
  uint8_t range_crossing : 1;
   */
  uint8_t range_crossing: 1;
  uint8_t reserved01: 7;

  uint16_t num_locking_admin_auth;
  uint16_t num_locking_user_auth;
  uint8_t initial_pin;
  uint8_t reverted_pin;
  uint8_t reserved02;
  uint32_t reserved03;
} discovery0_opal_ssc_feature_v200_t;

/**
 * The Discovery 0 - Enterprise SSC Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-SSC_Enterprise-v1.01_r1.00.pdf
 * 3.6.2.7
 */
typedef struct _discovery0_enterprise_ssc_feature {
  uint16_t feature_code; /* 0x0100 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  uint16_t base_com_id;
  uint16_t number_com_ids;
  /* big endian
  uint8_t reserved01 : 7;
  uint8_t range_crossing : 1;
   */
  uint8_t range_crossing: 1;
  uint8_t reserved01: 7;

  uint8_t reserved02;
  uint16_t reserved03;
  uint32_t reserved04;
  uint32_t reserved05;
} discovery0_enterprise_ssc_feature_t;

/**
 * The Discovery 0 - Single User Mode Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_Feature_Set_Single_User_Mode_v1-00_r1-00-Final.pdf
 * 4.2.1
 */
typedef struct _discovery0_single_user_mode_feature {
  uint16_t feature_code; /* 0x0201 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  uint32_t number_locking_objects;
  /* big endian
  uint8_t reserved01 : 5;
  uint8_t policy : 1;
  uint8_t all : 1;
  uint8_t any : 1;
   */
  uint8_t any: 1;
  uint8_t all: 1;
  uint8_t policy: 1;
  uint8_t reserved01: 5;

  uint8_t reserved02;
  uint16_t reserved03;
  uint32_t reserved04;
} discovery0_single_user_mode_feature_t;

/**
 * The Discovery 0 - DataStore Table Feature
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_Feature_Set-Additional_DataStore_Tables_v1_00_r1_00_Final.pdf
 * 4.1.1
 */
typedef struct _discovery0_datastore_table_feature {
  uint16_t feature_code; /* 0x0203 */
  uint8_t reserved_v: 4;
  uint8_t version: 4;
  uint8_t length;
  uint16_t reserved01;
  uint16_t max_tables;
  uint32_t max_size_tables;
  uint32_t table_size_alignment;
} discovery0_datastore_table_feature_t;

union _discovery0_feature_u {
  struct _discovery0_basic_feature basic;
  struct _discovery0_tper_feature tper;
  struct _discovery0_locking_feature locking;
  struct _discovery0_geometry_reporting_feature geometry_reporting;
  struct _discovery0_opal_ssc_feature_v100 opal_ssc_v100;
  struct _discovery0_opal_ssc_feature_v200 opal_ssc_v200;
  struct _discovery0_enterprise_ssc_feature enterprise_ssc;
  struct _discovery0_single_user_mode_feature single_user_mode;
  struct _discovery0_datastore_table_feature datastore_table;
};

/*
 * Reference: https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage_Opal_SSC_Application_Note_1-00_1-00-Final.pdf
 */
typedef struct _opal_com_packet {
  uint32_t reserved0;
  uint8_t  extended_com_id[4];
  uint32_t outstanding_data;
  uint32_t min_transfer;
  uint32_t length;
} opal_com_packet_t;

typedef struct _opal_packet {
  uint32_t tsn;
  uint32_t hsn;
  uint32_t seq_number;
  uint16_t reserved00;
  uint16_t ack_type;
  uint32_t acknowledgement;
  uint32_t length;
} opal_packet_t;

typedef struct _opal_data_sub_packet {
  uint8_t reserved00[6];
  uint16_t kind;
  uint32_t length;
} opal_data_sub_packet_t;

typedef struct _opal_header {
  opal_com_packet_t cp;
  opal_packet_t pkt;
  opal_data_sub_packet_t subpkt;
} opal_header_t;

#pragma pack(pop)

} // extern "C"

struct OpalUID {
  unsigned char value[8];

  /* users */
  static const OpalUID SMUID_UID;
  static const OpalUID THISSP_UID;
  static const OpalUID ADMINSP_UID;
  static const OpalUID LOCKINGSP_UID;
  static const OpalUID ENTERPRISE_LOCKINGSP_UID;
  static const OpalUID ANYBODY_UID;
  static const OpalUID SID_UID;
  static const OpalUID ADMIN1_UID;
  static const OpalUID USER1_UID;
  static const OpalUID USER2_UID;
  static const OpalUID PSID_UID;
  static const OpalUID ENTERPRISE_BANDMASTER0_UID;
  static const OpalUID ENTERPRISE_ERASEMASTER_UID;
  /* tables */
  static const OpalUID LOCKINGRANGE_GLOBAL;
  static const OpalUID LOCKINGRANGE_ACE_RDLOCKED;
  static const OpalUID LOCKINGRANGE_ACE_WRLOCKED;
  static const OpalUID MBRCONTROL;
  static const OpalUID MBR;
  static const OpalUID AUTHORITY_TABLE;
  static const OpalUID C_PIN_TABLE;
  static const OpalUID LOCKING_INFO_TABLE;
  static const OpalUID ENTERPRISE_LOCKING_INFO_TABLE;
  /* C_PIN_TABLE object ID's */
  static const OpalUID C_PIN_MSID;
  static const OpalUID C_PIN_SID;
  static const OpalUID C_PIN_ADMIN1;
  /* half UID's (only first 4 bytes used) */
  static const OpalUID HALF_UID_AUTHORITY_OBJ_REF;
  static const OpalUID HALF_UID_BOOLEAN_ACE;
  /* special value for omitted optional parameter */
  static const OpalUID UID_HEXFF;
};

struct OpalMethod {
  unsigned char value[8];

/**
 * TCG Storage SSC Methods
 * https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage-Opal_SSC_v2.01_rev1.00.pdf
 */
  static const OpalMethod PROPERTIES;
  static const OpalMethod STARTSESSION;
  static const OpalMethod REVERT;
  static const OpalMethod ACTIVATE;
  static const OpalMethod EGET;
  static const OpalMethod ESET;
  static const OpalMethod NEXT;
  static const OpalMethod EAUTHENTICATE;
  static const OpalMethod GETACL;
  static const OpalMethod GENKEY;
  static const OpalMethod REVERTSP;
  static const OpalMethod GET;
  static const OpalMethod SET;
  static const OpalMethod AUTHENTICATE;
  static const OpalMethod RANDOM;
  static const OpalMethod ERASE;
};

/*
 * Reference: https://trustedcomputinggroup.org/wp-content/uploads/TCG_Storage_Opal_SSC_Application_Note_1-00_1-00-Final.pdf
 */
enum OpalToken {
  /* Boolean */
  OPAL_TRUE = 0x01,
  OPAL_FALSE = 0x00,
  OPAL_BOOLEAN_EXPR = 0x03,

  /* cellblocks */
  TABLE = 0x00,
  STARTROW = 0x01,
  ENDROW = 0x02,
  STARTCOLUMN = 0x03,
  ENDCOLUMN = 0x04,
  VALUES = 0x01,

  /* authority table */
  PIN = 0x03,

  /* locking tokens */
  RANGESTART = 0x03,
  RANGELENGTH = 0x04,
  READLOCKENABLED = 0x05,
  WRITELOCKENABLED = 0x06,
  READLOCKED = 0x07,
  WRITELOCKED = 0x08,
  ACTIVEKEY = 0x0A,

  /* locking info table */
  MAXRANGES = 0x04,

  /* mbr control */
  MBRENABLE = 0x01,
  MBRDONE = 0x02,

  /* properties */
  HOSTPROPERTIES = 0x00,

  /* response tokenis() returned values */
  DTA_TOKENID_BYTESTRING = 0xe0,
  DTA_TOKENID_SINT = 0xe1,
  DTA_TOKENID_UINT = 0xe2,
  DTA_TOKENID_TOKEN = 0xe3, // actual token is returned

  STARTLIST = 0xf0,
  ENDLIST = 0xf1,
  STARTNAME = 0xf2,
  ENDNAME = 0xf3,
  CALL = 0xf8,
  ENDOFDATA = 0xf9,
  ENDOFSESSION = 0xfa,
  STARTTRANSACTON = 0xfb,
  ENDTRANSACTON = 0xfc,
  EMPTYATOM = 0xff,
  WHERE = 0x00,
};

enum OpalTinyAtom {
  UINT_00 = 0x00,
  UINT_01 = 0x01,
  UINT_02 = 0x02,
  UINT_03 = 0x03,
  UINT_04 = 0x04,
  UINT_05 = 0x05,
  UINT_06 = 0x06,
  UINT_07 = 0x07,
  UINT_08 = 0x08,
  UINT_09 = 0x09,
  UINT_10 = 0x0a,
  UINT_11 = 0x0b,
  UINT_12 = 0x0c,
  UINT_13 = 0x0d,
  UINT_14 = 0x0e,
  UINT_15 = 0x0f,
};

enum OpalShortAtom {
  UINT_3 = 0x83,
  BYTESTRING4 = 0xa4,
  BYTESTRING8 = 0xa8,
};

enum OpalLockingState {
  READWRITE = 0x01,
  READONLY = 0x02,
  LOCKED = 0x03,
  ARCHIVELOCKED = 0x04,
  ARCHIVEUNLOCKED = 0x05,
};

enum OpalStatusCode {
  SUCCESS = 0x00,
  NOT_AUTHORIZED = 0x01,
  //	OBSOLETE = 0x02,
  SP_BUSY = 0x03,
  SP_FAILED = 0x04,
  SP_DISABLED = 0x05,
  SP_FROZEN = 0x06,
  NO_SESSIONS_AVAILABLE = 0x07,
  UNIQUENESS_CONFLICT = 0x08,
  INSUFFICIENT_SPACE = 0x09,
  INSUFFICIENT_ROWS = 0x0A,
  INVALID_FUNCTION = 0x0B, // defined in early specs, still used in some firmware
  INVALID_PARAMETER = 0x0C,
  INVALID_REFERENCE = 0x0D,
  //	OBSOLETE = 0x0E,
  TPER_MALFUNCTION = 0x0F,
  TRANSACTION_FAILURE = 0x10,
  RESPONSE_OVERFLOW = 0x11,
  AUTHORITY_LOCKED_OUT = 0x12,
  FAIL = 0x3f,
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_TYPES_H_
