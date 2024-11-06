#ifndef FLIP_TRADER_FREE_H
#define FLIP_TRADER_FREE_H

// Function to free the resources used by FlipTraderApp
static void flip_trader_app_free(FlipTraderApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "FlipTraderApp is NULL");
        return;
    }

    if(!flipper_http_disconnect_wifi()) {
        FURI_LOG_E(TAG, "Failed to disconnect from wifi");
    }

    // Free View(s)
    if(app->view_main) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewMain);
        view_free(app->view_main);
    }

    // Free Submenu(s)
    if(app->submenu_main) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewMainSubmenu);
        submenu_free(app->submenu_main);
    }
    if(app->submenu_assets) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewAssetsSubmenu);
        submenu_free(app->submenu_assets);
    }

    // Free Widget(s)
    if(app->widget) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewAbout);
        widget_free(app->widget);
    }

    // Free Variable Item List(s)
    if(app->variable_item_list_wifi) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewWiFiSettings);
        variable_item_list_free(app->variable_item_list_wifi);
    }

    // Free Text Input(s)
    if(app->uart_text_input_ssid) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewTextInputSSID);
        text_input_free(app->uart_text_input_ssid);
    }
    if(app->uart_text_input_password) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipTraderViewTextInputPassword);
        text_input_free(app->uart_text_input_password);
    }

    // deinitalize flipper http
    flipper_http_deinit();

    // free the view dispatcher
    view_dispatcher_free(app->view_dispatcher);

    // close the gui
    furi_record_close(RECORD_GUI);

    // free the app
    free(app);
}

#endif // FLIP_TRADER_FREE_H
