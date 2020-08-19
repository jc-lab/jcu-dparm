/**
 * @file	tcg_session.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_SESSION_H_
#define JCU_DPARM_TCG_TCG_SESSION_H_

#include <stdint.h>

#include <vector>
#include <string>

#include "../err.h"
#include "tcg_types.h"
#include "tcg_command.h"
#include "tcg_response.h"

namespace jcu {
namespace dparm {
namespace tcg {

class TcgDevice;

class TcgSession {
 public:
  virtual ~TcgSession() {}
  virtual void close() = 0;

  virtual bool isNoHashPassword() const = 0;
  virtual void setNoHashPassword(bool no_hash = true) = 0;
  virtual void dontAutoClose() = 0;

  virtual void setTimeout(uint32_t timeout_ms) = 0;

  virtual DparmReturn<uint8_t> start(const OpalUID& sp, const std::string& host_challenge, const std::vector<uint8_t>& sign_authority) = 0;
  virtual DparmReturn<uint8_t> start(const OpalUID& sp, const std::string& host_challenge, const OpalUID& sign_authority) = 0;
  virtual DparmReturn<uint8_t> authenticate(const std::vector<uint8_t> &sign_authority, const std::string& challenge) = 0;
  virtual DparmReturn<uint8_t> sendCommand(TcgCommand& cmd, TcgResponse& resp) = 0;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_SESSION_H_
