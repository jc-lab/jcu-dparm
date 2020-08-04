/**
 * @file	ata_utils.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <string.h>

#include <jcu-dparm/ata_utils.h>

#define lba28_limit ((uint64_t)(1<<28) - 1)

namespace jcu {
namespace dparm {
namespace ata {

int needs_lba48 (uint8_t ata_op, uint64_t lba, unsigned int nsect)
{
  switch (ata_op) {
    case ATA_OP_DSM:
    case ATA_OP_READ_PIO_EXT:
    case ATA_OP_READ_DMA_EXT:
    case ATA_OP_WRITE_PIO_EXT:
    case ATA_OP_WRITE_DMA_EXT:
    case ATA_OP_READ_VERIFY_EXT:
    case ATA_OP_WRITE_UNC_EXT:
    case ATA_OP_READ_NATIVE_MAX_EXT:
    case ATA_OP_SET_MAX_EXT:
    case ATA_OP_FLUSHCACHE_EXT:
      return 1;
    case ATA_OP_SECURITY_ERASE_PREPARE:
    case ATA_OP_SECURITY_ERASE_UNIT:
    case ATA_OP_VENDOR_SPECIFIC_0x80:
    case ATA_OP_SMART:
      return 0;
  }
  if (lba >= lba28_limit)
    return 1;
  if (nsect) {
    if (nsect > 0xff)
      return 1;
    if ((lba + nsect - 1) >= lba28_limit)
      return 1;
  }
  return 0;
}

int is_dma (uint8_t ata_op)
{
  switch (ata_op) {
    case ATA_OP_DSM:
    case ATA_OP_READ_DMA_EXT:
    case ATA_OP_READ_FPDMA:
    case ATA_OP_WRITE_DMA_EXT:
    case ATA_OP_WRITE_FPDMA:
    case ATA_OP_READ_DMA:
    case ATA_OP_WRITE_DMA:
      return 1 /* SG_DMA */;
    default:
      return 0 /* SG_PIO */;
  }
}

void tf_init (struct ata_tf *tf, uint8_t ata_op, uint64_t lba, unsigned int nsect)
{
  memset(tf, 0, sizeof(*tf));
  tf->command  = ata_op;
  tf->dev      = ATA_USING_LBA;
  tf->lob.lbal = lba;
  tf->lob.lbam = lba >>  8;
  tf->lob.lbah = lba >> 16;
  tf->lob.nsect = nsect;
  if (needs_lba48(ata_op, lba, nsect)) {
    tf->is_lba48 = 1;
    tf->hob.nsect = nsect >> 8;
    tf->hob.lbal = lba >> 24;
    tf->hob.lbam = lba >> 32;
    tf->hob.lbah = lba >> 40;
  } else {
    tf->dev |= (lba >> 24) & 0x0f;
  }
}

}
}
}

