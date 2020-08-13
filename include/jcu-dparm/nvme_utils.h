/**
 * @file	nvme_utils.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/13
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_NVME_UTILS_H_
#define JCU_DPARM_NVME_UTILS_H_

#include <stdint.h>

#include "nvme_types.h"

namespace jcu {
namespace dparm {
namespace nvme {

const char *nvmeStatusToString(NvmeStatusCode status);

} // namespace nvme
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_NVME_UTILS_H_
