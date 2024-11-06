#ifndef FLIP_LIBRARY_E_H
#define FLIP_LIBRARY_E_H

#include <flipper_http.h>
#include <easy_flipper.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <dialogs/dialogs.h>
#include <jsmn.h>

#define TAG "FlipLibrary"

// Define the submenu items for our FlipLibrary application
typedef enum {
    FlipLibrarySubmenuIndexRandomFacts, // Click to run the random facts
    FlipLibrarySubmenuIndexDictionary, // click to view the dictionary variable item list
    FlipLibrarySubmenuIndexAbout, // Click to view the about screen
    FlipLibrarySubmenuIndexSettings, // Click to view the WiFi settings
    //
    FlipLibrarySubmenuIndexRandomFactsCats, // Click to view the random facts (cats)
    FlipLibrarySubmenuIndexRandomFactsDogs, // Click to view the random facts (dogs)
    FlipLibrarySubmenuIndexRandomFactsQuotes, // Click to view the random facts (quotes)
    FlipLibrarySubmenuIndexRandomFactsAll, // Click to view the random facts (all)
} FlipLibrarySubmenuIndex;

// Define a single view for our FlipLibrary application
typedef enum {
    FlipLibraryViewRandomFacts = 7, // The random facts main screen
    FlipLibraryViewRandomFactsRun = 8, // The random facts widget that displays the random fact
    FlipLibraryViewSubmenuMain = 9, // The submenu screen
    FlipLibraryViewAbout = 10, // The about screen
    FlipLibraryViewSettings = 11, // The settings screen
    FlipLibraryViewTextInputSSID = 12, // The text input screen (SSID)
    FlipLibraryViewTextInputPassword = 13, // The text input screen (password)
    FlipLibraryViewDictionary = 14, // The dictionary submenu screen
    //
    FlipLibraryViewDictionaryTextInput = 15,
    FlipLibraryViewDictionaryRun = 16,
    //
    FlipLibraryViewRandomFactsCats = 17,
    FlipLibraryViewRandomFactsDogs = 18,
    FlipLibraryViewRandomFactsQuotes = 19,
    FlipLibraryViewRandomFactsAll = 20,
    //
    FlipLibraryViewRandomFactWidget = 21, // The text box that displays the random fact
    FlipLibraryViewDictionaryWidget = 22, // The text box that displays the dictionary
} FlipLibraryView;

// Each screen will have its own view
typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    View* view_random_facts; // The main screen that displays the random fact
    View* view_dictionary; // The dictionary screen
    Submenu* submenu_main; // The submenu for the main screen
    Submenu* submenu_random_facts; // The submenu for the random facts screen
    Widget* widget; // The widget
    VariableItemList* variable_item_list_wifi; // The variable item list (WiFi settings)
    VariableItem* variable_item_ssid; // The variable item (SSID)
    VariableItem* variable_item_password; // The variable item (password)
    TextInput* uart_text_input_ssid; // The text input for the SSID
    TextInput* uart_text_input_password; // The text input for the password
    TextInput* uart_text_input_dictionary; // The text input for the dictionary
    //
    Widget* widget_random_fact; // The text box that displays the random fact
    Widget* widget_dictionary; // The text box that displays the dictionary

    char* uart_text_input_buffer_ssid; // Buffer for the text input (SSID)
    char* uart_text_input_temp_buffer_ssid; // Temporary buffer for the text input (SSID)
    uint32_t uart_text_input_buffer_size_ssid; // Size of the text input buffer (SSID)

    char* uart_text_input_buffer_password; // Buffer for the text input (password)
    char* uart_text_input_temp_buffer_password; // Temporary buffer for the text input (password)
    uint32_t uart_text_input_buffer_size_password; // Size of the text input buffer (password)

    char* uart_text_input_buffer_dictionary; // Buffer for the text input (dictionary)
    char* uart_text_input_temp_buffer_dictionary; // Temporary buffer for the text input (dictionary)
    uint32_t uart_text_input_buffer_size_dictionary; // Size of the text input buffer (dictionary)
} FlipLibraryApp;

#endif // FLIP_LIBRARY_E_H
