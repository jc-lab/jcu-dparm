/**
 * @file	volume_finder.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/12/04
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <windows.h>
#include <tchar.h>

#include <jcu-dparm/drive_handle.h>

#include "volume_finder.h"

namespace jcu {
namespace dparm {
namespace plat_win {

typedef BOOL (WINAPI *fnGetVolumeInformationByHandleW_t)(
    HANDLE  hFile,
    LPWSTR  lpVolumeNameBuffer,
    DWORD   nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPWSTR  lpFileSystemNameBuffer,
    DWORD   nFileSystemNameSize
);

DparmResult Win32EnumVolumesContext::getResult() const {
  return result_;
}

void Win32EnumVolumesContext::init() {
  HANDLE vf_handle;
  TCHAR volume_name[MAX_PATH];
  TCHAR filesystem_name[64];

  size_t temp_buffer_size = (sizeof(VOLUME_DISK_EXTENTS) + sizeof(TCHAR) * 4096U) & (~0x3ffU);
  std::vector<unsigned char> temp_buffer(temp_buffer_size);

  volume_list_.clear();

  vf_handle = FindFirstVolume(volume_name, MAX_PATH);
  if (!vf_handle) {
    result_ = DparmResult { DPARME_SYS, (int)::GetLastError() };
    return ;
  }

  do {
    Win32VolumeInfo volume_info;
    size_t volume_name_len = _tcslen(volume_name);
    DWORD returned_bytes = 0;

    if (::GetVolumePathNamesForVolumeName(volume_name, (TCHAR*)temp_buffer.data(), temp_buffer.size() / sizeof(TCHAR), &returned_bytes) && returned_bytes > 0) {
      for (LPCTSTR psz = (LPCTSTR)temp_buffer.data(); * psz; psz += (_tcslen (psz) + 1)) {
        std::basic_string<TCHAR> temp_str(psz);
        volume_info.mount_points.emplace_back(std::string(temp_str.cbegin(), temp_str.cend()));
      }
    }

    if (::GetVolumeInformation(volume_name, nullptr, 0, nullptr, nullptr, nullptr, filesystem_name, _countof(filesystem_name))) {
      std::basic_string<TCHAR> wstr_filesystem(filesystem_name);
      volume_info.filesystem = std::string(wstr_filesystem.cbegin(), wstr_filesystem.cend());
    }

    if (volume_name_len && volume_name[volume_name_len - 1] == '\\') {
      volume_name_len--;
      volume_name[volume_name_len] = 0;
    }

    {
      std::basic_string<TCHAR> temp_str(volume_name);
      volume_info.path = std::string(temp_str.cbegin(), temp_str.cend());
    }

    HANDLE volume_handle = ::CreateFile(volume_name, 0, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (volume_handle && (volume_handle != INVALID_HANDLE_VALUE)) {
      DWORD returned_bytes = 0;

      if (::DeviceIoControl(
          volume_handle,
          IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
          nullptr,
          0,
          temp_buffer.data(),
          temp_buffer.size(),
          &returned_bytes,
          nullptr
      )) {
        const VOLUME_DISK_EXTENTS* volume_disk_extents = (const VOLUME_DISK_EXTENTS*)temp_buffer.data();
        volume_info.volume_disk_extents_buffer.clear();
        volume_info.volume_disk_extents_buffer.insert(
            volume_info.volume_disk_extents_buffer.end(),
            temp_buffer.data(), temp_buffer.data() + returned_bytes
        );
      }

      ::CloseHandle(volume_handle);

      volume_list_.emplace_back(std::move(volume_info));
    }
  } while (::FindNextVolume(vf_handle, volume_name, MAX_PATH));
  ::FindVolumeClose(vf_handle);
}

std::list<VolumeInfo> Win32EnumVolumesContext::findVolumesByDrive(const DriveInfo& drive_info) const {
  std::list<VolumeInfo> output;
  for (auto it_vol = volume_list_.cbegin(); it_vol != volume_list_.cend(); it_vol++) {
    const VOLUME_DISK_EXTENTS* volume_disk_extents =
        it_vol->volume_disk_extents_buffer.empty() ? nullptr :
        (const VOLUME_DISK_EXTENTS*)it_vol->volume_disk_extents_buffer.data();
    if (volume_disk_extents
        && volume_disk_extents->NumberOfDiskExtents > 0
        && volume_disk_extents->Extents[0].DiskNumber == drive_info.windows_dev_num) {
      output.emplace_back(*it_vol);
    }
  }
  return std::move(output);
}

std::list<VolumeInfo> Win32EnumVolumesContext::getList() const {
  std::list<VolumeInfo> output;
  for (auto it_vol = volume_list_.cbegin(); it_vol != volume_list_.cend(); it_vol++) {
    output.emplace_back(*it_vol);
  }
  return std::move(output);
}

} // namespace plat_win
} // namespace dparm
} // namespace jcu
