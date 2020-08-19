/**
 * @file	tcg_command_impl.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <assert.h>
#include <string.h>

#include "tcg_command_impl.h"
#include "../intl_utils.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgCommandImpl::TcgCommandImpl() {
  cmd_ptr_ = cmd_buf_ + IO_BUFFER_ALIGNMENT;
  cmd_ptr_ = (uint8_t*)((uintptr_t)cmd_ptr_ & (uintptr_t)~(IO_BUFFER_ALIGNMENT - 1));
  header_ = (opal_header_t*)cmd_ptr_;
  reset();
}

bool TcgCommandImpl::checkCmdBufWrite(int need_size) {
  size_t remaining = MAX_BUFFER_LENGTH - cmd_pos_;
  return (remaining >= need_size);
}

void TcgCommandImpl::reset() {
  memset(cmd_ptr_, 0, MAX_BUFFER_LENGTH);
  cmd_pos_ = sizeof(*header_);
}

void TcgCommandImpl::reset(const OpalUID &invoking_uid, const OpalMethod &method) {
  reset();
  cmd_ptr_[cmd_pos_++] = CALL;
  addToken(invoking_uid);
  addToken(method);
}

bool TcgCommandImpl::addByteToken(uint8_t data) {
  if (checkCmdBufWrite(1)) {
    cmd_ptr_[cmd_pos_++] = (uint8_t) data;
    return true;
  }
  return false;
}

bool TcgCommandImpl::addRawToken(const uint8_t *data, int length) {
  if (checkCmdBufWrite(length)) {
    memcpy(&cmd_ptr_[cmd_pos_], data, length);
    cmd_pos_ += length;
    return true;
  }
  return false;
}

bool TcgCommandImpl::addToken(OpalToken token) {
  return addByteToken((uint8_t)token);
}

bool TcgCommandImpl::addToken(OpalTinyAtom token) {
  return addByteToken((uint8_t)token);
}

bool TcgCommandImpl::addToken(OpalShortAtom token) {
  return addByteToken((uint8_t)token);
}

bool TcgCommandImpl::addToken(OpalLockingState token) {
  return addByteToken((uint8_t)token);
}

bool TcgCommandImpl::addToken(const OpalUID& uid) {
  return addStringToken((const char*)uid.value, sizeof(uid.value));
}

bool TcgCommandImpl::addToken(const OpalMethod& method) {
  return addStringToken((const char*)method.value, sizeof(method.value));
}

bool TcgCommandImpl::addStringToken(const char *text, int length) {
  if (length == 0) {
    if (checkCmdBufWrite(2)) {
      // null token
      cmd_ptr_[cmd_pos_++] = 0xa1;
      cmd_ptr_[cmd_pos_++] = 0x00;
    }
  } else if (length < 16) {
    if (checkCmdBufWrite(1 + length)) {
      // tiny atom
      cmd_ptr_[cmd_pos_++] = length | 0xa0;
      memcpy(cmd_ptr_ + cmd_pos_, text, length);
      cmd_pos_ += length;
    }
  } else if (length < 2048) {
    // medium atom
    if (checkCmdBufWrite(2 + length)) {
      // tiny atom
      cmd_ptr_[cmd_pos_++] = (uint8_t) (0xd0U | (length >> 8U) & 0x07U);
      cmd_ptr_[cmd_pos_++] = (uint8_t)length;
      memcpy(cmd_ptr_ + cmd_pos_, text, length);
      cmd_pos_ += length;
    }
  }
  return false;
}

bool TcgCommandImpl::addNumberToken(uint64_t value) {
  if (value < 64) {
    if (!checkCmdBufWrite(1)) {
      return false;
    }
    cmd_ptr_[cmd_pos_++] = (uint8_t) (value & 0x3fU);
  } else {
    int startat;
    if (!checkCmdBufWrite(1)) {
      return false;
    }
    if (value < 0x100) {
      cmd_ptr_[cmd_pos_++] = 0x81;
      startat = 0;
    } else if (value < 0x10000) {
      cmd_ptr_[cmd_pos_++] = 0x82;
      startat = 1;
    } else if (value < 0x100000000) {
      cmd_ptr_[cmd_pos_++] = 0x84;
      startat = 3;
    } else {
      cmd_ptr_[cmd_pos_++] = 0x88;
      startat = 7;
    }
    if (!checkCmdBufWrite(startat + 1)) {
      return false;
    }
    for (int i = startat; i > -1; i--) {
      cmd_ptr_[cmd_pos_++] = (uint8_t) (value >> (i * 8U));
    }
  }
  return true;
}

bool TcgCommandImpl::complete(bool eod) {
  if (eod) {
    if (!checkCmdBufWrite(6)) {
      return false;
    }
    cmd_ptr_[cmd_pos_++] = ENDOFDATA;
    cmd_ptr_[cmd_pos_++] = STARTLIST;
    cmd_ptr_[cmd_pos_++] = 0x00;
    cmd_ptr_[cmd_pos_++] = 0x00;
    cmd_ptr_[cmd_pos_++] = 0x00;
    cmd_ptr_[cmd_pos_++] = ENDLIST;
  }
  header_->subpkt.length = SWAP32(cmd_pos_ - sizeof(opal_header_t));
  while (cmd_pos_ % 4) {
    if (!checkCmdBufWrite(1)) {
      return false;
    }
    cmd_ptr_[cmd_pos_++] = 0x00;
  }
  header_->pkt.length = SWAP32(cmd_pos_ - sizeof(opal_com_packet_t) - sizeof(opal_packet_t));
  header_->cp.length = SWAP32(cmd_pos_ - sizeof(opal_com_packet_t));
  assert(cmd_pos_ <= MAX_BUFFER_LENGTH);
  return true;
}

void TcgCommandImpl::setComId(uint16_t com_id) {
  header_->cp.extended_com_id[0] = (uint8_t)(com_id >> 8);
  header_->cp.extended_com_id[1] = (uint8_t)(com_id);
  header_->cp.extended_com_id[2] = 0x00;
  header_->cp.extended_com_id[3] = 0x00;
}

void TcgCommandImpl::setTSN(uint32_t tsn) {
  header_->pkt.tsn = tsn;
}

void TcgCommandImpl::setHSN(uint32_t hsn) {
  header_->pkt.hsn = hsn;
}

const uint8_t* TcgCommandImpl::getCmdBuf() const {
  return cmd_ptr_;
}

uint32_t TcgCommandImpl::getCmdSize() const {
  unsigned int x = cmd_pos_ % 512;
  if (x) {
    return (512 - x) + cmd_pos_;
  }
  return cmd_pos_;
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
