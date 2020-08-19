/**
 * @file	tcg_intl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_TCG_INTL_H
#define JCU_DPARM_SRC_TCG_INTL_H

#include <stdint.h>
#include <vector>

#include <jcu-dparm/tcg/tcg_device.h>

namespace jcu {
namespace dparm {
namespace tcg {

void tcgHashPassword(TcgDevice* device, bool no_hash_password, std::vector<uint8_t>& out_hash, const char* password);
void tcgHashPasswordPbkdf2(std::vector<uint8_t>& out_hash, std::vector<uint8_t>& salt, int iteration, int hash_size, const char* password);

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_TCG_INTL_H
