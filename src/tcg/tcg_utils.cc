/**
 * @file	tcg_utils.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <jcu-dparm/tcg/tcg_utils.h>

namespace jcu {
namespace dparm {
namespace tcg {

const char *tcgStatusToString(OpalStatusCode status) {
  switch (status) {
    case SUCCESS:
      return "SUCCESS";
    case NOT_AUTHORIZED:
      return "NOT_AUTHORIZED";
      //	OBSOLETE = 0x02,
    case SP_BUSY:
      return "SP_BUSY";
    case SP_FAILED:
      return "SP_FAILED";
    case SP_DISABLED:
      return "SP_DISABLED";
    case SP_FROZEN:
      return "SP_FROZEN";
    case NO_SESSIONS_AVAILABLE:
      return "NO_SESSIONS_AVAILABLE";
    case UNIQUENESS_CONFLICT:
      return "UNIQUENESS_CONFLICT";
    case INSUFFICIENT_SPACE:
      return "INSUFFICIENT_SPACE";
    case INSUFFICIENT_ROWS:
      return "INSUFFICIENT_ROWS";
    case INVALID_FUNCTION:
      return "INVALID_FUNCTION";
    case INVALID_PARAMETER:
      return "INVALID_PARAMETER";
    case INVALID_REFERENCE:
      return "INVALID_REFERENCE";
      //	OBSOLETE = 0x0E,
    case TPER_MALFUNCTION:
      return "TPER_MALFUNCTION";
    case TRANSACTION_FAILURE:
      return "TRANSACTION_FAILURE";
    case RESPONSE_OVERFLOW:
      return "RESPONSE_OVERFLOW";
    case AUTHORITY_LOCKED_OUT:
      return "AUTHORITY_LOCKED_OUT";
    case FAIL:
      return "FAIL";
  }
  return nullptr;
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
