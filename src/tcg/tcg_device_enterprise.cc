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

DparmReturn<OpalStatusCode> TcgDeviceEnterprise::enterpriseGetTable(TcgSession& session, TcgResponse& response, const std::vector<uint8_t> &table, const char *start_col, const char *end_col) {
  TcgCommandImpl cmd;
  cmd.reset(table, OpalMethod::GET);
  cmd.addToken(STARTLIST);
  {
    cmd.addToken(STARTLIST);
    {
      cmd.addToken(STARTNAME);
      cmd.addStringToken("startColumn");
      cmd.addStringToken(start_col);
      cmd.addToken(ENDNAME);
    }
    {
      cmd.addToken(STARTNAME);
      cmd.addStringToken("endColumn");
      cmd.addStringToken(end_col);
      cmd.addToken(ENDNAME);
    }
    cmd.addToken(ENDLIST);
  }
  cmd.addToken(ENDLIST);
  cmd.complete();

  return session.sendCommand(cmd, response);
}

DparmReturn<OpalStatusCode> TcgDeviceEnterprise::getDefaultPassword(std::string *out_password) {
  auto session = createSession();
  auto response = createResponse();
  auto dres = session->start(OpalUID::ADMINSP_UID, "", OpalUID::UID_HEXFF);
  if (!dres.isOk()) {
    return dres;
  }
  std::vector<uint8_t> table;

  table.reserve(9);

  table.push_back(BYTESTRING8);
  auto msid = OpalUID::C_PIN_MSID;
  table.insert(table.end(), msid.value, msid.value + sizeof(msid.value));

  dres = enterpriseGetTable(*session, *response, table, "PIN", "PIN");
  if (!dres.isOk()) {
    return dres;
  }

  auto password_token = response->getToken(4);
  if (!password_token) {
    return { DPARME_ILLEGAL_RESPONSE, 0 };
  }

  auto password_string = password_token->getString();
  if (!password_string.isOk()) {
    return { password_string, (OpalStatusCode)0 };
  }

  *out_password = password_string.value;

  return { DPARME_OK, 0, 0, (OpalStatusCode)0 };
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
