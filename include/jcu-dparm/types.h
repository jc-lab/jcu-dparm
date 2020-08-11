/**
 * @file	types.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TYPES_H_
#define JCU_DPARM_TYPES_H_

#include <stdint.h>
#include <string.h>

#include <string>
#include <map>
#include <vector>

#include "err.h"
#include "ata_types.h"
#include "nvme_types.h"

namespace jcu {
namespace dparm {

enum DrivingType {
  kDrivingUnknown = 0,
  kDrivingAtapi,
  kDrivingNvme,
};

struct DriveInfo {
  std::string device_path;
  DparmResult open_result;

  DrivingType driving_type;

  std::string model;
  std::string serial;
  std::string firmware_revision;

  ata::ata_identify_device_data_t ata_identify;
  nvme::nvme_identify_controller_t nvme_identify_ctrl;

  unsigned char nvme_major_version;
  unsigned char nvme_minor_version;
  unsigned char nvme_tertiary_version;

  bool is_ssd;
  int ssd_check_weight;

  bool support_sanitize_crypto_erase;
  bool support_sanitize_block_erase;
  bool support_sanitize_overwrite;

  bool tcg_support;

  bool tcg_tper;
  bool tcg_geomerty_reporting;
  bool tcg_locking;
  bool tcg_opal_v100;
  bool tcg_opal_v200;
  bool tcg_enterprise;
  bool tcg_single_user_mode;
  bool tcg_datastore;

  std::map<uint16_t, std::vector<unsigned char>> tcg_raw_features;

  DriveInfo() {
    driving_type = kDrivingUnknown;
    memset(&ata_identify, 0, sizeof(ata_identify));
    nvme_major_version = 0;
    nvme_minor_version = 0;
    nvme_tertiary_version = 0;
    is_ssd = false;
    ssd_check_weight = 0;
    support_sanitize_crypto_erase = false;
    support_sanitize_block_erase = false;
    support_sanitize_overwrite = false;
    tcg_support = false;
    tcg_tper = false;
    tcg_geomerty_reporting = false;
    tcg_locking = false;
    tcg_opal_v100 = false;
    tcg_opal_v200 = false;
    tcg_enterprise = false;
    tcg_single_user_mode = false;
    tcg_datastore = false;
  }
};

/**
 * Priority
 */
enum SanitizeFeature {
  kSanitizeStatus = 0,
  /**
   * ATA only supported
   */
  kSanitizeAntiFreezeLock = (1U << 1U),
  /**
   * ATA only supported
   */
  kSanitizeFreezeLock = (1U << 2U),
  kSanitizeCryptoScramble = (1U << 3U),
  kSanitizeBlockErase = (1U << 4U),
  kSanitizeOverwrite = (1U << 5U),
};

enum SanitizeState {
  kSanitizeIdle = (1U << 0U), // SD0, Duplicatable
  kSanitizeInFrozen = (1U << 1U), // SD1
  kSanitizeOperationInProgress = (1U << 2U), // SD2
  kSanitizeOperationFailed = (1U << 3U), // SD3
  kSanitizeOperationSucceeded = (1U << 4U), // SD4
  kSanitizeAntiFreezed = (1U << 8U), // Duplicatable
};

enum SanitizeErrorReason {
  kSanitizeErrNoReason = 0,
  kSanitizeErrCmdUnsuccessful,
  kSanitizeErrUnsupported,
  kSanitizeErrDeviceInFrozen,
  kSanitizeErrAntiFreezeLock,
  kSanitizeErrNumber
};

struct SanitizeCmdResult {
  uint8_t raw_state; // ata::SanitizeFlag / NVMe SSTAT
  SanitizeState sanitize_states;
  float progress;
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TYPES_H_