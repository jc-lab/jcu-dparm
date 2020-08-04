/**
 * @file	ata_utils.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_ATA_UTILS_H_
#define JCU_DPARM_ATA_UTILS_H_

#include <stdint.h>

#include "ata_types.h"

namespace jcu {
namespace dparm {
namespace ata {

int needs_lba48(uint8_t ata_op, uint64_t lba, unsigned int nsect);
int is_dma(uint8_t ata_op);
void tf_init(struct ata_tf *tf, uint8_t ata_op, uint64_t lba, unsigned int nsect);

} // namespace ata
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_ATA_UTILS_H_
