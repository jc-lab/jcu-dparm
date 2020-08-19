/**
 * @file	tcg_intl.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <string>

#include <jcu-dparm/drive_handle.h>
#include "tcg_intl.h"

#include "../crypto/hash.h"
#include "../crypto/hash_sha_1.h"
#include "../crypto/pbkdf2.h"

namespace jcu {
namespace dparm {
namespace tcg {

void tcgHashPassword(TcgDevice* device, bool no_hash_password, std::vector<uint8_t>& out_hash, const char* password) {
  int password_length = strnlen(password, 32);
  out_hash.clear();
  if (no_hash_password) {
    out_hash.resize(2 + password_length);
    out_hash[0] = 0xd0;
    out_hash[1] = password_length;
    memcpy(&out_hash[2], password, password_length);
  } else {
    const auto& drive_info = device->getDriveHandle()->getDriveInfo();
    std::vector<unsigned char> salt(drive_info.raw_serial, drive_info.raw_serial + sizeof(drive_info.raw_serial));
    std::vector<unsigned char> vpassword(password, password + password_length);
    crypto::HashSha1Factory factory;
    crypto::PBEKeySpec key_spec;
    key_spec.iteration = 75000;
    key_spec.key_length = 32;
    std::vector<uint8_t> derived = crypto::pbkdf2(factory, key_spec, vpassword);
    out_hash.resize(2 + derived.size());
    out_hash[0] = 0xd0;
    out_hash[1] = derived.size();
    memcpy(&out_hash[2], derived.data(), derived.size());
  }
}


} // namespace tcg
} // namespace dparm
} // namespace jcu
