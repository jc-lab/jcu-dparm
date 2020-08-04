/**
 * @file	err.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_ERR_H_
#define JCU_DPARM_ERR_H_

#include <stdint.h>
#include <utility>

namespace jcu {
namespace dparm {

enum DparmCode {
  DPARME_OK = 0,
  DPARME_SYS = 1,
  DPARME_NOT_SUPPORTED = 2,
  DPARME_ATA_FAILED = 3,
  DPARME_ILLEGAL_DATA = 11,
};

struct DparmResult {
  DparmCode code;
  int sys_error;

  DparmResult() : code(DPARME_OK), sys_error(0) {}
  DparmResult(DparmCode code, int sys_error) : code(code), sys_error(sys_error) {}

  bool isOk() const {
    return code == DPARME_OK;
  }
};

template<typename T>
struct DparmReturn : DparmResult {
  T value;

  DparmReturn() : DparmResult(DPARME_OK, 0), value() {}
  DparmReturn(DparmCode _code, int _sys_error) : DparmResult(_code, _sys_error), value() {}
  DparmReturn(DparmCode _code, int _sys_error, const T& _value) : DparmResult(_code, _sys_error), value(_value) {}
  DparmReturn(DparmCode _code, int _sys_error, T&& _value) : DparmResult(_code, _sys_error), value(std::move(_value)) {}
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_ERR_H_
