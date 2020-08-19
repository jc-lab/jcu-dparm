/**
 * @file	tcg_command_impl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_COMMAND_IMPL_H
#define JCU_DPARM_TCG_COMMAND_IMPL_H

#include <stdint.h>
#include <jcu-dparm/tcg/tcg_types.h>
#include <jcu-dparm/tcg/tcg_command.h>

namespace jcu {
namespace dparm {
namespace tcg {

class TcgCommandImpl : public TcgCommand {
 public:
  uint8_t cmd_buf_[MAX_BUFFER_LENGTH + IO_BUFFER_ALIGNMENT];
  opal_header_t* header_;
  uint8_t *cmd_ptr_;
  uint32_t cmd_pos_;

  TcgCommandImpl();
  void reset() override;
  void reset(const OpalUID &invoking_uid, const OpalMethod &method) override;
  bool addRawToken(const uint8_t* data, int length) override;
  bool addToken(OpalToken token) override;
  bool addToken(OpalTinyAtom token) override;
  bool addToken(OpalShortAtom token) override;
  bool addToken(OpalLockingState token) override;
  bool addToken(const OpalUID& uid) override;
  bool addToken(const OpalMethod& method) override;
  bool addStringToken(const char *text, int length) override;
  bool addNumberToken(uint64_t value) override;
  bool complete(bool eod = true) override;

  void setComId(uint16_t com_id) override;
  void setTSN(uint32_t tsn) override;
  void setHSN(uint32_t hsn) override;

  const uint8_t* getCmdBuf() const override;
  uint32_t getCmdSize() const override;

 private:
  bool checkCmdBufWrite(int need_size);
  bool addByteToken(uint8_t data);
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_TCG_COMMAND_IMPL_H
