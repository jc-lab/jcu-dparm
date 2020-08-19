/**
 * @file	tcg_session_impl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_SESSION_IMPL_H
#define JCU_DPARM_TCG_SESSION_IMPL_H

#include <memory>

#include <jcu-dparm/tcg/tcg_session.h>
#include <jcu-random/secure-random.h>

namespace jcu {
namespace dparm {
namespace tcg {

class TcgSessionImpl : public TcgSession {
 public:
  TcgSessionImpl(TcgDevice* tcg_device);
  ~TcgSessionImpl();
  void close() override;
  bool isNoHashPassword() const override;
  void setNoHashPassword(bool no_hash) override;
  void dontAutoClose() override;

  void setTimeout(uint32_t timeout_ms) override;

  DparmReturn<OpalStatusCode> start(const OpalUID &sp, const std::string &host_challenge, const std::vector<uint8_t> &sign_authority) override;
  DparmReturn<OpalStatusCode> start(const OpalUID &sp, const std::string &host_challenge, const OpalUID& sign_authority) override;
  DparmReturn<OpalStatusCode> authenticate(const std::vector<uint8_t> &sign_authority, const std::string &challenge) override;
  DparmReturn<OpalStatusCode> sendCommand(TcgCommand& cmd, TcgResponse& resp) override;

 private:
  std::unique_ptr<jcu::random::SecureRandom> random_;

  TcgDevice* tcg_device_;
  bool active_;

  uint64_t host_session_num_;
  uint64_t tper_session_num_;

  bool flag_no_hash_password_;
  bool flag_auto_close_;
  uint32_t timeout_;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_SESSION_IMPL_H
