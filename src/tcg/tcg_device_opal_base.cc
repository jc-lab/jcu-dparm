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

DparmReturn<uint8_t> TcgDeviceOpalBase::revertTPer(const std::string &password, uint8_t is_psid, uint8_t is_admin_sp) {
  TcgSessionImpl sess(this);
  TcgCommandImpl cmd;
  TcgResponseImpl resp;

  DparmReturn<uint8_t> dres;

  OpalUID uid = OpalUID::SID_UID;
  if (is_psid) {
    sess.setNoHashPassword(true);
    uid = OpalUID::PSID_UID;
  }

  dres = sess.start(OpalUID::ADMINSP_UID, password, uid);
  if (!dres.isOk()) {
    return dres;
  }

  cmd.reset(OpalUID::ADMINSP_UID, OpalMethod::REVERT);
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
