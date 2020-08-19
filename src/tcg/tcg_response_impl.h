/**
 * @file	tcg_response_impl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_RESPONSE_IMPL_H
#define JCU_DPARM_TCG_RESPONSE_IMPL_H

#include <list>
#include <vector>

#include <jcu-dparm/tcg/tcg_types.h>
#include <jcu-dparm/tcg/tcg_response.h>

namespace jcu {
namespace dparm {
namespace tcg {

class TcgResponseImpl : public TcgResponse {
 public:
  TcgResponseImpl();
  void reset() override;
  uint8_t* getRespBuf() override;
  uint32_t getRespBufSize() const override;

  DparmResult commit() override;

  unsigned int getTokenCount() const override;
  const TcgTokenVO *getToken(unsigned int index) const override;

 private:
  uint8_t resp_buf_[MIN_BUFFER_LENGTH + IO_BUFFER_ALIGNMENT];
  opal_header_t* header_;
  uint8_t *resp_ptr_;

  std::vector<TcgTokenVO> resp_tokens_;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_TCG_RESPONSE_IMPL_H
