/**
 * @file	pbkdf2.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "pbkdf2.h"
#include "hash.h"

namespace jcu {
namespace dparm {
namespace crypto {

static void F(unsigned char *tbuf, int ti_offset, Hmac *prf, const PBEKeySpec *key_spec, int block_index, unsigned char *U_r, unsigned char *U_i);

static void bytes_xor(unsigned char *dest, const unsigned char *src, int length) {
  for (int i = 0; i < length; i++) {
    dest[i] ^= src[i];
  }
}

static void int_to_bytes(uint8_t *dest, uint32_t i) {
  dest[0] = (uint8_t) (i >> 24U);
  dest[1] = (uint8_t) (i >> 16U);
  dest[2] = (uint8_t) (i >> 8U);
  dest[3] = (uint8_t) (i);
}

std::vector<uint8_t> pbkdf2(
    HashFactory& factory,
    const PBEKeySpec& spec,
    std::vector<uint8_t> password
) {
  Hmac prf(factory, password.data(), password.size());
  int hash_len = prf.getDigestSize();
  int l = (spec.key_length > hash_len) ? spec.key_length : hash_len;
  std::vector<unsigned char> tbuf(l * hash_len);
  int ti_offset = 0;

  std::vector<unsigned char> U_r(hash_len);
  std::vector<unsigned char> U_i(spec.salt.size() + 4);

  for (int i = 1; i <= l; i++) {
    memset(U_r.data(), 0, U_r.size());
    F(tbuf.data(), ti_offset, &prf, &spec, i, &U_r[0], &U_i[0]);
    ti_offset += hash_len;
  }

  return std::vector<uint8_t>(tbuf.data(), tbuf.data() + spec.key_length);
}

static void F(unsigned char *tbuf, int ti_offset, Hmac *prf, const PBEKeySpec *key_spec, int block_index, unsigned char *U_r, unsigned char *U_i) {
  int hash_len = prf->getDigestSize();
  const auto& salt = key_spec->salt;
  size_t U_i_len = salt.size() + 4;
  memcpy(U_i, salt.data(), salt.size());
  int_to_bytes(&U_i[salt.size()], block_index);

  std::vector<unsigned char> block(U_i, U_i + U_i_len);

  for (int i = 0; i < key_spec->iteration; i++) {
    prf->reset();
    prf->update(block.data(), block.size());
    std::vector<uint8_t> digest = prf->digest();
    block = digest;
    bytes_xor(U_r, block.data(), hash_len);
  }

  memcpy(&tbuf[ti_offset], U_r, hash_len);
}


} // namespace crypto
} // namespace dparm
} // namespace jcu
