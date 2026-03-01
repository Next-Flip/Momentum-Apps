#pragma once

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../flipper_wedge.h"

#define FLIPPER_WEDGE_SETTINGS_FILE_VERSION 5
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/hid_device")
#define FLIPPER_WEDGE_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/hid_device.conf"
#define FLIPPER_WEDGE_SETTINGS_SAVE_PATH_TMP FLIPPER_WEDGE_SETTINGS_SAVE_PATH ".tmp"
#define FLIPPER_WEDGE_SETTINGS_HEADER "FlipperWedge Config File"
#define FLIPPER_WEDGE_SETTINGS_KEY_DELIMITER "Delimiter"
#define FLIPPER_WEDGE_SETTINGS_KEY_APPEND_ENTER "AppendEnter"
#define FLIPPER_WEDGE_SETTINGS_KEY_MODE "Mode"
#define FLIPPER_WEDGE_SETTINGS_KEY_MODE_STARTUP "ModeStartup"
#define FLIPPER_WEDGE_SETTINGS_KEY_OUTPUT_MODE "OutputMode"
#define FLIPPER_WEDGE_SETTINGS_KEY_USB_DEBUG "UsbDebug"
#define FLIPPER_WEDGE_SETTINGS_KEY_VIBRATION "Vibration"
#define FLIPPER_WEDGE_SETTINGS_KEY_NDEF_MAX_LEN "NdefMaxLen"
#define FLIPPER_WEDGE_SETTINGS_KEY_LOG_TO_SD "LogToSd"

void flipper_wedge_save_settings(void* context);
void flipper_wedge_read_settings(void* context);