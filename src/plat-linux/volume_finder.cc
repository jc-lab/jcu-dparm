/**
 * @file	volume_finder.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/12/04
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <errno.h>

#include "volume_finder.h"

namespace jcu {
namespace dparm {
namespace plat_linux {

DparmResult LinuxEnumVolumesContext::getResult() const {
  return result_;
}

void LinuxEnumVolumesContext::init() {
  FILE* fp = ::fopen("/proc/mounts", "r");
  std::vector<char> line_buf(512);
  char* line_ptr;

  volume_list_.clear();

  if (!fp) {
    result_ = DparmResult { DPARME_SYS, errno };
    return;
  }

  while((line_ptr = fgets(line_buf.data(), line_buf.size(), fp)) != nullptr) {
    char* token_ctx = nullptr;
    const char* device_path = strtok_r(line_ptr, " ", &token_ctx);
    const char* mount_path = strtok_r(nullptr, " ", &token_ctx);
    const char* filesystem = strtok_r(nullptr, " ", &token_ctx);
    const char* mount_options = strtok_r(nullptr, " ", &token_ctx);

    LinuxVolumeInfo& volume_info = volume_list_[device_path];
    if (volume_info.path.empty()) {
      volume_info.path = device_path;
      volume_info.filesystem = filesystem;
    }
    volume_info.mount_points.emplace_back(mount_path);
  }

  ::fclose(fp);
}

std::list<VolumeInfo> LinuxEnumVolumesContext::findVolumesByDrive(const DriveInfo& drive_info) const {
  std::list<VolumeInfo> output;
  for (auto it = volume_list_.cbegin(); it != volume_list_.cend(); it++) {
    if (it->second.path.find(drive_info.device_path) == 0) {
      output.emplace_back(it->second);
    }
  }
  return output;
}

std::list<VolumeInfo> LinuxEnumVolumesContext::getList() const {
  std::list<VolumeInfo> output;
  for (auto it_vol = volume_list_.cbegin(); it_vol != volume_list_.cend(); it_vol++) {
    output.emplace_back(it_vol->second);
  }
  return std::move(output);
}

} // namespace plat_linux
} // namespace dparm
} // namespace jcu
