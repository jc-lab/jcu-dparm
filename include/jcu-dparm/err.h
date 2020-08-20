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
  DPARME_NOT_IMPL = 3,
  DPARME_ILLEGAL_DATA = 11,
  DPARME_ILLEGAL_RESPONSE = 12,
  DPARME_IOCTL_FAILED = 21,
  DPARME_IO_TIMEOUT = 22,
  DPARME_ATA_FAILED = 31,
  DPARME_NVME_FAILED = 34,
  DPARME_OPERATION_TIMEOUT = 40,
  DPARME_TCG_ERROR_STATUS = 104,
  DPARME_TCG_AUTH_FAILED = 105,
};

struct DparmResult {
  DparmCode code;
  int sys_error;
  int32_t drive_status;

  DparmResult() : code(DPARME_OK), sys_error(0), drive_status(0) {}
  DparmResult(DparmCode code, int sys_error) : code(code), sys_error(sys_error), drive_status(0) {}
  DparmResult(DparmCode code, int sys_error, int32_t drive_status) : code(code), sys_error(sys_error), drive_status(drive_status) {}

  bool isOk() const {
    return code == DPARME_OK;
  }
};

template<typename T>
struct DparmReturn : DparmResult {
  T value;

  DparmReturn() : DparmResult(DPARME_OK, 0), value() {}
  DparmReturn(DparmCode _code, int _sys_error) : DparmResult(_code, _sys_error), value() {}
  DparmReturn(DparmCode _code, int _sys_error, int32_t _drive_status) : DparmResult(_code, _sys_error, _drive_status), value() {}
  DparmReturn(DparmCode _code, int _sys_error, int32_t _drive_status, const T& _value) : DparmResult(_code, _sys_error, _drive_status), value(_value) {}
  DparmReturn(DparmCode _code, int _sys_error, int32_t _drive_status, T&& _value) : DparmResult(_code, _sys_error, _drive_status), value(std::move(_value)) {}
  DparmReturn(const DparmResult& _result, const T& _value) : DparmResult(_result), value(_value) {}
  DparmReturn(const DparmResult& _result, T&& _value) : DparmResult(_result), value(std::move(_value)) {}
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_ERR_H_
