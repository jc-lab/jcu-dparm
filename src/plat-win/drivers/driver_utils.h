/**
 * @file	driver_utils.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_WIN_DRIVERS_DRIVER_UTILS_H_
#define JCU_DPARM_SRC_PLAT_WIN_DRIVERS_DRIVER_UTILS_H_

#include <string>
#include <tchar.h>

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

int getScsiPath(std::basic_string<TCHAR> *pout, const std::basic_string<TCHAR>& str_path);

} //namespace drivers
} //namespace plat_win
} //namespace dparm
} //namespace jcu

#endif //JCU_DPARM_SRC_PLAT_WIN_DRIVERS_DRIVER_UTILS_H_
