#ifndef FLIP_WEATHER_FREE_H
#define FLIP_WEATHER_FREE_H

// Function to free the resources used by FlipWeatherApp
static void flip_weather_app_free(FlipWeatherApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "FlipWeatherApp is NULL");
        return;
    }

    // Free View(s)
    if(app->view_weather) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewWeather);
        view_free(app->view_weather);
    }
    if(app->view_gps) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewGPS);
        view_free(app->view_gps);
    }

    // Free Submenu(s)
    if(app->submenu) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewSubmenu);
        submenu_free(app->submenu);
    }

    // Free Widget(s)
    if(app->widget) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewAbout);
        widget_free(app->widget);
    }

    // Free Variable Item List(s)
    if(app->variable_item_list) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewSettings);
        variable_item_list_free(app->variable_item_list);
    }

    // Free Text Input(s)
    if(app->uart_text_input_ssid) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewTextInputSSID);
        text_input_free(app->uart_text_input_ssid);
    }
    if(app->uart_text_input_password) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipWeatherViewTextInputPassword);
        text_input_free(app->uart_text_input_password);
    }

    // Free the text input buffer
    if(app->uart_text_input_buffer_ssid) free(app->uart_text_input_buffer_ssid);
    if(app->uart_text_input_temp_buffer_ssid) free(app->uart_text_input_temp_buffer_ssid);
    if(app->uart_text_input_buffer_password) free(app->uart_text_input_buffer_password);
    if(app->uart_text_input_temp_buffer_password) free(app->uart_text_input_temp_buffer_password);

    // deinitalize flipper http
    flipper_http_deinit();

    // free the view dispatcher
    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);

    // close the gui
    furi_record_close(RECORD_GUI);

    // free the app
    if(app) free(app);
}

#endif // FLIP_WEATHER_FREE_H
