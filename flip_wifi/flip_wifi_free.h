#ifndef FLIP_WIFI_FREE_H
#define FLIP_WIFI_FREE_H

// Function to free the resources used by FlipWiFiApp
static void flip_wifi_app_free(FlipWiFiApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "FlipWiFiApp is NULL");
        return;
    }

    // Free View(s)
    if(app->view_wifi_scan) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewWiFiScan);
        view_free(app->view_wifi_scan);
    }
    if(app->view_wifi_saved) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewWiFiSaved);
        view_free(app->view_wifi_saved);
    }

    // Free Submenu(s)
    if(app->submenu_main) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewSubmenuMain);
        submenu_free(app->submenu_main);
    }
    if(app->submenu_wifi_scan) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewSubmenuScan);
        submenu_free(app->submenu_wifi_scan);
    }
    if(app->submenu_wifi_saved) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewSubmenuSaved);
        submenu_free(app->submenu_wifi_saved);
    }

    // Free Widget(s)
    if(app->widget_info) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewAbout);
        widget_free(app->widget_info);
    }

    // Free Text Input(s)
    if(app->uart_text_input_password_scan) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewTextInputScan);
        text_input_free(app->uart_text_input_password_scan);
    }
    if(app->uart_text_input_password_saved) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewTextInputSaved);
        text_input_free(app->uart_text_input_password_saved);
    }
    if(app->uart_text_input_add_ssid) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewTextInputSavedAddSSID);
        text_input_free(app->uart_text_input_add_ssid);
    }
    if(app->uart_text_input_add_password) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewTextInputSavedAddPassword);
        text_input_free(app->uart_text_input_add_password);
    }

    // free playlist
    for(size_t i = 0; i < app->wifi_playlist.count; i++) {
        if(app->wifi_playlist.ssids[i]) free(app->wifi_playlist.ssids[i]);
        if(app->wifi_playlist.passwords[i]) free(app->wifi_playlist.passwords[i]);
    }

    // free popup
    if(app->popup) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWiFiViewPopup);
        popup_free(app->popup);
    }

    // deinitalize flipper http
    flipper_http_deinit();

    // free the view dispatcher
    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);

    // close the gui
    furi_record_close(RECORD_GUI);

    // free the app
    if(app) free(app);
}

#endif // FLIP_WIFI_FREE_H
