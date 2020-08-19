/**
 * @file	tcg_command.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_COMMAND_H_
#define JCU_DPARM_TCG_TCG_COMMAND_H_

#include "tcg_types.h"

namespace jcu {
namespace dparm {
namespace tcg {

class TcgCommand {
 public:
  virtual void reset() = 0;
  virtual void reset(const OpalUID& uid, const OpalMethod& method) = 0;

  virtual bool addRawToken(const uint8_t* data, int length) = 0;
  virtual bool addToken(OpalToken token) = 0;
  virtual bool addToken(OpalTinyAtom token) = 0;
  virtual bool addToken(OpalShortAtom token) = 0;
  virtual bool addToken(OpalLockingState token) = 0;
  virtual bool addToken(const OpalUID& uid) = 0;
  virtual bool addToken(const OpalMethod& method) = 0;
  virtual bool addStringToken(const char* text, int length) = 0;
  virtual bool addNumberToken(uint64_t value) = 0;
  virtual bool complete(bool eod = true) = 0;

  virtual void setComId(uint16_t com_id) = 0;
  virtual void setTSN(uint32_t tsn) = 0;
  virtual void setHSN(uint32_t hsn) = 0;

  virtual const uint8_t* getCmdBuf() const = 0;
  virtual uint32_t getCmdSize() const = 0;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_TCG_TCG_COMMAND_H_
