#ifndef FLIP_WIFI_E_H
#define FLIP_WIFI_E_H

#include <flipper_http.h>
#include <easy_flipper.h>
#include <storage/storage.h>

#define TAG               "FlipWiFi"
#define MAX_WIFI_NETWORKS 25

// Define the submenu items for our FlipWiFi application
typedef enum {
    FlipWiFiSubmenuIndexAbout,
    //
    FlipWiFiSubmenuIndexWiFiScan,
    FlipWiFiSubmenuIndexWiFiSaved,
    //
    FlipWiFiSubmenuIndexWiFiSavedAddSSID,
    //
    FlipWiFiSubmenuIndexWiFiScanStart = 100,
    FlipWiFiSubmenuIndexWiFiSavedStart = 200,
} FlipWiFiSubmenuIndex;

// Define a single view for our FlipWiFi application
typedef enum {
    FlipWiFiViewWiFiScan, // The view for the wifi scan screen
    FlipWiFiViewWiFiSaved, // The view for the wifi scan screen
    FlipWiFiViewSubmenuMain, // The submenu for the main screen
    FlipWiFiViewSubmenuScan, // The submenu for the wifi scan screen
    FlipWiFiViewSubmenuSaved, // The submenu for the wifi scan screen
    FlipWiFiViewAbout, // The about screen
    FlipWiFiViewTextInputScan, // The text input screen for the wifi scan screen
    FlipWiFiViewTextInputSaved, // The text input screen for the wifi saved screen
    //
    FlipWiFiViewTextInputSavedAddSSID, // The text input screen for the wifi saved screen
    FlipWiFiViewTextInputSavedAddPassword, // The text input screen for the wifi saved screen
    //
    FlipWiFiViewPopup, // The popup screen
} FlipWiFiView;

// Define the WiFiPlaylist structure
typedef struct {
    char* ssids[MAX_WIFI_NETWORKS];
    char* passwords[MAX_WIFI_NETWORKS];
    size_t count;
} WiFiPlaylist;

// Each screen will have its own view
typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    Popup* popup; // The popup for the app
    View* view_wifi_scan; // The view for the wifi scan screen
    View* view_wifi_saved; // The view for the wifi saved screen
    Submenu* submenu_main; // The submenu for the main screen
    Submenu* submenu_wifi_scan; // The submenu for the wifi scan screen
    Submenu* submenu_wifi_saved; // The submenu for the saved wifi screen
    Widget* widget_info; // The widget
    VariableItemList* variable_item_list_wifi; // The variable item list (settngs)
    VariableItem* variable_item_ssid; // The variable item
    TextInput* uart_text_input_password_scan; // The text input for the wifi scan screen
    TextInput* uart_text_input_password_saved; // The text input for the wifi saved screen
    //
    TextInput* uart_text_input_add_ssid; // The text input for the wifi saved screen
    TextInput* uart_text_input_add_password; // The text input for the wifi saved screen

    char* uart_text_input_buffer_password_scan; // Buffer for the text input
    char* uart_text_input_temp_buffer_password_scan; // Temporary buffer for the text input
    uint32_t uart_text_input_buffer_size_password_scan; // Size of the text input buffer

    char* uart_text_input_buffer_password_saved; // Buffer for the text input
    char* uart_text_input_temp_buffer_password_saved; // Temporary buffer for the text input
    uint32_t uart_text_input_buffer_size_password_saved; // Size of the text input buffer

    char* uart_text_input_buffer_add_ssid; // Buffer for the text input
    char* uart_text_input_temp_buffer_add_ssid; // Temporary buffer for the text input
    uint32_t uart_text_input_buffer_size_add_ssid; // Size of the text input buffer

    char* uart_text_input_buffer_add_password; // Buffer for the text input
    char* uart_text_input_temp_buffer_add_password; // Temporary buffer for the text input
    uint32_t uart_text_input_buffer_size_add_password; // Size of the text input buffer

    WiFiPlaylist wifi_playlist; // The playlist of wifi networks
} FlipWiFiApp;

#endif // FLIP_WIFI_E_H
