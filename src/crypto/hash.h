/**
 * @file	hash.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_HASH_H
#define JCU_DPARM_HASH_H

#include <memory>
#include <vector>

namespace jcu {
namespace dparm {
namespace crypto {

class Hash;

class HashFactory {
 public:
  virtual int getDigestSize() const = 0;
  virtual int getBlockSize() const = 0;
  virtual std::unique_ptr<Hash> createHash() const = 0;
};

class Hash {
 public:
  virtual int getDigestSize() const = 0;
  virtual int getBlockSize() const = 0;
  virtual void update(const void* data, size_t length) = 0;
  virtual std::vector<uint8_t> digest() = 0;
  virtual void reset() = 0;

  template<typename T>
  void update(const std::vector<T>& data) {
    size_t bytes_size = sizeof(T) * data.size();
    update(data.data(), bytes_size);
  }

  std::vector<uint8_t> hash(const void* data, size_t length) {
    update(data, length);
    return digest();
  }
  std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
    return hash(data.data(), data.size());
  }
};

class Hmac : public Hash {
 public:
  Hmac(const HashFactory& hash_factory, const uint8_t* key, int key_len);
  int getDigestSize() const override;
  int getBlockSize() const override;
  void update(const void *data, size_t length) override;
  std::vector<uint8_t> digest() override;
  void reset() override;

 private:
  const HashFactory& hash_factory_;
  std::vector<uint8_t> i_key_pad_;
  std::vector<uint8_t> o_key_pad_;
  std::unique_ptr<Hash> message_hash_;
};

} // namespace crypto
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_HASH_H
