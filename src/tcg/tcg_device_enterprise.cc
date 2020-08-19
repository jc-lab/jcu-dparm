/**
 * @file	tcg_device_enterprise.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <assert.h>

#include "tcg_device_enterprise.h"
#include "../drive_handle_base.h"

#include "../intl_utils.h"

#include "tcg_session_impl.h"
#include "tcg_command_impl.h"
#include "tcg_response_impl.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgDeviceEnterprise::TcgDeviceEnterprise(DriveHandleBase *drive_handle)
    : TcgDeviceGeneric(drive_handle)
{
}

TcgDeviceType TcgDeviceEnterprise::getDeviceType() const {
  return kOpalEnterpriseDevice;
}

uint16_t TcgDeviceEnterprise::getBaseComId() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  auto feat_it = drive_info.tcg_raw_features.find(kFcEnterprise);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_enterprise_ssc_feature_t *)feat_it->second.data();
  return SWAP16(feature->base_com_id);
}
uint16_t TcgDeviceEnterprise::getNumComIds() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  auto feat_it = drive_info.tcg_raw_features.find(kFcEnterprise);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_enterprise_ssc_feature_t *)feat_it->second.data();
  return SWAP16(feature->number_com_ids);
}

DparmReturn<OpalStatusCode> TcgDeviceEnterprise::revertTPer(const std::string &password, uint8_t is_psid, uint8_t is_admin_sp) {
  TcgSessionImpl sess(this);
  TcgCommandImpl cmd;
  TcgResponseImpl resp;

  DparmReturn<OpalStatusCode> dres;

  OpalUID uid = OpalUID::SID_UID;
  if (is_psid) {
    sess.setNoHashPassword(true);
    uid = OpalUID::PSID_UID;
  }

  dres = sess.start(OpalUID::ADMINSP_UID, password, uid);
  if (!dres.isOk()) {
    return dres;
  }

  if (is_admin_sp) {
    cmd.reset(OpalUID::ADMINSP_UID, OpalMethod::REVERT);
  } else {
    cmd.reset(OpalUID::THISSP_UID, OpalMethod::REVERTSP);
  }
  cmd.addToken(STARTLIST);
  cmd.addToken(ENDLIST);
  cmd.complete();
  sess.dontAutoClose();

  dres = sess.sendCommand(cmd, resp);

  return dres;
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
