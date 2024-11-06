#ifndef FLIP_LIBRARY_FREE_H
#define FLIP_LIBRARY_FREE_H

// Function to free the resources used by FlipLibraryApp
static void flip_library_app_free(FlipLibraryApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "FlipLibraryApp is NULL");
        return;
    }

    // Free View(s)
    if(app->view_random_facts) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewRandomFactsRun);
        view_free(app->view_random_facts);
    }
    if(app->view_dictionary) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewDictionaryRun);
        view_free(app->view_dictionary);
    }

    // Free Submenu(s)
    if(app->submenu_main) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewSubmenuMain);
        submenu_free(app->submenu_main);
    }
    if(app->submenu_random_facts) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewRandomFacts);
        submenu_free(app->submenu_random_facts);
    }

    // Free Widget(s)
    if(app->widget) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewAbout);
        widget_free(app->widget);
    }
    if(app->widget_random_fact) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewRandomFactWidget);
        widget_free(app->widget_random_fact);
    }
    if(app->widget_dictionary) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewDictionaryWidget);
        widget_free(app->widget_dictionary);
    }

    // Free Variable Item List(s)
    if(app->variable_item_list_wifi) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewSettings);
        variable_item_list_free(app->variable_item_list_wifi);
    }

    // Free Text Input(s)
    if(app->uart_text_input_ssid) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewTextInputSSID);
        text_input_free(app->uart_text_input_ssid);
    }
    if(app->uart_text_input_password) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewTextInputPassword);
        text_input_free(app->uart_text_input_password);
    }
    if(app->uart_text_input_dictionary) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipLibraryViewDictionaryTextInput);
        text_input_free(app->uart_text_input_dictionary);
    }

    // deinitalize flipper http
    flipper_http_deinit();

    // free the view dispatcher
    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);

    // close the gui
    furi_record_close(RECORD_GUI);

    if(app_instance) {
        // free the app instance
        free(app_instance);
        app_instance = NULL;
    }
    // free the app
    free(app);
}

#endif // FLIP_LIBRARY_FREE_H
