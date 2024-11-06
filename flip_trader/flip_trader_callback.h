#ifndef FLIP_TRADER_CALLBACK_H
#define FLIP_TRADER_CALLBACK_H

#define MAX_TOKENS 32 // Adjust based on expected JSON size (25)

// hold the price of the asset
static char asset_price[64];
static bool sent_get_request = false;
static bool get_request_success = false;
static bool request_processed = false;

void flip_trader_request_error(Canvas* canvas) {
    if(fhttp.received_data == NULL) {
        if(fhttp.last_response != NULL) {
            if(strstr(fhttp.last_response, "[ERROR] Not connected to Wifi. Failed to reconnect.") !=
               NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
                canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else if(strstr(fhttp.last_response, "[ERROR] Failed to connect to Wifi.") != NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
                canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else if(strstr(fhttp.last_response, "[ERROR] WiFi SSID or Password is empty") != NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
                canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else {
                canvas_clear(canvas);
                FURI_LOG_E(TAG, "Received an error: %s", fhttp.last_response);
                canvas_draw_str(canvas, 0, 10, "[ERROR] Unusual error...");
                canvas_draw_str(canvas, 0, 60, "Press BACK and retry.");
            }
        } else {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "[ERROR] Unknown error.");
            canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        }
    } else {
        canvas_clear(canvas);
        canvas_draw_str(canvas, 0, 10, "Failed to receive data.");
        canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
    }
}

static bool send_price_request() {
    if(!sent_get_request && fhttp.state == IDLE) {
        sent_get_request = true;
        char url[128] = {0};
        snprintf(
            url,
            128,
            "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%s&apikey=2X90WLEFMP43OJKE",
            asset_names[asset_index]);
        get_request_success =
            flipper_http_get_request_with_headers(url, "{\"Content-Type\": \"application/json\"}");
        if(!get_request_success) {
            FURI_LOG_E(TAG, "Failed to send GET request");
            return false;
        }
        fhttp.state = RECEIVING;
    }
    return true;
}

static void process_asset_price() {
    if(!request_processed && fhttp.received_data != NULL) {
        request_processed = true;
        char* global_quote = get_json_value("Global Quote", fhttp.received_data, MAX_TOKENS);
        if(global_quote == NULL) {
            FURI_LOG_E(TAG, "Failed to get Global Quote");
            return;
        }
        char* price = get_json_value("05. price", global_quote, MAX_TOKENS);
        if(price == NULL) {
            FURI_LOG_E(TAG, "Failed to get price");
            return;
        }
        // store the price "Asset: $price"
        snprintf(asset_price, 64, "%s: $%s", asset_names[asset_index], price);

        fhttp.state = IDLE;
    } else if(!request_processed && fhttp.received_data == NULL) {
        request_processed = true;
        // store an error message instead of the price
        snprintf(asset_price, 64, "Failed. Update WiFi settings.");
        fhttp.state = ISSUE;
    }
}

// Callback for drawing the main screen
static void flip_trader_view_draw_callback(Canvas* canvas, void* model) {
    if(!canvas) {
        return;
    }
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

    canvas_draw_str(canvas, 0, 10, "Loading...");
    // canvas_draw_str(canvas, 0, 10, asset_names[asset_index]);

    // start the process
    if(!send_price_request()) {
        flip_trader_request_error(canvas);
    }
    // wait until the request is processed
    if(!sent_get_request || !get_request_success || fhttp.state == RECEIVING) {
        return;
    }
    // check status
    if(fhttp.state == ISSUE || fhttp.received_data == NULL) {
        flip_trader_request_error(canvas);
    }
    // success, process the data
    process_asset_price();
    canvas_clear(canvas);
    canvas_draw_str(canvas, 0, 10, asset_price);
}

// Input callback for the view (async input handling)
bool flip_trader_view_input_callback(InputEvent* event, void* context) {
    FlipTraderApp* app = (FlipTraderApp*)context;
    if(event->type == InputTypePress && event->key == InputKeyBack) {
        // Exit the app when the back button is pressed
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    }
    return false;
}

static void callback_submenu_choices(void* context, uint32_t index) {
    FlipTraderApp* app = (FlipTraderApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipTraderApp is NULL");
        return;
    }
    switch(index) {
    // view the assets submenu
    case FlipTradeSubmenuIndexAssets:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewAssetsSubmenu);
        break;
    // view the about screen
    case FlipTraderSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewAbout);
        break;
    // view the wifi settings screen
    case FlipTraderSubmenuIndexSettings:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewWiFiSettings);
        break;
    default:
        // handle FlipTraderSubmenuIndexAssetStartIndex + index
        if(index >= FlipTraderSubmenuIndexAssetStartIndex) {
            asset_index = index - FlipTraderSubmenuIndexAssetStartIndex;
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewMain);
        } else {
            FURI_LOG_E(TAG, "Unknown submenu index");
        }
        break;
    }
}

static void text_updated_ssid(void* context) {
    FlipTraderApp* app = (FlipTraderApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipTraderApp is NULL");
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

    // save settings
    save_settings(app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password);

    // save wifi settings to devboard
    if(strlen(app->uart_text_input_buffer_ssid) > 0 &&
       strlen(app->uart_text_input_buffer_password) > 0) {
        if(!flipper_http_save_wifi(
               app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings");
        }
    }

    // switch to the settings view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewWiFiSettings);
}

static void text_updated_password(void* context) {
    FlipTraderApp* app = (FlipTraderApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipTraderApp is NULL");
        return;
    }

    // store the entered text
    strncpy(
        app->uart_text_input_buffer_password,
        app->uart_text_input_temp_buffer_password,
        app->uart_text_input_buffer_size_password);

    // Ensure null-termination
    app->uart_text_input_buffer_password[app->uart_text_input_buffer_size_password - 1] = '\0';

    // update the variable item text
    if(app->variable_item_password) {
        variable_item_set_current_value_text(
            app->variable_item_password, app->uart_text_input_buffer_password);
    }

    // save settings
    save_settings(app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password);

    // save wifi settings to devboard
    if(strlen(app->uart_text_input_buffer_ssid) > 0 &&
       strlen(app->uart_text_input_buffer_password) > 0) {
        if(!flipper_http_save_wifi(
               app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_password)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings");
        }
    }

    // switch to the settings view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewWiFiSettings);
}

static uint32_t callback_to_submenu(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    sent_get_request = false;
    get_request_success = false;
    request_processed = false;
    asset_index = 0;
    return FlipTraderViewMainSubmenu;
}

static uint32_t callback_to_wifi_settings(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return FlipTraderViewWiFiSettings;
}

static uint32_t callback_to_assets_submenu(void* context) {
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    sent_get_request = false;
    get_request_success = false;
    request_processed = false;
    asset_index = 0;
    return FlipTraderViewAssetsSubmenu;
}

static void settings_item_selected(void* context, uint32_t index) {
    FlipTraderApp* app = (FlipTraderApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipTraderApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input SSID
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewTextInputSSID);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipTraderViewTextInputPassword);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

#endif // FLIP_TRADER_CALLBACK_H
