#ifndef FLIP_LIBRARY_I_H
#define FLIP_LIBRARY_I_H

// Function to allocate resources for the FlipLibraryApp
static FlipLibraryApp* flip_library_app_alloc() {
    FlipLibraryApp* app = (FlipLibraryApp*)malloc(sizeof(FlipLibraryApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize flipper http");
        return NULL;
    }

    // Allocate the text input buffer
    app->uart_text_input_buffer_size_ssid = 64;
    app->uart_text_input_buffer_size_password = 64;
    app->uart_text_input_buffer_size_dictionary = 64;
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_ssid, app->uart_text_input_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_ssid, app->uart_text_input_buffer_size_ssid)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_password, app->uart_text_input_buffer_size_password)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_password,
           app->uart_text_input_buffer_size_password)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_buffer_dictionary, app->uart_text_input_buffer_size_dictionary)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->uart_text_input_temp_buffer_dictionary,
           app->uart_text_input_buffer_size_dictionary)) {
        return NULL;
    }

    // Allocate ViewDispatcher
    if(!easy_flipper_set_view_dispatcher(&app->view_dispatcher, gui, app)) {
        return NULL;
    }

    // Main view
    if(!easy_flipper_set_view(
           &app->view_random_facts,
           FlipLibraryViewRandomFactsRun,
           view_draw_callback_random_facts,
           NULL,
           callback_to_random_facts,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_dictionary,
           FlipLibraryViewDictionaryRun,
           view_draw_callback_dictionary_run,
           NULL,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Widget
    if(!easy_flipper_set_widget(
           &app->widget,
           FlipLibraryViewAbout,
           "FlipLibrary v1.2\n-----\nDictionary, random facts, and\nmore.\n-----\nwww.github.com/jblanked",
           callback_to_submenu,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_widget(
           &app->widget_random_fact,
           FlipLibraryViewRandomFactWidget,
           "Error, try again.",
           callback_to_random_facts,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_widget(
           &app->widget_dictionary,
           FlipLibraryViewDictionaryWidget,
           "Error, try again.",
           callback_to_submenu,
           &app->view_dispatcher)) {
        return NULL;
    }

    // Text Input
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_ssid,
           FlipLibraryViewTextInputSSID,
           "Enter SSID",
           app->uart_text_input_temp_buffer_ssid,
           app->uart_text_input_buffer_size_ssid,
           text_updated_ssid,
           callback_to_wifi_settings,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_password,
           FlipLibraryViewTextInputPassword,
           "Enter Password",
           app->uart_text_input_temp_buffer_password,
           app->uart_text_input_buffer_size_password,
           text_updated_password,
           callback_to_wifi_settings,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->uart_text_input_dictionary,
           FlipLibraryViewDictionaryTextInput,
           "Enter a word",
           app->uart_text_input_temp_buffer_dictionary,
           app->uart_text_input_buffer_size_dictionary,
           text_updated_dictionary,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Variable Item List
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_wifi,
           FlipLibraryViewSettings,
           settings_item_selected,
           callback_to_submenu,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    app->variable_item_ssid =
        variable_item_list_add(app->variable_item_list_wifi, "SSID", 0, NULL, NULL);
    app->variable_item_password =
        variable_item_list_add(app->variable_item_list_wifi, "Password", 0, NULL, NULL);
    variable_item_set_current_value_text(app->variable_item_ssid, "");
    variable_item_set_current_value_text(app->variable_item_password, "");

    // Submenu
    if(!easy_flipper_set_submenu(
           &app->submenu_main,
           FlipLibraryViewSubmenuMain,
           "FlipLibrary v1.2",
           callback_exit_app,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_random_facts,
           FlipLibraryViewRandomFacts,
           "Random Facts",
           callback_to_submenu,
           &app->view_dispatcher)) {
        return NULL;
    }

    submenu_add_item(
        app->submenu_main,
        "Random Fact",
        FlipLibrarySubmenuIndexRandomFacts,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_main,
        "Dictionary",
        FlipLibrarySubmenuIndexDictionary,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_main, "About", FlipLibrarySubmenuIndexAbout, callback_submenu_choices, app);
    submenu_add_item(
        app->submenu_main, "WiFi", FlipLibrarySubmenuIndexSettings, callback_submenu_choices, app);
    //
    submenu_add_item(
        app->submenu_random_facts,
        "Cats",
        FlipLibrarySubmenuIndexRandomFactsCats,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_random_facts,
        "Dogs",
        FlipLibrarySubmenuIndexRandomFactsDogs,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_random_facts,
        "Quotes",
        FlipLibrarySubmenuIndexRandomFactsQuotes,
        callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_random_facts,
        "Random",
        FlipLibrarySubmenuIndexRandomFactsAll,
        callback_submenu_choices,
        app);

    // load settings
    if(load_settings(
           app->uart_text_input_buffer_ssid,
           app->uart_text_input_buffer_size_ssid,
           app->uart_text_input_buffer_password,
           app->uart_text_input_buffer_size_password)) {
        // Update variable items
        if(app->variable_item_ssid)
            variable_item_set_current_value_text(
                app->variable_item_ssid, app->uart_text_input_buffer_ssid);
        // dont show password

        // Copy items into their temp buffers with safety checks
        if(app->uart_text_input_buffer_ssid && app->uart_text_input_temp_buffer_ssid) {
            strncpy(
                app->uart_text_input_temp_buffer_ssid,
                app->uart_text_input_buffer_ssid,
                app->uart_text_input_buffer_size_ssid - 1);
            app->uart_text_input_temp_buffer_ssid[app->uart_text_input_buffer_size_ssid - 1] =
                '\0';
        }
        if(app->uart_text_input_buffer_password && app->uart_text_input_temp_buffer_password) {
            strncpy(
                app->uart_text_input_temp_buffer_password,
                app->uart_text_input_buffer_password,
                app->uart_text_input_buffer_size_password - 1);
            app->uart_text_input_temp_buffer_password[app->uart_text_input_buffer_size_password - 1] =
                '\0';
        }
    }

    // assign app instance
    app_instance = app;

    // start with the main view
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipLibraryViewSubmenuMain);

    return app;
}

#endif // FLIP_LIBRARY_I_H
