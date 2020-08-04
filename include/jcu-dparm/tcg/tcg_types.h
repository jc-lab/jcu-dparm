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
  uint8_t MBRDone : 1;
  uint8_t MBREnabled : 1;
  uint8_t media_encryption : 1;
  uint8_t locked : 1;
  uint8_t locking_enabled : 1;
  uint8_t locking_supported : 1;
   */
  uint8_t locking_supported: 1;
  uint8_t locking_enabled: 1;
  uint8_t locked: 1;
  uint8_t media_encryption: 1;
  uint8_t MBREnabled: 1;
  uint8_t MBRDone: 1;
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
  uint16_t base_comm_id;
  uint16_t num_comm_ids;
  /* big endian
  uint8_t reserved01 : 7;
  uint8_t range_crossing : 1;
   */
  uint8_t range_crossing: 1;
  uint8_t reserved01: 7;

  uint16_t numlocking_admin_auth;
  uint16_t numlocking_user_auth;
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

} // extern "C"

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_TYPES_H_
