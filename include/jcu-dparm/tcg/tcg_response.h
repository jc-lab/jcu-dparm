/**
 * @file	tcg_response.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_RESPONSE_H_
#define JCU_DPARM_TCG_TCG_RESPONSE_H_

#include <stdint.h>
#include <jcu-dparm/err.h>

#include "tcg_token.h"

namespace jcu {
namespace dparm {
namespace tcg {

class TcgResponse {
 public:
  virtual ~TcgResponse() {}

  virtual void reset() = 0;

  virtual uint8_t* getRespBuf() = 0;
  virtual uint32_t getRespBufSize() const = 0;
  virtual DparmResult commit() = 0;

  virtual unsigned int getTokenCount() const = 0;
  virtual const TcgTokenVO* getToken(unsigned int index) const = 0;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_RESPONSE_H_
