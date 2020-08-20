//
// Created by User on 2020-08-18.
//

#include <assert.h>
#include <chrono>
#include <thread>

#include <jcu-dparm/tcg/tcg_command.h>
#include <jcu-dparm/tcg/tcg_response.h>

#include "tcg_device_general.h"
#include "../drive_handle_base.h"

#include "tcg_session_impl.h"
#include "tcg_command_impl.h"
#include "tcg_response_impl.h"

namespace jcu {
namespace dparm {
namespace tcg {

TcgDeviceGeneric::TcgDeviceGeneric(DriveHandleBase *drive_handle)
    : drive_handle_(drive_handle) {
}

DriveHandle* TcgDeviceGeneric::getDriveHandle() const {
  return drive_handle_;
}

TcgDeviceType TcgDeviceGeneric::getDeviceType() const {
  return kGenericDevice;
}

bool TcgDeviceGeneric::isAnySSC() const {
  return false;
}

bool TcgDeviceGeneric::isLockingSupported() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  return drive_info.tcg_locking;
}

bool TcgDeviceGeneric::isLockingEnabled() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  if (!drive_info.tcg_locking) return false;
  auto feat_it = drive_info.tcg_raw_features.find(kFcLocking);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_locking_feature_t *)feat_it->second.data();
  return feature->locking_enabled ? true : false;
}

bool TcgDeviceGeneric::isLocked() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  if (!drive_info.tcg_locking) return false;
  auto feat_it = drive_info.tcg_raw_features.find(kFcLocking);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_locking_feature_t *)feat_it->second.data();
  return feature->locked ? true : false;
}

bool TcgDeviceGeneric::isMBREnabled() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  if (!drive_info.tcg_locking) return false;
  auto feat_it = drive_info.tcg_raw_features.find(kFcLocking);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_locking_feature_t *)feat_it->second.data();
  return feature->mbr_enabled ? true : false;
}

bool TcgDeviceGeneric::isMBRDone() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  if (!drive_info.tcg_locking) return false;
  auto feat_it = drive_info.tcg_raw_features.find(kFcLocking);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_locking_feature_t *)feat_it->second.data();
  return feature->mbr_done ? true : false;
}

bool TcgDeviceGeneric::isMediaEncryption() const {
  const DriveInfo& drive_info = drive_handle_->getDriveInfo();
  if (!drive_info.tcg_locking) return false;
  auto feat_it = drive_info.tcg_raw_features.find(kFcLocking);
  assert(feat_it != drive_info.tcg_raw_features.cend());
  const auto *feature = (const tcg::discovery0_locking_feature_t *)feat_it->second.data();
  return feature->media_encryption ? true : false;
}

uint16_t TcgDeviceGeneric::getBaseComId() const {
  return 0;
}

uint16_t TcgDeviceGeneric::getNumComIds() const {
  return 0;
}

std::unique_ptr<TcgSession> TcgDeviceGeneric::createSession() {
  return std::unique_ptr<TcgSession>(new TcgSessionImpl(this));
}

std::unique_ptr<TcgCommand> TcgDeviceGeneric::createCommand() {
  return std::unique_ptr<TcgCommand>(new TcgCommandImpl());
}

std::unique_ptr<TcgResponse> TcgDeviceGeneric::createResponse() {
  return std::unique_ptr<TcgResponse>(new TcgResponseImpl());
}

DparmResult TcgDeviceGeneric::exec(const TcgCommand &cmd, TcgResponse &resp, uint8_t protocol) {
  const int timeout_ms = 10000;
  const auto timeout_duration = std::chrono::milliseconds { timeout_ms };
  auto begin_at = std::chrono::steady_clock::now();

  if (!this->isAnySSC()) {
    return {DPARME_NOT_SUPPORTED, 0 };
  }

  auto dres = drive_handle_->doSecurityCommand(1, 0, protocol, getBaseComId(), (void*)cmd.getCmdBuf(), cmd.getCmdSize());
  if (!dres.isOk()) {
    return { dres.code, dres.sys_error, dres.drive_status };
  }

  opal_header_t *resp_header = (opal_header_t *)resp.getRespBuf();
  do {
    auto spent_time = std::chrono::steady_clock::now() -  begin_at;
    std::this_thread::sleep_for(std::chrono::milliseconds { 25 });
    resp.reset();
    dres = drive_handle_->doSecurityCommand(0, 0, protocol, getBaseComId(), (void*)resp_header, resp.getRespBufSize());
    if (!dres.isOk()) {
      return dres;
    }
    if (spent_time > timeout_duration) {
      // TIMEOUT!
      break;
    }
  } while (resp_header->cp.outstanding_data && (!resp_header->cp.min_transfer));

  if (resp_header->cp.outstanding_data && (!resp_header->cp.min_transfer)) {
    return { DPARME_OPERATION_TIMEOUT, 0, dres.drive_status };
  }

  auto commit_result = resp.commit();
  if (!commit_result.isOk()) {
    return commit_result;
  }

  return dres;
}

DparmReturn<OpalStatusCode> TcgDeviceGeneric::revertTPer(const std::string &password, uint8_t is_psid, uint8_t is_admin_sp) {
  return { DPARME_NOT_SUPPORTED, 0 };
}

} // namespace tcg
} // namespace dparm
} // namespace jcu
