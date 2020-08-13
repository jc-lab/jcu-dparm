#include <jcu-dparm/drive_handle.h>
#include <jcu-dparm/drive_factory.h>

using namespace jcu::dparm;

int main() {
  auto drive_factory = DriveFactory::getSystemFactory();
  int rc;

  // 디스크 목록 얻기
  std::list<DriveInfo> drive_list;
  rc = drive_factory->enumDrives(drive_list);
  printf("enum driver result = %d, size=%d\n", rc, drive_list.size());


  // 디스크 하나 열기
  auto drive_handle = drive_factory->open("\\\\.\\PhysicalDrive1"); // Windows
//  auto drive_handle = drive_factory->open("/dev/sdb"); // Linux
  if (!drive_handle->isOpen()) {
    printf("OPEN FAILED: code=%d, sys=%d\n", drive_handle->getError().code, drive_handle->getError().sys_error);
    return 1;
  }

  return 0;
}
