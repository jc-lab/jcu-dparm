/**
 * @file	physical_drive_finder.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/07
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_WIN_PHYSICAL_DRIVE_FINDER_H_
#define JCU_DPARM_SRC_PLAT_WIN_PHYSICAL_DRIVE_FINDER_H_

#include <string>
#include <list>
#include <memory>

namespace jcu {
namespace dparm {
namespace plat_win {

struct WindowsPhysicalDrive {
  int device_index;
  std::string device_path;
};

int enumPhysicalDrives(std::list<WindowsPhysicalDrive>& devices);

} // namespace plat_win
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_SRC_PLAT_WIN_PHYSICAL_DRIVE_FINDER_H_
