/**
 * @file	hash.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <string.h>

#include "hash.h"

namespace jcu {
namespace dparm {
namespace crypto {

static void bytes_xor_all(unsigned char *dest, int length, unsigned char value) {
  for (int i = 0; i < length; i++) {
    dest[i] ^= value;
  }
}

Hmac::Hmac(const HashFactory &hash_factory, const uint8_t *key, int key_len)
    : hash_factory_(hash_factory)
{
  int block_size = hash_factory.getBlockSize();
  std::vector<uint8_t> block_key;

  if (key_len > block_size) {
    auto h = hash_factory.createHash()->hash(key, key_len);
    while (h.size() < block_size) {
      std::vector<uint8_t> a = h;
      a.insert(a.end(), key, key + key_len);
      auto b = hash_factory.createHash()->hash(a);
      h.insert(h.end(), b.cbegin(), b.cend());
    }
    const uint8_t* tbegin = h.data();
    block_key.insert(block_key.end(), tbegin, tbegin + block_size);
  } else {
    block_key.resize(block_size);
    memcpy(block_key.data(), key, key_len);
  }

  o_key_pad_ = block_key;
  i_key_pad_ = block_key;

  bytes_xor_all(o_key_pad_.data(), o_key_pad_.size(), 0x5c);
  bytes_xor_all(i_key_pad_.data(), i_key_pad_.size(), 0x36);

  message_hash_ = hash_factory.createHash();
  reset();
}

int Hmac::getDigestSize() const {
  return hash_factory_.getDigestSize();
}

int Hmac::getBlockSize() const {
  return hash_factory_.getBlockSize();
}

void Hmac::update(const void *data, size_t length) {
  message_hash_->update(data, length);
}

std::vector<uint8_t> Hmac::digest() {
  auto a = message_hash_->digest();
  std::vector<uint8_t> b = o_key_pad_;
  b.insert(b.end(), a.cbegin(), a.cend());
  return hash_factory_.createHash()->hash(b);
}

void Hmac::reset() {
  message_hash_->reset();
  message_hash_->update(i_key_pad_);
}

} // namespace crypto
} // namespace dparm
} // namespace jcu
