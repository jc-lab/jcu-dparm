/**
 * @file	tcg_utils.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_UTILS_H_
#define JCU_DPARM_TCG_TCG_UTILS_H_

#include <stdint.h>

#include <vector>
#include <string>

#include "../err.h"
#include "tcg_types.h"
#include "tcg_command.h"
#include "tcg_response.h"

namespace jcu {
namespace dparm {
namespace tcg {

const char *tcgStatusToString(OpalStatusCode status);

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_UTILS_H_
