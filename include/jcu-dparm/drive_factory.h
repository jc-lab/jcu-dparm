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

#include <stdint.h>

#include <memory>
#include <list>

#include "err.h"
#include "types.h"

namespace jcu {
namespace dparm {

class DriveHandle;
class DriveFactory {
 public:
  static DriveFactory* getSystemFactory();
  static std::shared_ptr<DriveFactory> createSystemFactory(const DriveFactoryOptions& options);

  virtual std::unique_ptr<DriveHandle> open(const char* drive_path) const = 0;
  virtual int enumDrives(std::list<DriveInfo>& result_list) const = 0;
};

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_DRIVE_FACTORY_H_
