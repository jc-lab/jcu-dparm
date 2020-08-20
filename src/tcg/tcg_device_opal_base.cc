/**
 * @file	tcg_device_opal_base.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <assert.h>

#include "tcg_device_opal_base.h"
#include "../drive_handle_base.h"

#include "tcg_session_impl.h"
#include "tcg_command_impl.h"
#include "tcg_response_impl.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgDeviceOpalBase::TcgDeviceOpalBase(DriveHandleBase *drive_handle)
    : TcgDeviceGeneric(drive_handle)
{
}

bool TcgDeviceOpalBase::isAnySSC() const {
  return true;
}

DparmReturn<OpalStatusCode> TcgDeviceOpalBase::revertTPer(const std::string &password, uint8_t is_psid, uint8_t is_admin_sp) {
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
  if (!dres.isOk() || dres.value != SUCCESS) {
    return dres;
  }

  cmd.reset(OpalUID::ADMINSP_UID, OpalMethod::REVERT);
  cmd.addToken(STARTLIST);
  cmd.addToken(ENDLIST);
  cmd.complete();

  dres = sess.sendCommand(cmd, resp);
  if (dres.isOk()) {
    sess.dontAutoClose();
  }

  return dres;
}

DparmReturn<OpalStatusCode> TcgDeviceOpalBase::opalGetTable(TcgSession& session, TcgResponse& response, const std::vector<uint8_t> &table, uint16_t start_col, uint16_t end_col) {
  TcgCommandImpl cmd;
  cmd.reset(table, OpalMethod::GET);
  cmd.addToken(STARTLIST);
  {
    cmd.addToken(STARTLIST);
    {
      cmd.addToken(STARTNAME);
      cmd.addToken(STARTCOLUMN);
      cmd.addNumberToken(start_col);
      cmd.addToken(ENDNAME);
    }
    {
      cmd.addToken(STARTNAME);
      cmd.addToken(ENDCOLUMN);
      cmd.addNumberToken(end_col);
      cmd.addToken(ENDNAME);
    }
    cmd.addToken(ENDLIST);
  }
  cmd.addToken(ENDLIST);
  cmd.complete();

  return session.sendCommand(cmd, response);
}

DparmReturn<OpalStatusCode> TcgDeviceOpalBase::getDefaultPassword(std::string *out_password) {
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

  dres = opalGetTable(*session, *response, table, CREDENTIAL_PIN, CREDENTIAL_PIN);
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
