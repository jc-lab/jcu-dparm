/**
 * @file	ata_types.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2020/07/22
 * @copyright Copyright (C) 2020 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef JCU_DPARM_ATA_TYPES_H_
#define JCU_DPARM_ATA_TYPES_H_

#include <stdint.h>

namespace jcu {
namespace dparm {

namespace ata {

enum {
  ATA_OP_DSM			= 0x06, // Data Set Management (TRIM)
  ATA_OP_READ_PIO			= 0x20,
  ATA_OP_READ_PIO_ONCE		= 0x21,
  ATA_OP_READ_LONG		= 0x22,
  ATA_OP_READ_LONG_ONCE		= 0x23,
  ATA_OP_READ_PIO_EXT		= 0x24,
  ATA_OP_READ_DMA_EXT		= 0x25,
  ATA_OP_READ_LOG_EXT		= 0x2f,
  ATA_OP_READ_FPDMA		= 0x60,	// NCQ
  ATA_OP_WRITE_PIO		= 0x30,
  ATA_OP_WRITE_LONG		= 0x32,
  ATA_OP_WRITE_LONG_ONCE		= 0x33,
  ATA_OP_WRITE_PIO_EXT		= 0x34,
  ATA_OP_WRITE_DMA_EXT		= 0x35,
  ATA_OP_WRITE_FPDMA		= 0x61,	// NCQ
  ATA_OP_READ_VERIFY		= 0x40,
  ATA_OP_READ_VERIFY_ONCE		= 0x41,
  ATA_OP_READ_VERIFY_EXT		= 0x42,
  ATA_OP_WRITE_UNC_EXT		= 0x45,	// lba48, no data, uses feat reg
  ATA_OP_FORMAT_TRACK		= 0x50,
  ATA_OP_TRUSTED_RECV = 0x5c,
  ATA_OP_TRUSTED_RECV_DMA = 0x5d,
  ATA_OP_TRUSTED_SEND = 0x5e,
  ATA_OP_TRUSTED_SEND_DMA = 0x5f,
  ATA_OP_DOWNLOAD_MICROCODE	= 0x92,
  ATA_OP_STANDBYNOW2		= 0x94,
  ATA_OP_CHECKPOWERMODE2		= 0x98,
  ATA_OP_SLEEPNOW2		= 0x99,
  ATA_OP_PIDENTIFY		= 0xa1,
  ATA_OP_READ_NATIVE_MAX		= 0xf8,
  ATA_OP_READ_NATIVE_MAX_EXT	= 0x27,
  ATA_OP_GET_NATIVE_MAX_EXT	= 0x78,
  ATA_OP_SMART			= 0xb0,
  ATA_OP_DCO			= 0xb1,
  ATA_OP_SET_SECTOR_CONFIGURATION = 0xb2,
  ATA_OP_SANITIZE			= 0xb4,
  ATA_OP_ERASE_SECTORS		= 0xc0,
  ATA_OP_READ_DMA			= 0xc8,
  ATA_OP_WRITE_DMA		= 0xca,
  ATA_OP_DOORLOCK			= 0xde,
  ATA_OP_DOORUNLOCK		= 0xdf,
  ATA_OP_STANDBYNOW1		= 0xe0,
  ATA_OP_IDLEIMMEDIATE		= 0xe1,
  ATA_OP_SETIDLE			= 0xe3,
  ATA_OP_SET_MAX			= 0xf9,
  ATA_OP_SET_MAX_EXT		= 0x37,
  ATA_OP_SET_MULTIPLE		= 0xc6,
  ATA_OP_CHECKPOWERMODE1		= 0xe5,
  ATA_OP_SLEEPNOW1		= 0xe6,
  ATA_OP_FLUSHCACHE		= 0xe7,
  ATA_OP_FLUSHCACHE_EXT		= 0xea,
  ATA_OP_IDENTIFY			= 0xec,
  ATA_OP_SETFEATURES		= 0xef,
  ATA_OP_SECURITY_SET_PASS	= 0xf1,
  ATA_OP_SECURITY_UNLOCK		= 0xf2,
  ATA_OP_SECURITY_ERASE_PREPARE	= 0xf3,
  ATA_OP_SECURITY_ERASE_UNIT	= 0xf4,
  ATA_OP_SECURITY_FREEZE_LOCK	= 0xf5,
  ATA_OP_SECURITY_DISABLE		= 0xf6,
  ATA_OP_VENDOR_SPECIFIC_0x80	= 0x80,
};

/*
 * Some useful ATA register bits
 */
enum {
  ATA_USING_LBA		= (1 << 6),
  ATA_STAT_DRQ		= (1 << 3),
  ATA_STAT_ERR		= (1 << 0),
};

/*
 * Useful parameters for init_hdio_taskfile():
 */
enum {
  RW_READ			= 0,
  RW_WRITE		= 1,
  LBA28_OK		= 0,
  LBA48_FORCE		= 1,
};

#pragma pack(push, 1)

/*
 * Definitions and structures for use with SG_IO + ATA_16:
 */
struct ata_lba_regs {
  uint8_t feat;
  uint8_t nsect; // 16bit able
  uint8_t lbal; // 16bit able
  uint8_t lbam; // 16bit able
  uint8_t lbah; // 16bit able
};

struct ata_tf {
  uint8_t dev;
  uint8_t command;
  uint8_t error;
  uint8_t status;
  uint8_t is_lba48;
  struct ata_lba_regs lob;
  struct ata_lba_regs hob;
};

typedef struct ata_tf ata_tf_t;

struct ata_identify_device_data {

  struct {
    uint16_t reserved1 : 1;
    uint16_t retired3 : 1;
    uint16_t response_incomplete : 1;
    uint16_t retired2 : 3;
    uint16_t fixed_device : 1;   // obsolete
    uint16_t removable_media : 1;// obsolete
    uint16_t retired1 : 7;
    uint16_t device_type : 1;
  } general_configuration;// word 0

  uint16_t num_cylinders;         // word 1, obsolete
  uint16_t specific_configuration;// word 2
  uint16_t num_heads;             // word 3, obsolete
  uint16_t retired1[2];
  uint16_t num_sectors_per_track;// word 6, obsolete
  uint16_t vendor_unique1[3];
  uint8_t serial_number[20];// word 10-19
  uint16_t retired2[2];
  uint16_t obsolete1;
  uint8_t firmware_revision[8]; // word 23-26
  uint8_t model_number[40];     // word 27-46
  uint8_t maximum_block_transfer;// word 47. 01h-10h = Maximum number of sectors that shall be transferred per interrupt on READ/WRITE MULTIPLE commands
  uint8_t vendor_unique2;

  struct {
    uint16_t feature_supported : 1;
    uint16_t reserved : 15;
  } trusted_computing;// word 48

  struct {
    uint8_t current_long_physical_sector_alignment : 2;
    uint8_t reserved_byte49 : 6;

    uint8_t dma_supported : 1;
    uint8_t lba_supported : 1;// Shall be set to one to indicate that LBA is supported.
    uint8_t iordy_disable : 1;
    uint8_t iordy_supported : 1;
    uint8_t reserved1 : 1;// Reserved for the IDENTIFY PACKET DEVICE command
    uint8_t standyby_timer_support : 1;
    uint8_t reserved2 : 2;// Reserved for the IDENTIFY PACKET DEVICE command

    uint16_t reserved_word50;
  } capabilities;// word 49-50

  uint16_t obsolete_words51[2];

  uint16_t translation_fields_valid : 3;// word 53, bit 0 - Obsolete; bit 1 - words 70:64 valid; bit 2; word 88 valid
  uint16_t reserved3 : 5;
  uint16_t free_fall_control_sensitivity : 8;

  uint16_t number_of_current_cylinders;// word 54, obsolete
  uint16_t number_of_current_heads;    // word 55, obsolete
  uint16_t current_sectors_per_track;  // word 56, obsolete
  uint32_t current_sector_capacity;      // word 57, word 58, obsolete

  uint8_t current_multi_sector_setting;// word 59
  uint8_t multi_sector_setting_valid : 1;
  uint8_t reserved_byte59 : 3;
  uint8_t sanitize_feature_supported : 1;
  uint8_t crypto_scramble_ext_command_supported : 1;
  uint8_t overwrite_ext_command_supported : 1;
  uint8_t block_erase_ext_command_supported : 1;

  uint32_t user_addressable_sectors;// word 60-61, for 28-bit commands

  uint16_t obsolete_word62;

  uint16_t multi_word_dma_support : 8;// word 63
  uint16_t multi_word_dma_active : 8;

  uint16_t advanced_pioModes : 8;// word 64. bit 0:1 - PIO mode supported
  uint16_t reserved_byte64 : 8;

  uint16_t minimum_mwxfer_cycle_time;    // word 65
  uint16_t recommended_mwxfer_cycle_time;// word 66
  uint16_t minimum_pio_cycle_time;       // word 67
  uint16_t minimum_pio_cycle_time_iordy;  // word 68

  struct {
    uint16_t reserved : 2;
    uint16_t non_volatile_write_cache : 1;// All write cache is non-volatile
    uint16_t extended_user_addressable_sectors_supported : 1;
    uint16_t device_encrypts_all_user_data : 1;
    uint16_t read_zero_after_trim_supported : 1;
    uint16_t optional_28bit_commands_supported : 1;
    uint16_t ieee1667 : 1;// Reserved for IEEE 1667
    uint16_t download_microcode_dma_supported : 1;
    uint16_t set_max_set_password_unlock_dma_supported : 1;
    uint16_t write_buffer_dma_supported : 1;
    uint16_t read_buffer_dma_supported : 1;
    uint16_t device_config_identify_set_dma_supported : 1;// obsolete
    uint16_t lpsaerc_supported : 1;                   // Long Physical Sector Alignment Error Reporting Control is supported.
    uint16_t deterministic_read_after_trim_supported : 1;
    uint16_t cfast_spec_supported : 1;
  } additional_supported;// word 69

  uint16_t reserved_words70[5];// word 70 - reserved
  // word 71:74 - Reserved for the IDENTIFY PACKET DEVICE command

  //Word 75
  uint16_t queue_depth : 5;//  Maximum queue depth - 1
  uint16_t reserved_word75 : 11;

  struct {
    // Word 76
    uint16_t reserved0 : 1;// shall be set to 0
    uint16_t sata_gen1 : 1; // Supports SATA Gen1 Signaling Speed (1.5Gb/s)
    uint16_t sata_gen2 : 1; // Supports SATA Gen2 Signaling Speed (3.0Gb/s)
    uint16_t sata_gen3 : 1; // Supports SATA Gen3 Signaling Speed (6.0Gb/s)

    uint16_t reserved1 : 4;

    uint16_t ncq : 1;      // Supports the NCQ feature set
    uint16_t hipm : 1;     // Supports HIPM
    uint16_t phy_events : 1;// Supports the SATA Phy Event Counters log
    uint16_t ncq_unload : 1;// Supports Unload while NCQ commands are outstanding

    uint16_t ncq_priority : 1; // Supports NCQ priority information
    uint16_t host_auto_ps : 1;  // Supports Host Automatic Partial to Slumber transitions
    uint16_t device_auto_ps : 1;// Supports Device Automatic Partial to Slumber transitions
    uint16_t read_log_dma : 1;  // Supports READ LOG DMA EXT as equivalent to READ LOG EXT

    // Word 77
    uint16_t reserved2 : 1;   // shall be set to 0
    uint16_t current_speed : 3;// Coded value indicating current negotiated Serial ATA signal speed

    uint16_t ncq_streaming : 1;  // Supports NCQ Streaming
    uint16_t ncq_queue_mgmt : 1;  // Supports NCQ Queue Management Command
    uint16_t ncq_receive_send : 1;// Supports RECEIVE FPDMA QUEUED and SEND FPDMA QUEUED commands
    uint16_t devslp_to_reduced_pwr_state : 1;

    uint16_t reserved3 : 8;
  } serial_ata_capabilities;

  // Word 78
  struct {
    uint16_t reserved0 : 1;           //shall be set to 0
    uint16_t non_zero_offsets : 1;      // Device supports non-zero buffer offsets in DMA Setup FIS
    uint16_t dma_setup_auto_activate : 1;// Device supports DMA Setup auto-activation
    uint16_t dipm : 1;                // Device supports DIPM

    uint16_t in_order_data : 1;                 // Device supports in-order data delivery
    uint16_t hardware_feature_control : 1;      // Hardware Feature Control is supported
    uint16_t software_settings_preservation : 1;// Device supports Software Settings Preservation
    uint16_t ncq_autosense : 1;                // Supports NCQ Autosense

    uint16_t DEVSLP : 1;           // Device supports link power state - device sleep
    uint16_t hybrid_information : 1;// Device supports Hybrid Information Feature (If the device does not support NCQ (word 76 bit 8 is 0), then this bit shall be cleared to 0.)

    uint16_t reserved1 : 6;
  } serial_ata_features_supported;

  // Word 79
  struct {
    uint16_t reserved0 : 1;           // shall be set to 0
    uint16_t non_zero_offsets : 1;      // Non-zero buffer offsets in DMA Setup FIS enabled
    uint16_t dma_setup_auto_activate : 1;// DMA Setup auto-activation optimization enabled
    uint16_t dipm : 1;                // DIPM enabled

    uint16_t in_order_data : 1;                 // In-order data delivery enabled
    uint16_t hardware_feature_control : 1;      // Hardware Feature Control is enabled
    uint16_t software_settings_preservation : 1;// Software Settings Preservation enabled
    uint16_t device_auto_pS : 1;                // Device Automatic Partial to Slumber transitions enabled

    uint16_t devslp : 1;           // link power state - device sleep is enabled
    uint16_t hybrid_information : 1;// Hybrid Information Feature is enabled

    uint16_t reserved1 : 6;
  } serial_ata_features_enabled;

  uint16_t major_revision;// word 80. bit 5 - supports ATA5; bit 6 - supports ATA6; bit 7 - supports ATA7; bit 8 - supports ATA8-ACS; bit 9 - supports ACS-2;
  uint16_t minor_revision;// word 81. T13 minior version number

  struct {

    //
    // Word 82
    //
    uint16_t smart_commands : 1;        // The SMART feature set is supported
    uint16_t security_mode : 1;         // The Security feature set is supported
    uint16_t removable_media_feature : 1;// obsolete
    uint16_t power_management : 1;      // shall be set to 1
    uint16_t reserved1 : 1;            // PACKET feature set, set to 0 indicates not supported for ATA devices (only support for ATAPI devices)
    uint16_t write_cache : 1;           // The volatile write cache is supported
    uint16_t look_ahead : 1;            // Read look-ahead is supported
    uint16_t release_interrupt : 1;     // obsolete
    uint16_t service_interrupt : 1;     // obsolete
    uint16_t device_reset : 1;          // Shall be cleared to zero to indicate that the DEVICE RESET command is not supported
    uint16_t host_protected_area : 1;    // obsolete
    uint16_t obsolete1 : 1;
    uint16_t write_buffer : 1;// The WRITE BUFFER command is supported
    uint16_t read_buffer : 1; // The READ BUFFER command is supported
    uint16_t nop : 1;        // The NOP command is supported
    uint16_t obsolete2 : 1;

    //
    // Word 83
    //
    uint16_t download_microcode : 1;// The DOWNLOAD MICROCODE command is supported
    uint16_t dma_queued : 1;        // obsolete
    uint16_t cfa : 1;              // The CFA feature set is supported
    uint16_t advanced_pm : 1;       // The APM feature set is supported
    uint16_t msn : 1;              // obsolete
    uint16_t power_up_in_standby : 1; // The PUIS feature set is supported
    uint16_t manual_power_up : 1;    // SET FEATURES subcommand is required to spin-up after power-up
    uint16_t reserved2 : 1;
    uint16_t set_max : 1;             // obsolete
    uint16_t acoustics : 1;          // obsolete
    uint16_t big_lba : 1;             // The 48-bit Address feature set is supported
    uint16_t device_config_overlay : 1;// obsolete
    uint16_t flush_cache : 1;         // Shall be set to one to indicate that the mandatory FLUSH CACHE command is supported
    uint16_t flush_cache_ext : 1;      // The FLUSH CACHE EXT command is supported
    uint16_t word_valid83 : 2;        // shall be 01b

    //
    // Word 84
    //
    uint16_t smart_error_log : 1;       // SMART error logging is supported
    uint16_t smart_self_test : 1;       // The SMART self-test is supported
    uint16_t media_serial_number : 1;   // Media serial number is supported
    uint16_t media_card_pass_through : 1;// obsolete
    uint16_t streaming_feature : 1;    // The Streaming feature set is supported
    uint16_t gp_logging : 1;           // The GPL feature set is supported
    uint16_t write_fua : 1;            // The WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands are supported
    uint16_t write_queued_fua : 1;      // obsolete
    uint16_t WWN64Bit : 1;            // The 64-bit World wide name is supported
    uint16_t URGRead_stream : 1;       // obsolete
    uint16_t URGWrite_stream : 1;      // obsolete
    uint16_t reserved_for_tech_report : 2;
    uint16_t idle_with_unload_feature : 1;// The IDLE IMMEDIATE command with UNLOAD feature is supported
    uint16_t word_valid : 2;            // shall be 01b

  } command_set_support;

  struct {

    //
    // Word 85
    //
    uint16_t smart_commands : 1;        // The SMART feature set is enabled
    uint16_t security_mode : 1;         // The Security feature set is enabled
    uint16_t removable_media_feature : 1;// obsolete
    uint16_t power_management : 1;      // Shall be set to one to indicate that the mandatory Power Management feature set is supported
    uint16_t reserved1 : 1;            // Shall be cleared to zero to indicate that the PACKET feature set is not supported
    uint16_t write_cache : 1;           // The volatile write cache is enabled
    uint16_t look_ahead : 1;            // Read look-ahead is enabled
    uint16_t release_interrupt : 1;     // The release interrupt is enabled
    uint16_t service_interrupt : 1;     // The SERVICE interrupt is enabled
    uint16_t device_reset : 1;          // Shall be cleared to zero to indicate that the DEVICE RESET command is not supported
    uint16_t host_protected_area : 1;    // obsolete
    uint16_t obsolete1 : 1;
    uint16_t write_buffer : 1;// The WRITE BUFFER command is supported
    uint16_t read_buffer : 1; // The READ BUFFER command is supported
    uint16_t nop : 1;        // The NOP command is supported
    uint16_t obsolete2 : 1;

    //
    // Word 86
    //
    uint16_t download_microcode : 1;// The DOWNLOAD MICROCODE command is supported
    uint16_t dma_queued : 1;        // obsolete
    uint16_t cfa : 1;              // The CFA feature set is supported
    uint16_t advanced_pm : 1;       // The APM feature set is enabled
    uint16_t msn : 1;              // obsolete
    uint16_t power_up_in_standby : 1; // The PUIS feature set is enabled
    uint16_t manual_power_up : 1;    // SET FEATURES subcommand is required to spin-up after power-up
    uint16_t reserved2 : 1;
    uint16_t set_max : 1;             // obsolete
    uint16_t acoustics : 1;          // obsolete
    uint16_t big_lba : 1;             // The 48-bit Address features set is supported
    uint16_t device_config_overlay : 1;// obsolete
    uint16_t flush_cache : 1;         // FLUSH CACHE command supported
    uint16_t flush_cache_ext : 1;      // FLUSH CACHE EXT command supported
    uint16_t resrved3 : 1;
    uint16_t words119_120Valid : 1;// Words 119..120 are valid

    //
    // Word 87
    //
    uint16_t smart_error_log : 1;       // SMART error logging is supported
    uint16_t smart_self_test : 1;       // SMART self-test supported
    uint16_t media_serial_number : 1;   // Media serial number is valid
    uint16_t media_card_pass_through : 1;// obsolete
    uint16_t streaming_feature : 1;    // obsolete
    uint16_t gp_logging : 1;           // The GPL feature set is supported
    uint16_t write_fua : 1;            // The WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT commands are supported
    uint16_t write_queued_fua : 1;      // obsolete
    uint16_t WWN64Bit : 1;            // The 64-bit World wide name is supported
    uint16_t URGRead_stream : 1;       // obsolete
    uint16_t URGWrite_stream : 1;      // obsolete
    uint16_t reserved_for_tech_report : 2;
    uint16_t idle_with_unload_feature : 1;// The IDLE IMMEDIATE command with UNLOAD FEATURE is supported
    uint16_t reserved4 : 2;            // bit 14 shall be set to 1; bit 15 shall be cleared to 0

  } command_set_active;

  uint16_t ultra_dma_support : 8;// word 88. bit 0 - UDMA mode 0 is supported ... bit 6 - UDMA mode 6 and below are supported
  uint16_t ultra_dma_active : 8; // word 88. bit 8 - UDMA mode 0 is selected ... bit 14 - UDMA mode 6 is selected

  struct {// word 89
    uint16_t time_required : 15;
    uint16_t extended_time_reported : 1;
  } normal_security_erase_unit;

  struct {// word 90
    uint16_t time_required : 15;
    uint16_t extended_time_reported : 1;
  } enhanced_security_erase_unit;

  uint16_t current_aPMLevel : 8;// word 91
  uint16_t reserved_word91 : 8;

  uint16_t master_password_id;// word 92. Master Password Identifier

  uint16_t hardware_reset_result;// word 93

  uint16_t current_acoustic_value : 8;// word 94. obsolete
  uint16_t recommended_acoustic_value : 8;

  uint16_t stream_min_request_size;        // word 95
  uint16_t streaming_transfer_time_dma;    // word 96
  uint16_t streaming_access_latency_dma_pio;// word 97
  uint32_t Streaming_perf_granularity;       // word 98, 99

  uint32_t max48bit_lba[2];// word 100-103

  uint16_t streaming_transfer_time;// word 104. Streaming Transfer Time - PIO

  uint16_t dsm_cap;// word 105

  struct {
    uint16_t logical_sectors_per_physical_sector : 4;// n power of 2: logical sectors per physical sector
    uint16_t reserved0 : 8;
    uint16_t logical_sector_longer_than256Words : 1;
    uint16_t multiple_logical_sectors_per_physical_sector : 1;
    uint16_t reserved1 : 2; // bit 14 - shall be set to  1; bit 15 - shall be clear to 0
  } physical_logical_sector_size;// word 106

  uint16_t inter_seek_delay;                //word 107.     Inter-seek delay for ISO 7779 standard acoustic testing
  uint16_t world_wide_name[4];              //words 108-111
  uint16_t reserved_for_world_wide_name128[4];//words 112-115
  uint16_t reserved_for_tlc_technical_report; //word 116
  uint16_t words_per_logical_sector[2];      //words 117-118 Logical sector size (DWord)

  struct {
    uint16_t reserved_for_drq_technical_report : 1;
    uint16_t write_read_verify : 1;        // The Write-Read-Verify feature set is supported
    uint16_t write_uncorrectable_ext : 1;  // The WRITE UNCORRECTABLE EXT command is supported
    uint16_t read_write_log_dma_ext : 1;     // The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported
    uint16_t download_microcode_mode3 : 1; // Download Microcode mode 3 is supported
    uint16_t freefall_control : 1;        // The Free-fall Control feature set is supported
    uint16_t sense_data_reporting : 1;     // Sense Data Reporting feature set is supported
    uint16_t extended_power_conditions : 1;// Extended Power Conditions feature set is supported
    uint16_t reserved0 : 6;
    uint16_t word_valid : 2;// shall be 01b
  } command_set_support_ext;    //word 119

  struct {
    uint16_t reserved_for_drq_technical_report : 1;
    uint16_t write_read_verify : 1;        // The Write-Read-Verify feature set is enabled
    uint16_t write_uncorrectable_ext : 1;  // The WRITE UNCORRECTABLE EXT command is supported
    uint16_t read_write_log_dma_ext : 1;     // The READ LOG DMA EXT and WRITE LOG DMA EXT commands are supported
    uint16_t download_microcode_mode3 : 1; // Download Microcode mode 3 is supported
    uint16_t freefall_control : 1;        // The Free-fall Control feature set is enabled
    uint16_t sense_data_reporting : 1;     // Sense Data Reporting feature set is enabled
    uint16_t extended_power_conditions : 1;// Extended Power Conditions feature set is enabled
    uint16_t reserved0 : 6;
    uint16_t reserved1 : 2;// bit 14 - shall be set to  1; bit 15 - shall be clear to 0
  } command_set_active_ext;     //word 120

  uint16_t reserved_for_expanded_supportand_active[6];

  uint16_t msn_support : 2;//word 127. obsolete
  uint16_t reserved_word127 : 14;

  struct {//word 128
    uint16_t security_supported : 1;
    uint16_t security_enabled : 1;
    uint16_t security_locked : 1;
    uint16_t security_frozen : 1;
    uint16_t security_count_expired : 1;
    uint16_t enhanced_security_erase_supported : 1;
    uint16_t reserved0 : 2;
    uint16_t security_level : 1;// Master Password Capability: 0 = High, 1 = Maximum
    uint16_t reserved1 : 7;
  } security_status;

  uint16_t reserved_word129[31];//word 129...159. Vendor specific

  struct {//word 160
    uint16_t maximum_current_in_mA : 12;
    uint16_t cfa_power_mode1Disabled : 1;
    uint16_t cfa_power_mode1Required : 1;
    uint16_t reserved0 : 1;
    uint16_t word160Supported : 1;
  } cfa_power_mode1;

  uint16_t reserved_for_cfa_word161[7];//Words 161-167

  uint16_t nominal_form_factor : 4;//Word 168
  uint16_t reserved_word168 : 12;

  struct {//Word 169
    uint16_t supports_trim : 1;
    uint16_t reserved0 : 15;
  } data_set_management_feature;

  uint16_t additional_product_iD[4];//Words 170-173

  uint16_t reserved_for_cfa_word174[2];//Words 174-175

  uint16_t current_media_serial_number[30];//Words 176-205

  struct {                                       //Word 206
    uint16_t supported : 1;                    // The SCT Command Transport is supported
    uint16_t reserved0 : 1;                    // obsolete
    uint16_t write_same_suported : 1;            // The SCT Write Same command is supported
    uint16_t error_recovery_control_supported : 1;// The SCT Error Recovery Control command is supported
    uint16_t feature_control_suported : 1;       // The SCT Feature Control command is supported
    uint16_t data_tables_suported : 1;           // The SCT Data Tables command is supported
    uint16_t reserved1 : 6;
    uint16_t vendor_specific : 4;
  } sCTCommand_transport;

  uint16_t reserved_word207[2];//Words 207-208

  struct {//Word 209
    uint16_t alignment_of_logical_within_physical : 14;
    uint16_t word209Supported : 1;// shall be set to 1
    uint16_t reserved0 : 1;       // shall be cleared to 0
  } block_alignment;

  uint16_t write_read_verify_sector_count_mode3Only[2];//Words 210-211
  uint16_t write_read_verify_sector_count_mode2Only[2];//Words 212-213

  struct {
    uint16_t nv_cache_power_mode_enabled : 1;
    uint16_t reserved0 : 3;
    uint16_t nv_cache_feature_set_enabled : 1;
    uint16_t reserved1 : 3;
    uint16_t nv_cache_power_mode_version : 4;
    uint16_t nv_cache_feature_set_version : 4;
  } nv_cache_capabilities;  //Word 214. obsolete
  uint16_t nv_cache_size_lSW;//Word 215. obsolete
  uint16_t nv_cache_size_mSW;//Word 216. obsolete

  uint16_t nominal_media_rotation_rate;//Word 217; value 0001h means non-rotating media.

  uint16_t reserved_word218;//Word 218

  struct {
    uint8_t nv_cache_estimated_time_to_spin_up_in_seconds;
    uint8_t reserved;
  } nv_cache_options;//Word 219. obsolete

  uint16_t write_read_verify_sector_count_mode : 8;//Word 220. Write-Read-Verify feature set current mode
  uint16_t reserved_word220 : 8;

  uint16_t reserved_word221;//Word 221

  struct {                       //Word 222 Transport major version number
    uint16_t major_version : 12;// 0000h or FFFFh = device does not report version
    uint16_t transport_type : 4;
  } transport_major_version;

  uint16_t transport_minor_version;// Word 223

  uint16_t reserved_word224[6];// Word 224...229

  uint32_t extended_number_of_user_addressable_sectors[2];// Words 230...233 Extended Number of User Addressable Sectors

  uint16_t min_blocks_per_download_microcode_mode03;// Word 234 Minimum number of 512-byte data blocks per Download Microcode mode 03h operation
  uint16_t max_blocks_per_download_microcode_mode03;// Word 235 Maximum number of 512-byte data blocks per Download Microcode mode 03h operation

  uint16_t reserved_word236[19];// Word 236...254

  uint16_t signature : 8;//Word 255
  uint16_t check_sum : 8;

};

#pragma pack(pop)

/*
 * Sanitize Device FEATURE field values
 */
enum {
  SANITIZE_STATUS_EXT = 0x0000,
  SANITIZE_CRYPTO_SCRAMBLE_EXT = 0x0011,
  SANITIZE_BLOCK_ERASE_EXT = 0x0012,
  SANITIZE_OVERWRITE_EXT = 0x0014,
  SANITIZE_FREEZE_LOCK_EXT = 0x0020,
  SANITIZE_ANTIFREEZE_LOCK_EXT = 0x0040,
};

/*
 * Sanitize commands keys
 */
enum {
  SANITIZE_FREEZE_LOCK_KEY = 0x46724C6B,    /* "FrLk" */
  SANITIZE_ANTIFREEZE_LOCK_KEY = 0x416E7469,    /* "Anti" */
  SANITIZE_CRYPTO_SCRAMBLE_KEY = 0x43727970,    /* "Cryp" */
  SANITIZE_BLOCK_ERASE_KEY = 0x426B4572,    /* "BkEr" */
  SANITIZE_OVERWRITE_KEY = 0x00004F57,    /* "OW"   */
};

/*
 * Sanitize outputs flags
 */
enum SanitizeFlag {
  SANITIZE_FLAG_OPERATION_SUCCEEDED = (1U << 7U),
  SANITIZE_FLAG_OPERATION_IN_PROGRESS = (1U << 6U),
  SANITIZE_FLAG_DEVICE_IN_FROZEN = (1U << 5U),
  SANITIZE_FLAG_ANTIFREEZE_BIT = (1U << 4U),
};

} // namespace ata
} // namespace dparm
} // namespace jcu

#endif //JCU_DPARM_ATA_TYPES_H_
