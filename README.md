# jcu-dparm

ATA/NVMe and TCG protocol



# Supported platforms

## Windows

* ATA passthrough command
* TCG protocol support
  - Support on ATA, SCSI, Windows NVMe driver
  - nvmewin driver is not tested

* NVMe passthrough command is not supported yet

## Linux

* All feature support (sg, nvme driver)



# example
```cpp
#include <jcu-dparm/drive_handle.h>
#include <jcu-dparm/drive_factory.h>

using namespace jcu::dparm;

int main() {
  auto drive_factory = DriveFactory::getSystemFactory();
  int rc;

  // retrieve disk drives
  std::list<DriveInfo> drive_list;
  rc = drive_factory->enumDrives(drive_list);
  printf("enum driver result = %d, size=%d\n", rc, drive_list.size());


  // open disk drive
  auto drive_handle = drive_factory->open("\\\\.\\PhysicalDrive1"); // Windows
//  auto drive_handle = drive_factory->open("/dev/sdb"); // Linux
  if (!drive_handle->isOpen()) {
    printf("OPEN FAILED: code=%d, sys=%d\n", drive_handle->getError().code, drive_handle->getError().sys_error);
    return 1;
  }

  return 0;
}
```
