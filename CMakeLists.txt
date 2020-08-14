cmake_minimum_required(VERSION 3.8)
project(jcu_dparm)

include(FetchContent)

set(CMAKE_CXX_STANDARD 11)

# Project Options
option(jcu_dparm_BUILD_TESTS "Build tests" ON)

# Main Library
set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INC_DIR ${INC_ROOT}/jcu-dparm)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(INC_FILES
        ${INC_DIR}/drive_handle.h
        ${INC_DIR}/drive_factory.h
        ${INC_DIR}/types.h
        ${INC_DIR}/err.h
        ${INC_DIR}/ata_types.h
        ${INC_DIR}/ata_utils.h
        ${INC_DIR}/nvme_types.h
        ${INC_DIR}/nvme_utils.h
        )

set(SRC_FILES
        ${SRC_DIR}/ata_utils.cc
        ${SRC_DIR}/nvme_utils.cc
        ${SRC_DIR}/drive_driver_handle.h
        ${SRC_DIR}/drive_handle_base.h
        ${SRC_DIR}/drive_handle_base.cc
        ${SRC_DIR}/drive_handle_sanitize.cc
        ${SRC_DIR}/intl_utils.h
        ${SRC_DIR}/intl_utils.cc
        ${SRC_DIR}/tcg/tcg_device.h
        ${SRC_DIR}/tcg/tcg_device.cc
        ${SRC_DIR}/tcg/tcg_command.h
        ${SRC_DIR}/tcg/tcg_command.cc
        ${SRC_DIR}/tcg/tcg_session.h
        ${SRC_DIR}/tcg/tcg_session.cc
        ${SRC_DIR}/tcg/tcg_device_enterprise.h
        ${SRC_DIR}/tcg/tcg_device_enterprise.cc
        ${SRC_DIR}/tcg/tcg_device_opal_base.h
        ${SRC_DIR}/tcg/tcg_device_opal_base.cc
        ${SRC_DIR}/tcg/tcg_device_opal_1.h
        ${SRC_DIR}/tcg/tcg_device_opal_1.cc
        ${SRC_DIR}/tcg/tcg_device_opal_2.h
        ${SRC_DIR}/tcg/tcg_device_opal_2.cc
        )

if(MSVC AND WIN32)
    set(PLATFORM_SRC_FILES
            ${SRC_DIR}/plat-win/drive_factory.cc
            ${SRC_DIR}/plat-win/driver_base.h
            ${SRC_DIR}/plat-win/driver_base.cc
            ${SRC_DIR}/plat-win/physical_drive_finder.cc
            ${SRC_DIR}/plat-win/physical_drive_finder.h
            ${SRC_DIR}/plat-win/drivers/driver_utils.h
            ${SRC_DIR}/plat-win/drivers/driver_utils.cc
            ${SRC_DIR}/plat-win/drivers/nvme_win_driver.h
            ${SRC_DIR}/plat-win/drivers/nvme_win_driver.cc
            ${SRC_DIR}/plat-win/drivers/samsung_nvme_driver.h
            ${SRC_DIR}/plat-win/drivers/samsung_nvme_driver.cc
            ${SRC_DIR}/plat-win/drivers/scsi_driver.h
            ${SRC_DIR}/plat-win/drivers/scsi_driver.cc
            ${SRC_DIR}/plat-win/drivers/ata_driver.h
            ${SRC_DIR}/plat-win/drivers/ata_driver.cc
            ${SRC_DIR}/plat-win/drivers/windows_nvme_driver.h
            ${SRC_DIR}/plat-win/drivers/windows_nvme_driver.cc
            ${SRC_DIR}/plat-win/drivers/windows_nvme_ioctl.h
            )
else()
    set(PLATFORM_SRC_FILES
            ${SRC_DIR}/plat-linux/drive_factory.cc
            ${SRC_DIR}/plat-linux/sgio.cc
            ${SRC_DIR}/plat-linux/sgio.h
            ${SRC_DIR}/plat-linux/sysfs_utils.cc
            ${SRC_DIR}/plat-linux/sysfs_utils.h
            ${SRC_DIR}/plat-linux/apt.cc
            ${SRC_DIR}/plat-linux/driver_base.cc
            ${SRC_DIR}/plat-linux/driver_base.h
            ${SRC_DIR}/plat-linux/drivers/driver_utils.h
            ${SRC_DIR}/plat-linux/drivers/driver_utils.cc
            ${SRC_DIR}/plat-linux/drivers/sg_driver.cc
            ${SRC_DIR}/plat-linux/drivers/sg_driver.h
            ${SRC_DIR}/plat-linux/drivers/nvme_driver.cc
            ${SRC_DIR}/plat-linux/drivers/nvme_driver.h
            ${SRC_DIR}/plat-linux/drivers/nvme_ioctl.h
            )
endif(MSVC AND WIN32)

add_library(${PROJECT_NAME} ${INC_FILES} ${SRC_FILES} ${PLATFORM_SRC_FILES})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${INC_ROOT}
        )

# Test
#
#if (jcu_dparm_BUILD_TESTS)
#    set(gtest_force_shared_crt ON CACHE BOOL "")
#
#    if (NOT TARGET gtest)
#        FetchContent_Declare(
#                googletest
#                GIT_REPOSITORY https://github.com/google/googletest.git
#                GIT_TAG release-1.10.0
#        )
#
#        FetchContent_GetProperties(googletest)
#
#        if (NOT googletest_POPULATED)
#            FetchContent_Populate(googletest)
#            add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
#        endif ()
#    endif ()
#
#    enable_testing()
##    add_subdirectory(test)
#endif ()
