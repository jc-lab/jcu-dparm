/**
 * @file	volume_finder.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/12/04
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_WIN_VOLUME_FINDER_H_
#define JCU_DPARM_SRC_PLAT_WIN_VOLUME_FINDER_H_

#include <jcu-dparm/drive_factory.h>

namespace jcu {
namespace dparm {
namespace plat_win {

struct Win32VolumeInfo : public VolumeInfo {
  std::vector<unsigned char> volume_disk_extents_buffer;

  bool hasVolumeDiskExtents() const {
    return volume_disk_extents_buffer.size() > 0;
  }
};

class Win32EnumVolumesContext : public EnumVolumesContext {
 private:
  DparmResult result_;
  std::list<Win32VolumeInfo> volume_list_;

 public:
  DparmResult getResult() const override;
  void init();
  std::list<VolumeInfo> findVolumesByDrive(const DriveInfo& drive_info) const override;
  std::list<VolumeInfo> getList() const override;
};

} // namespace plat_win
} // namespace dparm
} // namespace jcu

#endif//JCU_DPARM_SRC_PLAT_WIN_VOLUME_FINDER_H_
