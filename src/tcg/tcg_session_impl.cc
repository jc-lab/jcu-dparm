/**
 * @file	tcg_session_impl.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "tcg_session_impl.h"
#include "tcg_command_impl.h"
#include "tcg_response_impl.h"

#include <jcu-random/secure-random-factory.h>

#include <jcu-dparm/tcg/tcg_device.h>

#include "tcg_intl.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgSessionImpl::TcgSessionImpl(TcgDevice* tcg_device)
    : tcg_device_(tcg_device),
      active_(false),
      host_session_num_(0),
      tper_session_num_(0),
      flag_auto_close_(true),
      flag_no_hash_password_(false),
      timeout_(60000)
{
  random_ = jcu::random::getSecureRandomFactory()->create();
}

TcgSessionImpl::~TcgSessionImpl() {
  close();
}

void TcgSessionImpl::close() {
  if (flag_auto_close_ && active_) {
    active_ = false;

    TcgCommandImpl cmd;
    TcgResponseImpl resp;

    cmd.addToken(ENDOFSESSION);
    cmd.complete(false);

    sendCommand(cmd, resp);
  }
}

bool TcgSessionImpl::isNoHashPassword() const {
  return flag_no_hash_password_;
}

void TcgSessionImpl::setNoHashPassword(bool no_hash) {
  flag_no_hash_password_ = no_hash;
}

void TcgSessionImpl::dontAutoClose() {
  flag_auto_close_ = false;
}

void TcgSessionImpl::setTimeout(uint32_t timeout_ms) {
  timeout_ = timeout_ms;
}

DparmReturn<uint8_t> TcgSessionImpl::start(const OpalUID &sp, const std::string &host_challenge, const OpalUID& sign_authority) {
  std::vector<uint8_t> encoded_sign_authority;
  encoded_sign_authority.reserve(1 + sizeof(sign_authority.value));
  encoded_sign_authority.push_back(BYTESTRING8);
  encoded_sign_authority.insert(encoded_sign_authority.end(), sign_authority.value, sign_authority.value + sizeof(sign_authority.value));
  return start(sp, host_challenge, encoded_sign_authority);
}

DparmReturn<uint8_t> TcgSessionImpl::start(const OpalUID &sp, const std::string &host_challenge, const std::vector<uint8_t> &sign_authority) {
  TcgCommandImpl cmd;
  TcgResponseImpl resp;
  uint64_t host_session_id;

  bool is_enterprise = tcg_device_->getDeviceType() == kOpalEnterpriseDevice;

  host_session_id = (uint64_t)random_->nextInt64();

  cmd.reset(OpalUID::SMUID_UID, OpalMethod::STARTSESSION);
  {
    cmd.addToken(OpalToken::STARTLIST);
    cmd.addNumberToken(host_session_id);
    cmd.addToken(sp);
    cmd.addToken(UINT_01);
    if (!host_challenge.empty() && (!is_enterprise)) {
      {
        cmd.addToken(STARTNAME);
        cmd.addToken(UINT_00);
        if (!flag_no_hash_password_) {
          std::vector<uint8_t> hashed;
          tcgHashPassword(tcg_device_, false, hashed, host_challenge.c_str());
          cmd.addStringToken((const char*)hashed.data(), hashed.size());
        } else {
          cmd.addStringToken(host_challenge.c_str(), host_challenge.length());
        }
        cmd.addToken(ENDNAME);
      }
      {
        cmd.addToken(STARTNAME);
        cmd.addToken(UINT_03);
        cmd.addRawToken(sign_authority.data(), sign_authority.size());
        cmd.addToken(ENDNAME);
      }
    }

    if (is_enterprise) {
      const char *text = "SessionTimeout";
      cmd.addToken(STARTNAME);
      cmd.addStringToken(text, strlen(text));
      cmd.addNumberToken(timeout_);
      cmd.addToken(ENDNAME);
    }
    cmd.addToken(ENDLIST);
  }
  cmd.complete();

  DparmReturn<uint8_t> dres = sendCommand(cmd, resp);
  if (!dres.isOk() || dres.value != 0) {
    return dres;
  }

  DparmReturn<uint32_t> temp_u32;

  auto hsn_token = resp.getToken(4);
  auto tsn_token = resp.getToken(5);
  if (!hsn_token || !tsn_token) {
    return { DPARME_ILLEGAL_DATA, 0, 0, 0 };
  }

  temp_u32 = hsn_token->getUint32();
  if (!temp_u32.isOk()) {
    return { temp_u32.code, temp_u32.sys_error, temp_u32.drive_status, 0 };
  }
  host_session_num_ = temp_u32.value;

  temp_u32 = tsn_token->getUint32();
  if (!temp_u32.isOk()) {
    return { temp_u32.code, temp_u32.sys_error, temp_u32.drive_status, 0 };
  }
  tper_session_num_ = temp_u32.value;

  if ((!host_challenge.empty()) && is_enterprise) {
    return authenticate(sign_authority, host_challenge);
  }

  return dres;
}

DparmReturn<uint8_t> TcgSessionImpl::authenticate(const std::vector<uint8_t>& authority, const std::string &challenge) {
  TcgCommandImpl cmd;
  TcgResponseImpl resp;

  bool is_enterprise = tcg_device_->getDeviceType() == kOpalEnterpriseDevice;

  cmd.reset(OpalUID::THISSP_UID, is_enterprise ? OpalMethod::EAUTHENTICATE : OpalMethod::AUTHENTICATE);
  {
    cmd.addToken(OpalToken::STARTLIST);
    cmd.addRawToken(authority.data(), authority.size());
    if (!challenge.empty()) {
      cmd.addToken(STARTNAME);
      if (is_enterprise) {
        const char *text = "Challenge";
        cmd.addStringToken(text, strlen(text));
      } else {
        cmd.addToken(UINT_00);
      }
      if (!flag_no_hash_password_) {
        std::vector<uint8_t> hashed;
        tcgHashPassword(tcg_device_, false, hashed, challenge.c_str());
        cmd.addStringToken((const char *) hashed.data(), hashed.size());
      } else {
        cmd.addStringToken(challenge.c_str(), challenge.length());
      }
      cmd.addToken(ENDNAME);
    }
    cmd.addToken(ENDNAME);
  }

  cmd.complete();

  DparmReturn<uint8_t> dres = sendCommand(cmd, resp);
  if (!dres.isOk() || dres.value != 0) {
    return dres;
  }

  auto temp_token = resp.getToken(1);
  if (!temp_token) {
    return { DPARME_ILLEGAL_DATA, 0, 0, 0 };
  }
  DparmReturn<uint8_t> temp_u8 = temp_token->getUint8();
  if (!temp_u8.isOk()) {
    return { temp_u8.code, temp_u8.sys_error, temp_u8.drive_status, 0 };
  }
  if (temp_u8.value != 0) {
    return { DPARME_TCG_AUTH_FAILED, dres.sys_error, dres.drive_status, temp_u8.value };
  }

  return dres;
}

DparmReturn<uint8_t> TcgSessionImpl::sendCommand(TcgCommand& cmd, TcgResponse& resp) {
  cmd.setHSN(host_session_num_);
  cmd.setTSN(tper_session_num_);
  cmd.setComId(tcg_device_->getBaseComId());
  auto dres = tcg_device_->exec(cmd, resp, 0x01 /* SecurityProtocol */);
  if (!dres.isOk()) {
    return { dres, 0 };
  }

  opal_header_t *resp_header = (opal_header_t*)resp.getRespBuf();
  if ((!resp_header->cp.length) || (!resp_header->pkt.length) || (!resp_header->subpkt.length)) {
    // payload is not received
    return { DPARME_ILLEGAL_DATA, 0, 0, 0 };
  }

  const TcgTokenVO* temp_token = resp.getToken(0);
  if (!temp_token) {
    return { DPARME_ILLEGAL_DATA, 0, 0, 0 };
  }
  if (temp_token->type() == ENDOFSESSION) {
    return { DPARME_OK, 0, 0, 0 };
  }

  auto token_a = resp.getToken(resp.getTokenCount() - 1);
  auto token_b = resp.getToken(resp.getTokenCount() - 5);
  if (!token_a || !token_b) {
    return { DPARME_ILLEGAL_DATA, 0, 0, 0 };
  }

  if ((token_a->type() != ENDLIST) && token_b->type() == STARTLIST) {
    // No method status
    return { DPARME_ILLEGAL_DATA, 0, 0, 0 };
  }

  temp_token = resp.getToken(resp.getTokenCount() - 4);
  auto method_status = temp_token->getUint8();
  if (!method_status.isOk()) {
    return { method_status.code, method_status.sys_error, method_status.drive_status, 0 };
  }
  return { DPARME_OK, dres.sys_error, dres.drive_status, method_status.value };
}


} // namespace tcg
} // namespace dparm
} // namespace jcu
