/**
 * @file	hash_sha_1.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_HASH_SHA_1_H
#define JCU_DPARM_HASH_SHA_1_H

#include "hash.h"

namespace jcu {
namespace dparm {
namespace crypto {

class HashSha1 : public Hash {
 public:
  HashSha1();
  int getDigestSize() const override;
  int getBlockSize() const override;
  void update(const void *data, size_t length) override;
  std::vector<uint8_t> digest() override;
  void reset() override;

 private:
  uint32_t state_[5];
  uint32_t count_[2];
  unsigned char buffer_[64];

  void sha1Init();
  void sha1Update(const unsigned char *data, uint32_t len);
  void sha1Final(unsigned char digest[20]);
  void sha1Transform(uint32_t state[5], const unsigned char buffer[64]);
};

class HashSha1Factory : public HashFactory {
 public:
  int getDigestSize() const override;
  int getBlockSize() const override;
  std::unique_ptr<Hash> createHash() const;
};

} // namespace crypto
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_HASH_SHA_1_H
