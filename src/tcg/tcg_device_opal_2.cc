/**
 * @file	tcg_device_opal_2.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <assert.h>

#include "tcg_device_opal_2.h"
#include "../drive_handle_base.h"

#include "../intl_utils.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgDeviceOpal2::TcgDeviceOpal2(DriveHandleBase *drive_handle)
    : TcgDeviceOpalBase(drive_handle)
{
}

TcgDeviceType TcgDeviceOpal2::getDeviceType() const {
  return kOpalV2Device;
}

uint16_t TcgDeviceOpal2::getBaseComId() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  auto feat_it = drive_info.tcg_raw_features.find(kFcOpalSscV200);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_opal_ssc_feature_v200_t *)feat_it->second.data();
  return SWAP16(feature->base_com_id);
}
uint16_t TcgDeviceOpal2::getNumComIds() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  auto feat_it = drive_info.tcg_raw_features.find(kFcOpalSscV200);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_opal_ssc_feature_v200_t *)feat_it->second.data();
  return SWAP16(feature->num_com_ids);
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
