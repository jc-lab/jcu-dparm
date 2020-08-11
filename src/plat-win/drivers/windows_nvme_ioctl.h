/**
 * @file	windows_nvme_ioctl.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/08/11
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_PLAT_WIN_DRIVERS_WINDOWS_NVME_IOCTL_H_
#define JCU_DPARM_SRC_PLAT_WIN_DRIVERS_WINDOWS_NVME_IOCTL_H_

#include <windows.h>

#include <nvme.h> // SDK Version >= 10

namespace jcu {
namespace dparm {
namespace plat_win {
namespace drivers {

// Reference: https://www.naraeon.net/windows-10-nvme-get-features-sample-cpp/
// Reference: 10.0.16299.0/shared/nvme.h

namespace windows10 {

#if 0
typedef enum {
    StorageDeviceProperty = 0,
    StorageAdapterProperty,
    StorageDeviceIdProperty,
    StorageDeviceUniqueIdProperty,
    StorageDeviceWriteCacheProperty,
    StorageMiniportProperty,
    StorageAccessAlignmentProperty,
    StorageDeviceSeekPenaltyProperty,
    StorageDeviceTrimProperty,
    StorageDeviceWriteAggregationProperty,
    StorageDeviceDeviceTelemetryProperty,
    StorageDeviceLBProvisioningProperty,
    StorageDevicePowerProperty,
    StorageDeviceCopyOffloadProperty,
    StorageDeviceResiliencyProperty,
    StorageDeviceMediumProductType,
    StorageDeviceRpmbProperty,
    StorageDeviceIoCapabilityProperty = 48,
    StorageAdapterProtocolSpecificProperty,
    StorageDeviceProtocolSpecificProperty,
    StorageAdapterTemperatureProperty,
    StorageDeviceTemperatureProperty,
    StorageAdapterPhysicalTopologyProperty,
    StorageDevicePhysicalTopologyProperty,
    StorageDeviceAttributesProperty,
} TStoragePropertyId;

typedef enum {
    PropertyStandardQuery = 0,
    PropertyExistsQuery,
    PropertyMaskQuery,
    PropertyQueryMaxDefined
} TStorageQueryType;

typedef enum {
    ProtocolTypeUnknown = 0x00,
    ProtocolTypeScsi,
    ProtocolTypeAta,
    ProtocolTypeNvme,
    ProtocolTypeSd,
    ProtocolTypeProprietary = 0x7E,
    ProtocolTypeMaxReserved = 0x7F
} TStroageProtocolType;

typedef struct {
    TStroageProtocolType ProtocolType;
    DWORD DataType;
    DWORD ProtocolDataRequestValue;
    DWORD ProtocolDataRequestSubValue;
    DWORD ProtocolDataOffset;
    DWORD ProtocolDataLength;
    DWORD FixedProtocolReturnData;
    DWORD Reserved[3];
} TStorageProtocolSpecificData;

typedef enum {
    NVMeDataTypeUnknown = 0,
    NVMeDataTypeIdentify,
    NVMeDataTypeLogPage,
    NVMeDataTypeFeature,
} TStorageProtocolNVMeDataType;

enum TNvmeIdentifyCnsCode {
  NVME_IDENTIFY_CNS_SPECIFIC_NAMESPACE = 0,
  NVME_IDENTIFY_CNS_CONTROLLER = 1,
  NVME_IDENTIFY_CNS_ACTIVE_NAMESPACES = 2,// A list of up to 1024 active namespace IDs is returned to the host containing active namespaces with a namespace identifier greater than the value specified in the Namespace Identifier (CDW1.NSID) field.
};
#endif

typedef struct TStoragePropertyQuery {
  STORAGE_PROPERTY_ID PropertyId;
  STORAGE_QUERY_TYPE QueryType;
} TStoragePropertyQuery_t;


typedef struct {
  TStoragePropertyQuery query;
  STORAGE_PROTOCOL_SPECIFIC_DATA protocol_specific;
  BYTE buffer[4096];
} TStorageQueryWithBuffer;

}// namespace drivers

} // namespace drivers
} // namespace plat_win
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_PLAT_WIN_DRIVERS_WINDOWS_NVME_IOCTL_H_
