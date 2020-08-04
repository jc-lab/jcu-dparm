/**
 * @file	drive_factory.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_DRIVE_FACTORY_H_
#define JCU_DPARM_DRIVE_FACTORY_H_

#include <memory>
#include <stdint.h>

#include <stdio.h>

namespace jcu {
namespace dparm {

static void HEX_DUMPS(const void *data, int length) {
  const unsigned char *u = (const unsigned char *) data;
  for (int i = 0; i < length; i++) {
    printf("%02x ", u[i]);
  }
  printf("\n");
}

class DriveHandle;
class DriveFactory {
 public:
  static DriveFactory* getSystemFactory();

  virtual std::unique_ptr<DriveHandle> open(const char* drive_path) = 0;
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_DRIVE_FACTORY_H_
