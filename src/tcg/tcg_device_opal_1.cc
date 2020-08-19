/**
 * @file	tcg_device_opal_1.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <assert.h>

#include "tcg_device_opal_1.h"
#include "../drive_handle_base.h"

#include "../intl_utils.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgDeviceOpal1::TcgDeviceOpal1(DriveHandleBase *drive_handle)
    : TcgDeviceOpalBase(drive_handle)
{
}

TcgDeviceType TcgDeviceOpal1::getDeviceType() const {
  return kOpalV1Device;
}

uint16_t TcgDeviceOpal1::getBaseComId() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  auto feat_it = drive_info.tcg_raw_features.find(kFcOpalSscV100);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_opal_ssc_feature_v100_t *)feat_it->second.data();
  return SWAP16(feature->base_com_id);
}

uint16_t TcgDeviceOpal1::getNumComIds() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  auto feat_it = drive_info.tcg_raw_features.find(kFcOpalSscV100);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_opal_ssc_feature_v100_t *)feat_it->second.data();
  return SWAP16(feature->number_com_ids);
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
