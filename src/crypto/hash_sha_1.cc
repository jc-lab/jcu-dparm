/**
 * @file	hash_sha_1.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/18
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <memory>
#include <string.h>

#include "hash_sha_1.h"

namespace jcu {
namespace dparm {
namespace crypto {

int HashSha1Factory::getDigestSize() const {
  return 20;
}

int HashSha1Factory::getBlockSize() const {
  return 64;
}

std::unique_ptr<Hash> HashSha1Factory::createHash() const {
  return std::unique_ptr<Hash>(new HashSha1());
}

HashSha1::HashSha1() {
  reset();
}

int HashSha1::getDigestSize() const {
  return 20;
}

int HashSha1::getBlockSize() const {
  return 64;
}

void HashSha1::update(const void *data, size_t length) {
  sha1Update((const unsigned char*)data, length);
}

std::vector<uint8_t> HashSha1::digest() {
  unsigned char buffer[20];
  sha1Final(buffer);
  return std::vector<uint8_t>(buffer, buffer + sizeof(buffer));
}

void HashSha1::reset() {
  sha1Init();
}

#define SHA1HANDSOFF

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#ifndef WORDS_BIGENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) | \
	(rol(block->l[i], 8) & 0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i & 15] = rol(block->l[(i + 13) & 15] ^ \
	block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15], 1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) \
	z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5A827999 + rol(v, 5); \
	w = rol(w, 30);
#define R1(v,w,x,y,z,i) \
	z += ((w & (x ^ y)) ^ y) + blk(i) + 0x5A827999 + rol(v, 5); \
	w = rol(w, 30);
#define R2(v,w,x,y,z,i) \
	z += (w ^ x ^ y) + blk(i) + 0x6ED9EBA1 + rol(v, 5); w = rol(w, 30);
#define R3(v,w,x,y,z,i) \
	z += (((w | x) & y) | (w & x)) + blk(i) + 0x8F1BBCDC + rol(v, 5); \
	w = rol(w, 30);
#define R4(v,w,x,y,z,i) \
	z += (w ^ x ^ y) + blk(i) + 0xCA62C1D6 + rol(v, 5); \
	w=rol(w, 30);


#ifdef VERBOSE  /* SAK */
void SHAPrintContext(SHA1_CTX *context, char *msg)
{
	printf("%s (%d,%d) %x %x %x %x %x\n",
	       msg,
	       count[0], count[1],
	       state[0],
	       state[1],
	       state[2],
	       state[3],
	       state[4]);
}
#endif

/* Hash a single 512-bit block. This is the core of the algorithm. */

void HashSha1::sha1Transform(uint32_t state[5], const unsigned char buffer[64])
{
  uint32_t a, b, c, d, e;
  typedef union {
    unsigned char c[64];
    uint32_t l[16];
  } CHAR64LONG16;
  CHAR64LONG16* block;
#ifdef SHA1HANDSOFF
  CHAR64LONG16 workspace;
  block = &workspace;
  memcpy(block, buffer, 64);
#else
  block = (CHAR64LONG16 *) buffer;
#endif
  /* Copy state[] to working vars */
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  /* 4 rounds of 20 operations each. Loop unrolled. */
  R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
  R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
  R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
  R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
  R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
  R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
  R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
  R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
  R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
  R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
  R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
  R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
  R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
  R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
  R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
  R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
  R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
  R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
  R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
  R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
  /* Add the working vars back into context.state[] */
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  /* Wipe variables */
  a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
  memset(block, 0, 64);
#endif
}

/* sha1Init - Initialize new context */
void HashSha1::sha1Init()
{
  /* SHA1 initialization constants */
  state_[0] = 0x67452301;
  state_[1] = 0xEFCDAB89;
  state_[2] = 0x98BADCFE;
  state_[3] = 0x10325476;
  state_[4] = 0xC3D2E1F0;
  count_[0] = count_[1] = 0;
  memset(buffer_, 0, sizeof(buffer_));
}

/* Run your data through this. */
void HashSha1::sha1Update(const unsigned char *data, uint32_t len)
{
  uint32_t i, j;

#ifdef VERBOSE
  SHAPrintContext(context, "before");
#endif
  j = (count_[0] >> 3) & 63;
  if ((count_[0] += len << 3) < (len << 3))
    count_[1]++;
  count_[1] += (len >> 29);
  if ((j + len) > 63) {
    memcpy(&buffer_[j], data, (i = 64-j));
    sha1Transform(state_, buffer_);
    for ( ; i + 63 < len; i += 64) {
      sha1Transform(state_, &data[i]);
    }
    j = 0;
  }
  else i = 0;
  memcpy(&buffer_[j], &data[i], len - i);
#ifdef VERBOSE
  SHAPrintContext(context, "after ");
#endif
}

/* Add padding and return the message digest. */
void HashSha1::sha1Final(unsigned char digest[20]) {
  uint32_t i;
  unsigned char finalcount[8];

  for (i = 0; i < 8; i++) {
    finalcount[i] = (unsigned char) ((count_[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255); /* Endian independent */
  }
  sha1Update((unsigned char *) "\200", 1);
  while ((count_[0] & 504) != 448) {
    sha1Update((unsigned char *) "\0", 1);
  }
  sha1Update(finalcount, 8); /* Should cause a sha1Transform()
					      */
  for (i = 0; i < 20; i++) {
    digest[i] = (unsigned char) ((state_[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
  }
  /* Wipe variables */
  i = 0;
  memset(buffer_, 0, 64);
  memset(state_, 0, 20);
  memset(count_, 0, 8);
  memset(finalcount, 0, 8);
}

} // namespace crypto
} // namespace dparm
} // namespace jcu
