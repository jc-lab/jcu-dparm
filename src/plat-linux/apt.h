/**
 * @file	apt.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

/*
 * References. hdparm
 */

#ifndef JCU_DPARM_SRC_PLAT_LINUX_APT_H_
#define JCU_DPARM_SRC_PLAT_LINUX_APT_H_

namespace jcu {
namespace dparm {

struct apt_data_struct;

struct scsi_sg_device;

/* Structs */
struct apt_usb_id_entry {
  int vendor_id;
  int product_id;
  int version;
  const char *type;
  int (*init_func)(scsi_sg_device *dev);
  int (*sg16_func)(scsi_sg_device *dev, int rw, int dma, ata::ata_tf_t *tf,
                   void *data, unsigned int data_bytes, unsigned int timeout_secs);

};

struct apt_data_struct {
  int is_apt;
  struct apt_usb_id_entry id;
  int verbose;
  union {
    struct {
      int port;
    } jmicron;
  };
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_LINUX_APT_H_
