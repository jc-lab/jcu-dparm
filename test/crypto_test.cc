#include <gtest/gtest.h>

#include "../src/crypto/hash.h"
#include "../src/crypto/hash_sha_1.h"
#include "../src/crypto/pbkdf2.h"

using namespace jcu::dparm::crypto;

namespace {

struct HmacTestVector {
  std::vector<uint8_t> key;
  std::vector<uint8_t> data;
  std::vector<uint8_t> digest;
};

struct PBKDF2TestVector {
  const char* password;
  int password_len;
  const char* salt;
  int salt_len;
  int iteration;
  int dk_len;
  std::vector<uint8_t> dk;
};

class Sha1Test : public ::testing::Test {};

/**
 * rfc2202: Test Cases for HMAC-MD5 and HMAC-SHA-1
 */
#define HMAC_TEST_VECTOR_COUNT 3
static HmacTestVector hmac_test_vectors[HMAC_TEST_VECTOR_COUNT] = {
    {{0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b},
        {'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e'},
        {0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64, 0xe2, 0x8b, 0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e, 0xf1, 0x46, 0xbe, 0x00}},
    {{0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
        {0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd},
        {0x12, 0x5d, 0x73, 0x42, 0xb9, 0xac, 0x11, 0xcd, 0x91, 0xa3, 0x9a, 0xf4, 0x8a, 0xa1, 0x7b, 0x4f, 0x63, 0xf1, 0x75, 0xd3}},
    {{0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c},
        {'T', 'e', 's', 't', ' ', 'W', 'i', 't', 'h', ' ', 'T', 'r', 'u', 'n', 'c', 'a', 't', 'i', 'o', 'n'},
        {0x4c, 0x1a, 0x03, 0x42, 0x4b, 0x55, 0xe0, 0x7f, 0xe7, 0xf2, 0x7b, 0xe1, 0xd5, 0x8b, 0xb9, 0x32, 0x4a, 0x9a, 0x5a, 0x04}}};

/**
 * rfc6070: PBKDF2 Test Vectors
 */
#define PBKDF2_TEST_VECTOR_COUNT 3
static PBKDF2TestVector pbkdf2_test_vectors[PBKDF2_TEST_VECTOR_COUNT] = {
    {
        "password", 8, "salt", 4, 4096, 20,
        {0x4b, 0x00, 0x79, 0x01, 0xb7, 0x65, 0x48, 0x9a, 0xbe, 0xad, 0x49, 0xd9, 0x26, 0xf7, 0x21, 0xd0, 0x65, 0xa4, 0x29, 0xc1 }
    },
    {
        "passwordPASSWORDpassword", 24, "saltSALTsaltSALTsaltSALTsaltSALTsalt", 36, 4096, 25,
        { 0x3d,0x2e,0xec,0x4f,0xe4,0x1c,0x84,0x9b,0x80,0xc8,0xd8,0x36,0x62,0xc0,0xe4,0x4a,0x8b,0x29,0x1a,0x96,0x4c,0xf2,0xf0,0x70,0x38 }
    },
    {
        "pass\0word", 9, "sa\0lt", 5, 4096, 16,
        { 0x56, 0xfa, 0x6a, 0xa7, 0x55, 0x48, 0x09, 0x9d, 0xcc, 0x37, 0xd7, 0xf0, 0x34, 0x25, 0xe0, 0xc3 }
    }
};

/**
 * Test Vector from FIPS PUB 180-1
 */
TEST(Sha1Test, DIGEST_TEST_VECTOR_1) {
  HashSha1 hash;
  auto out = hash.hash("abc", 3);
  std::vector<uint8_t> expect_value = {
      0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D};
  EXPECT_EQ(out, expect_value);
}

/**
 * Test Vector from FIPS PUB 180-1
 */
TEST(Sha1Test, DIGEST_TEST_VECTOR_2) {
  HashSha1 hash;
  const char *test_vector_input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  auto out = hash.hash(test_vector_input, strlen(test_vector_input));
  std::vector<uint8_t> expect_value = {
      0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1};
  EXPECT_EQ(out, expect_value);
}

TEST(Sha1Test, HMAC_TEST_VECTORS) {
  HashSha1Factory factory;

  for (int i=0; i<HMAC_TEST_VECTOR_COUNT; i++) {
    auto test_vector = hmac_test_vectors[i];
    Hmac hmac(factory, test_vector.key.data(), test_vector.key.size());
    hmac.update(test_vector.data.data(), test_vector.data.size());
    auto out = hmac.digest();
    EXPECT_EQ(out, test_vector.digest);
  }
}

/**
 * ietf RFC-6070
 */
TEST(Sha1Test, PBKDF2_TEST_VECTOR) {
  HashSha1Factory factory;

  for (int i=0; i<PBKDF2_TEST_VECTOR_COUNT; i++) {
    auto test_vector = pbkdf2_test_vectors[i];
    printf("Test case #%d\n", i);
    PBEKeySpec key_spec;
    key_spec.iteration = test_vector.iteration;
    key_spec.key_length = test_vector.dk_len;
    key_spec.salt = std::vector<uint8_t>(test_vector.salt, test_vector.salt + test_vector.salt_len);
    std::vector<uint8_t> password = std::vector<uint8_t>(test_vector.password, test_vector.password + test_vector.password_len);
    auto out = pbkdf2(factory, key_spec, password);
    EXPECT_EQ(out, test_vector.dk);
  }
}

} // namespace
