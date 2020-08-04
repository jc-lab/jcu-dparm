/**
 * @file	intl_utils.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_INTL_UTILS_H_
#define JCU_DPARM_SRC_INTL_UTILS_H_

/** change the "endianess" of a 16bit field */
#define SWAP16(x) ((uint16_t) ((x & 0x00ff) << 8) | ((x & 0xff00) >> 8))
/** change the "endianess" of a 32bit field */
#define SWAP32(x) ((uint32_t) (((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8) \
	                        | (((x) & 0x00ff0000) >> 8) | (((x) & 0xff000000) >> 24))
/** change the "endianess" of a 64bit field */
#define SWAP64(x) (uint64_t) \
	((uint64_t) (SWAP32((x & 0x00000000ffffffff)) << 32) | \
	((uint64_t) (SWAP32((x >> 32))) )    \
	)

#endif // JCU_DPARM_SRC_INTL_UTILS_H_