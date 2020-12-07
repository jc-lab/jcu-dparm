/**
 * @file	volume_finder.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/12/04
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_LINUX_VOLUME_FINDER_H_
#define JCU_DPARM_SRC_PLAT_LINUX_VOLUME_FINDER_H_

#include <map>

#include <jcu-dparm/drive_factory.h>

namespace jcu {
namespace dparm {
namespace plat_linux {

struct LinuxVolumeInfo : public VolumeInfo {
};

class LinuxEnumVolumesContext : public EnumVolumesContext {
 private:
  DparmResult result_;
  std::map<std::string, LinuxVolumeInfo> volume_list_;

 public:
  DparmResult getResult() const override;
  void init();
  std::list<VolumeInfo> findVolumesByDrive(const DriveInfo &drive_info) const override;
  std::list<VolumeInfo> getList() const override;
};

} // namespace jcu
} // namespace dparm
} // namespace plat_linux

#endif//JCU_DPARM_SRC_PLAT_LINUX_VOLUME_FINDER_H_
