/**
 * @file	driver_utils.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include "driver_utils.h"

#include <windows.h>
#include <ntddscsi.h>

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

int getScsiPath(std::basic_string<TCHAR> *pout, const std::basic_string<TCHAR>& str_path) {
  SCSI_ADDRESS sadr;
  BOOL bRet = 0;
  DWORD dwReturned;
  DWORD dwError;
  HANDLE hIoCtrl = CreateFile(
      str_path.c_str(),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      0
  );
  if ((!hIoCtrl) || (hIoCtrl == INVALID_HANDLE_VALUE)) {
    return (int)::GetLastError();
  }

  bRet = DeviceIoControl(
      hIoCtrl,
      IOCTL_SCSI_GET_ADDRESS,
      NULL,
      0,
      &sadr,
      sizeof(sadr),
      &dwReturned,
      NULL
  );
  dwError = ::GetLastError();
  ::CloseHandle(hIoCtrl);

  if (!bRet) {
    return (int)dwError;
  }

  TCHAR result_buf[32];
  _stprintf_s(result_buf, _T("\\\\.\\SCSI%d:"), sadr.PortNumber);
  *pout = std::basic_string<TCHAR>(result_buf);

  return 0;
}

} //namespace drivers
} //namespace plat_win
} //namespace dparm
} //namespace jcu
