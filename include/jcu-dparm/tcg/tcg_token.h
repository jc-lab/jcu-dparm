/**
 * @file	tcg_token.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/19
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_TOKEN_H_
#define JCU_DPARM_TCG_TCG_TOKEN_H_

#include <vector>
#include <string>
#include <stdint.h>
#include "tcg_types.h"

namespace jcu {
namespace dparm {
namespace tcg {

class TcgTokenVO {
 private:
  std::vector<uint8_t> buf_;

 public:
  TcgTokenVO(const std::vector<uint8_t>& data) // Copy
      : buf_(data)
  {}
  TcgTokenVO(std::vector<uint8_t>&& data) // Move
      : buf_(std::move(data))
  {}
  const std::vector<uint8_t>& buffer() const {
    return buf_;
  }

  OpalToken type() const {
    uint8_t type_value = buf_.front();
    if (!(type_value & 0x80U)) {
      // tiny atom
      if ((type_value & 0x40U))
        return DTA_TOKENID_SINT;
      else
        return DTA_TOKENID_UINT;
    } else if (!(type_value & 0x40U)) {
      // short atom
      if ((type_value & 0x20U))
        return DTA_TOKENID_BYTESTRING;
      else if ((type_value & 0x10U))
        return DTA_TOKENID_SINT;
      else
        return DTA_TOKENID_UINT;
    } else if (!(type_value & 0x20U)) {
      // medium atom
      if ((type_value & 0x10U))
        return DTA_TOKENID_BYTESTRING;
      else if ((type_value & 0x08U))
        return DTA_TOKENID_SINT;
      else
        return DTA_TOKENID_UINT;
    } else if (!(type_value & 0x10U)) {
      // long atom
      if ((type_value & 0x02U))
        return DTA_TOKENID_BYTESTRING;
      else if ((type_value & 0x01U))
        return DTA_TOKENID_SINT;
      else
        return DTA_TOKENID_UINT;
    } else {
      return (OpalToken) type_value;
    }
  }

  size_t length() const {
    return buf_.size();
  }

  DparmReturn<uint64_t> getUint64() const {
    uint8_t type_value = buf_.front();
    if (!(type_value & 0x80U)) { //tiny atom
      if ((type_value & 0x40U)) {
        // signed atom
        return { DPARME_ILLEGAL_DATA, 0 };
      } else {
        return { DPARME_OK, 0, 0, (uint64_t) (type_value & 0x3fU) };
      }
    } else if (!(type_value & 0x40U)) {
      // short atom
      if ((type_value & 0x10U)) {
        // signed atom
        return { DPARME_ILLEGAL_DATA, 0 };
      } else {
        uint64_t whatever = 0;
        if (buf_.size() > 9) {
          // Maybe error?
        }
        int b = 0;
        for (uint32_t i = (uint32_t) buf_.size() - 1; i > 0; i--) {
          whatever |= ((uint64_t)buf_[i] << (8U * b));
          b++;
        }
        return { DPARME_OK, 0, 0, whatever };
      }
    } else if (!(type_value & 0x20U)) {
      // medium atom
      return { DPARME_ILLEGAL_DATA, 0 };
    } else if (!(type_value & 0x10U)) {
      // long atom
      return { DPARME_ILLEGAL_DATA, 0 };
    } else {
      // non number token
      return { DPARME_ILLEGAL_DATA, 0 };
    }
  }

  DparmReturn<uint32_t> getUint32() const {
    auto dres = getUint64();
    if (!dres.isOk())
      return { dres, (uint32_t) dres.value };
    if (dres.value > 0xffffffffULL) {
      return { DPARME_OK, 0, 1, (uint32_t) dres.value };
    }
    return { DPARME_OK, 0, 0, (uint32_t) dres.value };
  }

  DparmReturn<uint16_t> getUint16() const {
    auto dres = getUint64();
    if (!dres.isOk())
      return { dres, (uint16_t) dres.value };
    if (dres.value > 0xffffULL) {
      return { DPARME_OK, 0, 1, (uint16_t) dres.value };
    }
    return { DPARME_OK, 0, 0, (uint16_t) dres.value };
  }

  DparmReturn<uint8_t> getUint8() const {
    auto dres = getUint64();
    if (!dres.isOk())
      return { dres, (uint8_t) dres.value };
    if (dres.value > 0xffULL) {
      return { DPARME_OK, 0, 1, (uint8_t) dres.value };
    }
    return { DPARME_OK, 0, 0, (uint8_t) dres.value };
  }

  DparmReturn<std::string> getString() const {
    uint8_t type_value = buf_.front();
    int offset = 0;
    if (!(type_value & 0x80U)) {
      // tiny atom
      return { DPARME_ILLEGAL_DATA, 0 };
    } else if (!(type_value & 0x40U)) {
      // short atom
      offset = 1;
    } else if (!(type_value & 0x20U)) {
      // medium atom
      offset = 2;
    } else if (!(type_value & 0x10U)) {
      // long atom
      offset = 4;
    } else {
      // non string token
      return { DPARME_OK, 0, 2 };
    }
    std::string s(buf_.data() + offset, buf_.data() + buf_.size());
    return { DPARME_OK, 0, 0, s };
  }

  DparmReturn<std::vector<uint8_t>> getBytes() const {
    uint8_t type_value = buf_.front();
    int offset = 0;
    if (!(type_value & 0x80U)) {
      // tiny atom
      return { DPARME_ILLEGAL_DATA, 0 };
    } else if (!(type_value & 0x40U)) {
      // short atom
      offset = 1;
    } else if (!(type_value & 0x20U)) {
      // medium atom
      offset = 2;
    } else if (!(type_value & 0x10U)) {
      // long atom
      offset = 4;
    } else {
      // non bytestring atom
      return { DPARME_OK, 0, 2 };
    }
    std::vector<uint8_t> s(buf_.data() + offset, buf_.data() + buf_.size());
    return { DPARME_OK, 0, 0, s };
  }
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_TOKEN_H_
