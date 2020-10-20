#include <gtest/gtest.h>

#include <jcu-dparm/ata_types.h>
#include <jcu-dparm/nvme_types.h>

using namespace jcu::dparm;

namespace {

using namespace ata;

class AtaTypesTest : public ::testing::Test {};

TEST(AtaTypesTest, struct_ata_smart_data) {
  ata_smart_attribute_values_t * p = (ata_smart_attribute_values_t *)0;

  EXPECT_EQ((int)&(p->rev_number), 0);
  EXPECT_EQ((int)&(p->attributes), 2);
  EXPECT_EQ((int)&(p->offline_data_collection_status), 362);
  EXPECT_EQ((int)&(p->total_time_to_complete_offline_data_collection_activity), 364);
  EXPECT_EQ((int)&(p->vendor_specific_366), 366);
  EXPECT_EQ((int)&(p->offline_data_collection_capability), 367);
  EXPECT_EQ((int)&(p->smart_capability), 368);
  EXPECT_EQ((int)&(p->vendor_specific_371), 371);
  EXPECT_EQ((int)&(p->short_selftest_routine_recommanded_polling_time), 372);
  EXPECT_EQ((int)&(p->extended_selftest_routine_recommanded_polling_time_a), 373);
  EXPECT_EQ((int)&(p->conveyance_selftest_routine_recommanded_polling_time), 374);
  EXPECT_EQ((int)&(p->extended_selftest_routine_recommanded_polling_time_b), 375);
  EXPECT_EQ((int)&(p->vendor_specific_386), 386);
  EXPECT_EQ((int)&(p->checksum), 511);

  EXPECT_EQ(sizeof(*p), 512);
}

} // namespace

namespace {

using namespace nvme;

class NvmeTypesTest : public ::testing::Test {};

TEST(NvmeTypesTest, struct_nvme_smart_log_page) {
  nvme_smart_log_page_t* p = (nvme_smart_log_page_t*)0;

  EXPECT_EQ((int)&(p->critical_warning), 0);
  EXPECT_EQ((int)&(p->composite_temperature), 1);
  EXPECT_EQ((int)&(p->available_spare), 3);
  EXPECT_EQ((int)&(p->available_spare_threshold), 4);
  EXPECT_EQ((int)&(p->percentage_used), 5);
  EXPECT_EQ((int)&(p->rev01), 6);
  EXPECT_EQ((int)&(p->data_units_read), 32);
  EXPECT_EQ((int)&(p->data_units_written), 48);
  EXPECT_EQ((int)&(p->host_read_commands), 64);
  EXPECT_EQ((int)&(p->host_write_commands), 80);
  EXPECT_EQ((int)&(p->controller_busy_time), 96);
  EXPECT_EQ((int)&(p->power_cycles), 112);
  EXPECT_EQ((int)&(p->power_on_hours), 128);
  EXPECT_EQ((int)&(p->unsafe_shutdowns), 144);
  EXPECT_EQ((int)&(p->media_and_data_integrity_errors), 160);
  EXPECT_EQ((int)&(p->number_of_error_information_log_entries), 176);
  EXPECT_EQ((int)&(p->warning_composite_temperature_time), 192);
  EXPECT_EQ((int)&(p->critical_composite_temperature_time), 196);
  EXPECT_EQ((int)&(p->temperature_sensor), 200);
  EXPECT_EQ((int)&(p->thermal_management_temperature_transition_count), 216);
  EXPECT_EQ((int)&(p->total_time_for_thermal_management_temperature), 224);
  EXPECT_EQ((int)&(p->rev_remaining), 232);

  EXPECT_EQ(sizeof(*p), 512);
}

} // namespace