/**
 * @file	tcg_device.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_TCG_TCG_DEVICE_H_
#define JCU_DPARM_TCG_TCG_DEVICE_H_

#include <string>
#include <memory>

#include <jcu-dparm/err.h>
#include <jcu-dparm/tcg/tcg_types.h>

namespace jcu {
namespace dparm {

class DriveHandle;

namespace tcg {

enum TcgDeviceType {
  kUnknownDeviceType = 0,
  kGenericDevice,
  kOpalV1Device,
  kOpalV2Device,
  kOpalEnterpriseDevice
};

class TcgCommand;
class TcgResponse;
class TcgSession;

class TcgDevice {
 public:
  virtual ~TcgDevice() {}

  virtual DriveHandle* getDriveHandle() const = 0;

  virtual TcgDeviceType getDeviceType() const = 0;
  virtual bool isAnySSC() const = 0;

  /*
   * tcg locking
   */
  virtual bool isLockingSupported() const = 0;
  virtual bool isLockingEnabled() const = 0;
  virtual bool isLocked() const = 0;
  virtual bool isMBREnabled() const = 0;
  virtual bool isMBRDone() const = 0;
  virtual bool isMediaEncryption() const = 0;

  virtual uint16_t getBaseComId() const = 0;
  virtual uint16_t getNumComIds() const = 0;

  virtual std::unique_ptr<TcgSession> createSession() = 0;
  virtual std::unique_ptr<TcgCommand> createCommand() = 0;
  virtual std::unique_ptr<TcgResponse> createResponse() = 0;

  /**
   * execute low-level TCG Command
   * @param cmd      command
   * @param resp     response
   * @param protocol protocol
   * @return result
   */
  virtual DparmResult exec(const TcgCommand& cmd, TcgResponse& resp, uint8_t protocol = 0x01) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  /**
   * get table
   *
   * @param response
   * @param table
   * @param start_col refer enum OpalToken
   * @param end_col   refer enum OpalToken
   * @return result
   */
  virtual DparmReturn<OpalStatusCode> opalGetTable(TcgSession& session, TcgResponse& response, const std::vector<uint8_t>& table, uint16_t start_col, uint16_t end_col) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  /**
   * get table
   *
   * @param response
   * @param table
   * @param start_col column name
   * @param end_col   column name
   * @return result
   */
  virtual DparmReturn<OpalStatusCode> enterpriseGetTable(TcgSession& session, TcgResponse& response, const std::vector<uint8_t>& table, const char* start_col, const char* end_col) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  /**
   * get default password (MSID)
   *
   * @return MSID
   */
  virtual DparmReturn<OpalStatusCode> getDefaultPassword(std::string* out_password) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }

  /**
   * revertTPer
   *
   * @param password    password of authority (SID or PSID)
   * @param is_psid     password is PSID
   * @param is_admin_sp use admin sp uid
   * @return
   */
  virtual DparmReturn<OpalStatusCode> revertTPer(const std::string& password, uint8_t is_psid = 0, uint8_t is_admin_sp = 0) {
    return { DPARME_NOT_SUPPORTED, 0 };
  }
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_TCG_TCG_DEVICE_H_
