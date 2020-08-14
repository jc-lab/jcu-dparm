/**
 * @file	driver_utils.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/23
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <windows.h>
#include <ntddscsi.h>

#include "driver_utils.h"
#include "../../intl_utils.h"

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

DparmReturn<InquiryDeviceResult> getInquiryDeviceInfoImpl(HANDLE handle) {
  InquiryDeviceResult info = {};
  std::vector<unsigned char> payload_buffer(4096);

  STORAGE_PROPERTY_QUERY storage_property_query;
  PSTORAGE_DEVICE_DESCRIPTOR storage_device_descriptor = (PSTORAGE_DEVICE_DESCRIPTOR)payload_buffer.data();
  DWORD read_bytes;
  ZeroMemory(&storage_property_query, sizeof(STORAGE_PROPERTY_QUERY));
  storage_property_query.PropertyId = StorageDeviceProperty;
  storage_property_query.QueryType = PropertyStandardQuery;
  if (!DeviceIoControl(
      handle, IOCTL_STORAGE_QUERY_PROPERTY,
      &storage_property_query, sizeof(storage_property_query),
      storage_device_descriptor, payload_buffer.size(),
      &read_bytes, NULL)) {
    return { DPARME_IOCTL_FAILED, (int)::GetLastError() };
  }

  if (storage_device_descriptor->VendorIdOffset > 0) {
    // TODO: Not sure...
    info.vendor_identification = intl::trimString(intl::readStringRange(
        payload_buffer.data(),
        storage_device_descriptor->VendorIdOffset,
        payload_buffer.size(),
        true));
  }

  if (storage_device_descriptor->ProductIdOffset > 0) {
    info.product_identification = intl::trimString(intl::readStringRange(
        payload_buffer.data(),
        storage_device_descriptor->ProductIdOffset,
        payload_buffer.size(),
        true));
  }

  if (storage_device_descriptor->SerialNumberOffset > 0) {
    info.drive_serial_number = intl::trimString(intl::readStringRange(
        payload_buffer.data(),
        storage_device_descriptor->SerialNumberOffset,
        payload_buffer.size(),
        true));
  }

  if (storage_device_descriptor->ProductRevisionOffset > 0) {
    info.product_revision_level = intl::trimString(intl::readStringRange(
        payload_buffer.data(),
        storage_device_descriptor->ProductRevisionOffset,
        payload_buffer.size(),
        true));
  }

  return { DPARME_OK, 0, 0, info };
}

} //namespace drivers
} //namespace plat_win
} //namespace dparm
} //namespace jcu
