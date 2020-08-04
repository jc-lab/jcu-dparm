/**
 * @file	sysfs_utils.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_LINUX_SYSFS_UTILS_H_
#define JCU_DPARM_SRC_PLAT_LINUX_SYSFS_UTILS_H_

namespace jcu {
namespace dparm {

int sysfs_get_attr (int fd, const char *attr, const char *fmt, void *val1, void *val2, int verbose);
int sysfs_set_attr (int fd, const char *attr, const char *fmt, void *val_p, int verbose);
int sysfs_get_attr_recursive (int fd, const char *attr, const char *fmt, void *val1, void *val2, int verbose);

} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_LINUX_SYSFS_UTILS_H_
