/**
 * @file	tcg_device_enterprise.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/27
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_SRC_TCG_TCG_DEVICE_ENTERPRISE_H_
#define JCU_DPARM_SRC_TCG_TCG_DEVICE_ENTERPRISE_H_

#include <string>
#include <vector>

#include "tcg_device_general.h"

namespace jcu {
namespace dparm {
namespace tcg {

class TcgDeviceEnterprise : public TcgDeviceGeneric {
 public:
  TcgDeviceEnterprise(DriveHandleBase *drive_handle);
  TcgDeviceType getDeviceType() const override;
  uint16_t getBaseComId() const override;
  uint16_t getNumComIds() const override;

  /**
   * get table
   *
   * @param response
   * @param table
   * @param start_col OpalToken
   * @param end_col   OpalToken
   * @return result
   */
  DparmReturn<OpalStatusCode> enterpriseGetTable(TcgSession& session, TcgResponse& response, const std::vector<uint8_t>& table, const char* start_col, const char* end_col) override;

  DparmReturn<OpalStatusCode> getDefaultPassword(std::string* out_password) override;
  DparmReturn<OpalStatusCode> revertTPer(const std::string &password, uint8_t is_psid, uint8_t is_admin_sp) override;
};

} // namespace tcg
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_SRC_TCG_TCG_DEVICE_ENTERPRISE_H_
