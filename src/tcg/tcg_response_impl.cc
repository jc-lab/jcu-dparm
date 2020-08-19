/**
 * @file	tcg_response_impl.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <vector>

#include "tcg_response_impl.h"

#include "../intl_utils.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgResponseImpl::TcgResponseImpl() {
  resp_ptr_ = resp_buf_ + IO_BUFFER_ALIGNMENT;
  resp_ptr_ = (uint8_t*)((uintptr_t)resp_ptr_ & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
  header_ = (opal_header_t*)resp_ptr_;
}

void TcgResponseImpl::reset() {
  memset(resp_ptr_, 0, MIN_BUFFER_LENGTH);
}

uint8_t* TcgResponseImpl::getRespBuf() {
  return resp_ptr_;
}

uint32_t TcgResponseImpl::getRespBufSize() const {
  return MIN_BUFFER_LENGTH;
}

DparmResult TcgResponseImpl::commit() {
  const std::vector<uint8_t> EMPTYATOM_TOKEN = { EMPTYATOM };
  std::list<TcgTokenVO> resp_tokens;

  uint8_t* cur = resp_ptr_;
  opal_header_t *header = (opal_header_t *)cur;
  uint32_t subpkt_length = SWAP32(header->subpkt.length);
  cur += sizeof(opal_header_t);
  uint8_t* end = cur + subpkt_length;

  uint32_t cur_token_length = 0;
  std::vector<uint8_t> temp_token_buffer;

  if (end > (resp_ptr_ + MIN_BUFFER_LENGTH)) {
    return { DPARME_ILLEGAL_DATA, 0, 0 };
  }

  while (cur < end) {
    if (!(*cur & 0x80U)) {
      // tiny atom
      cur_token_length = 1;
    } else if (!(*cur & 0x40U)) {
      // short atom
      cur_token_length = (*cur & 0x0fU) + 1;
    } else if (!(*cur & 0x20U)) {
      // medium atom
      cur_token_length = ((((uint32_t)cur[0] & 0x07U) << 8U) | (uint32_t)cur[1]) + 2;
    } else if (!(*cur & 0x10U)) {
      // long atom
      cur_token_length = (((uint32_t)cur[1] << 16U) | ((uint32_t)cur[2] << 8U) | (uint32_t)cur[3]) + 4;
    }
    temp_token_buffer.clear();
    temp_token_buffer.insert(temp_token_buffer.end(), cur, cur + cur_token_length);
    cur += cur_token_length;

    if (temp_token_buffer != EMPTYATOM_TOKEN) {
      resp_tokens.emplace_back(TcgTokenVO { std::move(temp_token_buffer) });
    }
  }

  resp_tokens_.clear();
  resp_tokens_.reserve(resp_tokens.size());
  for (auto it = resp_tokens.begin(); it != resp_tokens.end(); it++) {
    resp_tokens_.emplace_back(std::move(*it));
  }

  return { DPARME_OK, 0 };
}
unsigned int TcgResponseImpl::getTokenCount() const {
  return resp_tokens_.size();
}

const TcgTokenVO *TcgResponseImpl::getToken(unsigned int index) const {
  if (index >= resp_tokens_.size()) {
    return nullptr;
  }
  return &resp_tokens_.at(index);
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
