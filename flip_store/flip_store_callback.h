#ifndef FLIP_STORE_CALLBACK_H
#define FLIP_STORE_CALLBACK_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <flip_store_apps.h>
#include "flip_store_icons.h"

// Callback for drawing the main screen
static void flip_store_view_draw_callback_main(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_set_font(canvas, FontSecondary);

    if(fhttp.state == INACTIVE) {
        canvas_draw_str(canvas, 0, 7, "Wifi Dev Board disconnected.");
        canvas_draw_str(canvas, 0, 17, "Please connect to the board.");
        canvas_draw_str(canvas, 0, 32, "If your board is connected,");
        canvas_draw_str(canvas, 0, 42, "make sure you have flashed");
        canvas_draw_str(canvas, 0, 52, "your WiFi Devboard with the");
        canvas_draw_str(canvas, 0, 62, "latest FlipperHTTP flash.");
        return;
    }

    if(!flip_store_sent_request) {
        flip_store_sent_request = true;

        if(!flip_store_install_app(canvas, categories[flip_store_category_index])) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "Failed to install app.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        }
    } else {
        if(flip_store_success) {
            if(fhttp.state == RECEIVING) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "Downloading app...");
                canvas_draw_str(canvas, 0, 60, "Please wait...");
                return;
            } else if(fhttp.state == IDLE) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "App installed successfully.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            }
        } else {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "Failed to install app.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        }
    }
}

static void flip_store_view_draw_callback_app_list(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    // Adjusted to access flip_catalog as an array of structures
    canvas_draw_str(canvas, 0, 10, flip_catalog[app_selected_index].app_name);
    // canvas_draw_icon(canvas, 0, 53, &I_ButtonLeft_4x7); (future implementation)
    //  canvas_draw_str_aligned(canvas, 7, 54, AlignLeft, AlignTop, "Delete");  (future implementation)
    canvas_draw_icon(canvas, 0, 53, &I_ButtonBACK_10x8);
    canvas_draw_str_aligned(canvas, 12, 54, AlignLeft, AlignTop, "Back");
    canvas_draw_icon(canvas, 90, 53, &I_ButtonRight_4x7);
    canvas_draw_str_aligned(canvas, 97, 54, AlignLeft, AlignTop, "Install");
}

static bool flip_store_input_callback(InputEvent* event, void* context) {
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipStoreApp is NULL");
        return false;
    }
    if(event->type == InputTypeShort) {
        // Future implementation
        // if (event->key == InputKeyLeft)
        //{
        // Left button clicked, delete the app with DialogEx confirmation
        // view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAppDelete);
        //    return true;
        //}
        if(event->key == InputKeyRight) {
            // Right button clicked, download the app
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewMain);
            return true;
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyBack) {
            // Back button clicked, switch to the previous view.
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAppList);
            return true;
        }
    }

    return false;
}

static void flip_store_text_updated_ssid(void* context) {
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipStoreApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_ssid,
        app->uart_text_input_temp_buffer_ssid,
        app->uart_text_input_buffer_size_ssid);

    // Ensure null-termination
    app->uart_text_input_buffer_ssid[app->uart_text_input_buffer_size_ssid - 1] = '\0';

    // update the variable item text
    if(app->variable_item_ssid) {
        variable_item_set_current_value_text(
            app->variable_item_ssid, app->uart_text_input_buffer_ssid);
    }

    // save the settings
    save_settings(app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_pass);

    // if SSID and PASS are not empty, connect to the WiFi
    if(strlen(app->uart_text_input_buffer_ssid) > 0 &&
       strlen(app->uart_text_input_buffer_pass) > 0) {
        // save wifi settings
        if(!flipper_http_save_wifi(
               app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_pass)) {
            FURI_LOG_E(TAG, "Failed to save WiFi settings");
        }
    }

    // switch to the settings view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewSettings);
}
static void flip_store_text_updated_pass(void* context) {
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipStoreApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_pass,
        app->uart_text_input_temp_buffer_pass,
        app->uart_text_input_buffer_size_pass);

    // Ensure null-termination
    app->uart_text_input_buffer_pass[app->uart_text_input_buffer_size_pass - 1] = '\0';

    // update the variable item text
    if(app->variable_item_pass) {
        variable_item_set_current_value_text(
            app->variable_item_pass, app->uart_text_input_buffer_pass);
    }

    // save the settings
    save_settings(app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_pass);

    // if SSID and PASS are not empty, connect to the WiFi
    if(strlen(app->uart_text_input_buffer_ssid) > 0 &&
       strlen(app->uart_text_input_buffer_pass) > 0) {
        // save wifi settings
        if(!flipper_http_save_wifi(
               app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_pass)) {
            FURI_LOG_E(TAG, "Failed to save WiFi settings");
        }
    }

    // switch to the settings view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewSettings);
}

static uint32_t callback_to_submenu(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return FlipStoreViewSubmenu;
}

static uint32_t callback_to_app_list(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    flip_store_sent_request = false;
    flip_store_success = false;
    flip_store_saved_data = false;
    flip_store_saved_success = false;
    return FlipStoreViewAppList;
}

static void settings_item_selected(void* context, uint32_t index) {
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipStoreApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input SSID
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewTextInputSSID);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewTextInputPass);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

void dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(result == DialogExResultLeft) // No
    {
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAppList);
    } else if(result == DialogExResultRight) {
        // delete the app then return to the app list

        // pop up a message
        popup_set_header(app->popup, "Success", 0, 0, AlignLeft, AlignTop);
        popup_set_text(app->popup, "App deleted successfully.", 0, 60, AlignLeft, AlignTop);
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewPopup);
        furi_delay_ms(2000); // delay for 2 seconds
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAppList);
    }
}

void popup_callback(void* context) {
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipStoreApp is NULL");
        return;
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewSubmenu);
}

/**
 * @brief Navigation callback for exiting the application
 * @param context The context - unused
 * @return next view id (VIEW_NONE to exit the app)
 */
static uint32_t callback_exit_app(void* context) {
    // Exit the application
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return VIEW_NONE; // Return VIEW_NONE to exit the app
}

static void callback_submenu_choices(void* context, uint32_t index) {
    FlipStoreApp* app = (FlipStoreApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipStoreApp is NULL");
        return;
    }
    switch(index) {
    case FlipStoreSubmenuIndexMain:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewMain);
        break;
    case FlipStoreSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAbout);
        break;
    case FlipStoreSubmenuIndexSettings:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewSettings);
        break;
    case FlipStoreSubmenuIndexAppList:
        flip_store_category_index = 0;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAppList);
        break;
    case FlipStoreSubmenuIndexAppListBluetooth:
        flip_store_category_index = 0;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListBluetooth, "Bluetooth", &app->submenu_app_list_bluetooth));
        break;
    case FlipStoreSubmenuIndexAppListGames:
        flip_store_category_index = 1;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListGames, "Games", &app->submenu_app_list_games));
        break;
    case FlipStoreSubmenuIndexAppListGPIO:
        flip_store_category_index = 2;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListGPIO, "GPIO", &app->submenu_app_list_gpio));
        break;
    case FlipStoreSubmenuIndexAppListInfrared:
        flip_store_category_index = 3;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListInfrared, "Infrared", &app->submenu_app_list_infrared));
        break;
    case FlipStoreSubmenuIndexAppListiButton:
        flip_store_category_index = 4;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListiButton, "iButton", &app->submenu_app_list_ibutton));
        break;
    case FlipStoreSubmenuIndexAppListMedia:
        flip_store_category_index = 5;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListMedia, "Media", &app->submenu_app_list_media));
        break;
    case FlipStoreSubmenuIndexAppListNFC:
        flip_store_category_index = 6;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListNFC, "NFC", &app->submenu_app_list_nfc));
        break;
    case FlipStoreSubmenuIndexAppListRFID:
        flip_store_category_index = 7;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListRFID, "RFID", &app->submenu_app_list_rfid));
        break;
    case FlipStoreSubmenuIndexAppListSubGHz:
        flip_store_category_index = 8;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListSubGHz, "Sub-GHz", &app->submenu_app_list_subghz));
        break;
    case FlipStoreSubmenuIndexAppListTools:
        flip_store_category_index = 9;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListTools, "Tools", &app->submenu_app_list_tools));
        break;
    case FlipStoreSubmenuIndexAppListUSB:
        flip_store_category_index = 10;
        view_dispatcher_switch_to_view(
            app->view_dispatcher,
            flip_store_handle_app_list(
                app, FlipStoreViewAppListUSB, "USB", &app->submenu_app_list_usb));
        break;
    default:
        // Check if the index is within the app list range
        if(index >= FlipStoreSubmenuIndexStartAppList &&
           index < FlipStoreSubmenuIndexStartAppList + MAX_APP_COUNT) {
            // Get the app index
            uint32_t app_index = index - FlipStoreSubmenuIndexStartAppList;

            // Check if the app index is valid
            if((int)app_index >= 0 && app_index < MAX_APP_COUNT) {
                // Get the app name
                char* app_name = flip_catalog[app_index].app_name;

                // Check if the app name is valid
                if(app_name != NULL && strlen(app_name) > 0) {
                    app_selected_index = app_index;
                    view_dispatcher_switch_to_view(app->view_dispatcher, FlipStoreViewAppInfo);
                } else {
                    FURI_LOG_E(TAG, "Invalid app name");
                }
            } else {
                FURI_LOG_E(TAG, "Invalid app index");
            }
        } else {
            FURI_LOG_E(TAG, "Unknown submenu index");
        }
        break;
    }
}

#endif // FLIP_STORE_CALLBACK_H
