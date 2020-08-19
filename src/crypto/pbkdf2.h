/**
 * @file	pbkdf2.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_PBKDF2_H
#define JCU_DPARM_PBKDF2_H

#include <vector>
#include "hash.h"

namespace jcu {
namespace dparm {
namespace crypto {

class Hash;
class Hmac;

struct PBEKeySpec {
  int iteration;
  int key_length; // bytes
  std::vector<uint8_t> salt;
};

std::vector<uint8_t> pbkdf2(
    HashFactory& factory,
    const PBEKeySpec& spec,
    std::vector<uint8_t> password
    );

} // namespace crypto
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_PBKDF2_H
