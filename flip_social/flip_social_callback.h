// flip_social_callback.h
#ifndef FLIP_SOCIAL_CALLBACK_H
#define FLIP_SOCIAL_CALLBACK_H

/**
 * @brief Navigation callback to go back to the submenu Logged out.
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedOutSubmenu)
 */
static uint32_t flip_social_callback_to_submenu_logged_out(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedOutSubmenu;
}

/**
 * @brief Navigation callback to go back to the submenu Logged in.
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInSubmenu)
 */
static uint32_t flip_social_callback_to_submenu_logged_in(void* context) {
    UNUSED(context);
    flip_social_free_explore();
    flip_social_free_feed();
    flip_social_free_friends();
    flip_social_free_message_users();
    flip_social_free_messages();
    return FlipSocialViewLoggedInSubmenu;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged out) Login screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedOutLogin)
 */
static uint32_t flip_social_callback_to_login_logged_out(void* context) {
    UNUSED(context);
    flip_social_sent_login_request = false;
    flip_social_login_success = false;
    return FlipSocialViewLoggedOutLogin;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged out) Register screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedOutRegister)
 */
static uint32_t flip_social_callback_to_register_logged_out(void* context) {
    UNUSED(context);
    flip_social_sent_register_request = false;
    flip_social_register_success = false;
    return FlipSocialViewLoggedOutRegister;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged out) Wifi Settings screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedOutWifiSettings)
 */
static uint32_t flip_social_callback_to_wifi_settings_logged_out(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedOutWifiSettings;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged in) Wifi Settings screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInSettingsWifi)
 */
static uint32_t flip_social_callback_to_wifi_settings_logged_in(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedInSettingsWifi;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged in) Settings screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInSettingsWifi)
 */
static uint32_t flip_social_callback_to_settings_logged_in(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedInSettings;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged in) Compose screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInCompose)
 */
static uint32_t flip_social_callback_to_compose_logged_in(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedInCompose;
}

/**
 * @brief Navigation callback to bring the user back to the (Logged in) Profile screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInProfile)
 */
static uint32_t flip_social_callback_to_profile_logged_in(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedInProfile;
}

/**
 * @brief Navigation callback to bring the user back to the Explore submenu
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInExploreSubmenu)
 */
static uint32_t flip_social_callback_to_explore_logged_in(void* context) {
    UNUSED(context);
    flip_social_dialog_stop = true;
    last_explore_response = "";
    flip_social_dialog_shown = false;
    flip_social_explore->index = 0;
    action = ActionNone;
    return FlipSocialViewLoggedInExploreSubmenu;
}

/**
 * @brief Navigation callback to bring the user back to the Friends submenu
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInFriendsSubmenu)
 */
static uint32_t flip_social_callback_to_friends_logged_in(void* context) {
    UNUSED(context);
    flip_social_dialog_stop = true;
    last_explore_response = "";
    flip_social_dialog_shown = false;
    flip_social_friends->index = 0;
    action = ActionNone;
    return FlipSocialViewLoggedInFriendsSubmenu;
}

/**
 * @brief Navigation callback to bring the user back to the Messages submenu
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInMessagesSubmenu)
 */
static uint32_t flip_social_callback_to_messages_logged_in(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedInMessagesSubmenu;
}

/**
 * @brief Navigation callback to bring the user back to the User Choices screen
 * @param context The context - unused
 * @return next view id (FlipSocialViewLoggedInMessagesUserChoices)
 */
static uint32_t flip_social_callback_to_messages_user_choices(void* context) {
    UNUSED(context);
    return FlipSocialViewLoggedInMessagesUserChoices;
}

/**
 * @brief Navigation callback for exiting the application
 * @param context The context - unused
 * @return next view id (VIEW_NONE to exit the app)
 */
static uint32_t flip_social_callback_exit_app(void* context) {
    // Exit the application
    if(!context) {
        FURI_LOG_E(TAG, "Context is NULL");
        return VIEW_NONE;
    }
    UNUSED(context);
    return VIEW_NONE; // Return VIEW_NONE to exit the app
}

/**
 * @brief Handle ALL submenu item selections.
 * @param context The context - FlipSocialApp object.
 * @param index The FlipSocialSubmenuIndex item that was clicked.
 * @return void
 */
static void flip_social_callback_submenu_choices(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case FlipSocialSubmenuLoggedOutIndexLogin:
        flip_social_sent_login_request = false;
        flip_social_login_success = false;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutLogin);
        break;
    case FlipSocialSubmenuLoggedOutIndexRegister:
        flip_social_sent_register_request = false;
        flip_social_register_success = false;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutRegister);
        break;
    case FlipSocialSubmenuLoggedOutIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutAbout);
        break;
    case FlipSocialSubmenuLoggedOutIndexWifiSettings:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutWifiSettings);
        break;
    case FlipSocialSubmenuLoggedInIndexProfile:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInProfile);
        break;
    case FlipSocialSubmenuLoggedInIndexMessages:
        if(flipper_http_process_response_async(
               flip_social_get_message_users, flip_social_parse_json_message_users)) {
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInMessagesSubmenu);
        }
        break;
    case FlipSocialSubmenuLoggedInIndexMessagesNewMessage:
        if(flipper_http_process_response_async(
               flip_social_get_explore, flip_social_parse_json_message_user_choices)) {
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInMessagesUserChoices);
        }
        break;
    case FlipSocialSubmenuLoggedInIndexFeed:
        if(flipper_http_process_response_async(flip_social_get_feed, flip_social_parse_json_feed)) {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInFeed);
        } else {
            // Set failure FlipSocialFeed object
            if(!flip_social_temp_feed()) {
                return;
            }
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInFeed);
        }
        break;
    case FlipSocialSubmenuExploreIndex:
        if(flipper_http_process_response_async(
               flip_social_get_explore, flip_social_parse_json_explore)) {
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInExploreSubmenu);
        }
        break;
    case FlipSocialSubmenuLoggedInIndexCompose:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInCompose);
        break;
    case FlipSocialSubmenuLoggedInIndexSettings:
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSettings);
        break;
    case FlipSocialSubmenuLoggedInSignOutButton:
        app->is_logged_in = "false";

        save_settings(
            app_instance->wifi_ssid_logged_out,
            app_instance->wifi_password_logged_out,
            app_instance->login_username_logged_out,
            app_instance->login_username_logged_in,
            app_instance->login_password_logged_out,
            app_instance->change_password_logged_in,
            app_instance->is_logged_in);

        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutSubmenu);
        break;
    case FlipSocialSubmenuComposeIndexAddPreSave:
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedInComposeAddPreSaveInput);
        break;
    default:
        // Handle the pre-saved message selection (has a max of 25 items)
        if(index >= FlipSocialSubemnuComposeIndexStartIndex &&
           index < FlipSocialSubemnuComposeIndexStartIndex + MAX_PRE_SAVED_MESSAGES) {
            flip_social_feed->index = index - FlipSocialSubemnuComposeIndexStartIndex;
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInProcessCompose);
        }

        // Handle the explore selection
        else if(
            index >= FlipSocialSubmenuExploreIndexStartIndex &&
            index < FlipSocialSubmenuExploreIndexStartIndex + MAX_EXPLORE_USERS) {
            flip_social_explore->index = index - FlipSocialSubmenuExploreIndexStartIndex;
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInExploreProccess);
        }

        // handle the friends selection
        else if(
            index >= FlipSocialSubmenuLoggedInIndexFriendsStart &&
            index < FlipSocialSubmenuLoggedInIndexFriendsStart + MAX_FRIENDS) {
            flip_social_friends->index = index - FlipSocialSubmenuLoggedInIndexFriendsStart;
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInFriendsProcess);
        }

        // handle the messages selection
        else if(
            index >= FlipSocialSubmenuLoggedInIndexMessagesUsersStart &&
            index < FlipSocialSubmenuLoggedInIndexMessagesUsersStart + MAX_MESSAGE_USERS) {
            flip_social_message_users->index =
                index - FlipSocialSubmenuLoggedInIndexMessagesUsersStart;
            if(flipper_http_process_response_async(
                   flip_social_get_messages_with_user, flip_social_parse_json_messages)) {
                view_dispatcher_switch_to_view(
                    app->view_dispatcher, FlipSocialViewLoggedInMessagesProcess);
            }
        }

        // handle the messages user choices selection
        else if(
            index >= FlipSocialSubmenuLoggedInIndexMessagesUserChoicesIndexStart &&
            index <
                FlipSocialSubmenuLoggedInIndexMessagesUserChoicesIndexStart + MAX_EXPLORE_USERS) {
            flip_social_explore->index =
                index - FlipSocialSubmenuLoggedInIndexMessagesUserChoicesIndexStart;
            view_dispatcher_switch_to_view(
                app->view_dispatcher, FlipSocialViewLoggedInMessagesNewMessageUserChoicesInput);
        } else {
            FURI_LOG_E(TAG, "Unknown submenu index");
        }

        break;
    }
}

/**
 * @brief Text input callback for when the user finishes entering their SSID on the wifi settings (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_out_wifi_settings_ssid_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered name
    strncpy(
        app->wifi_ssid_logged_out,
        app->wifi_ssid_logged_out_temp_buffer,
        app->wifi_ssid_logged_out_temp_buffer_size);

    // Store the entered name in the logged in name field
    strncpy(
        app->wifi_ssid_logged_in,
        app->wifi_ssid_logged_out_temp_buffer,
        app->wifi_ssid_logged_out_temp_buffer_size);
    strncpy(
        app->wifi_ssid_logged_in_temp_buffer,
        app->wifi_ssid_logged_out_temp_buffer,
        app->wifi_ssid_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->wifi_ssid_logged_out[app->wifi_ssid_logged_out_temp_buffer_size - 1] = '\0';

    // Update the name item text
    if(app->variable_item_logged_out_wifi_settings_ssid) {
        variable_item_set_current_value_text(
            app->variable_item_logged_out_wifi_settings_ssid, app->wifi_ssid_logged_out);
    }

    // update the wifi settings
    if(!flipper_http_save_wifi(app->wifi_ssid_logged_out, app->wifi_password_logged_out)) {
        FURI_LOG_E(TAG, "Failed to save wifi settings via UART");
        FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
    }

    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_out,
        app_instance->wifi_password_logged_out,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutWifiSettings);
}

/**
 * @brief Text input callback for when the user finishes entering their password on the wifi settings (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_out_wifi_settings_password_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered WiFi password
    strncpy(
        app->wifi_password_logged_out,
        app->wifi_password_logged_out_temp_buffer,
        app->wifi_password_logged_out_temp_buffer_size);

    // Store the entered WiFi password in the logged in password field
    strncpy(
        app->wifi_password_logged_in,
        app->wifi_password_logged_out_temp_buffer,
        app->wifi_password_logged_out_temp_buffer_size);
    strncpy(
        app->wifi_password_logged_in_temp_buffer,
        app->wifi_password_logged_out_temp_buffer,
        app->wifi_password_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->wifi_password_logged_out[app->wifi_password_logged_out_temp_buffer_size - 1] = '\0';

    // Update the password item text
    if(app->variable_item_logged_out_wifi_settings_password) {
        variable_item_set_current_value_text(
            app->variable_item_logged_out_wifi_settings_password, app->wifi_password_logged_out);
    }

    // update the wifi settings
    if(!flipper_http_save_wifi(app->wifi_ssid_logged_out, app->wifi_password_logged_out)) {
        FURI_LOG_E(TAG, "Failed to save wifi settings via UART");
        FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
    }

    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_out,
        app_instance->wifi_password_logged_out,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutWifiSettings);
}

/**
 * @brief Callback when the user selects a menu item in the wifi settings (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @param index The index of the selected item.
 * @return void
 */
static void
    flip_social_text_input_logged_out_wifi_settings_item_selected(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input SSID
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutWifiSettingsSSIDInput);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutWifiSettingsPasswordInput);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief Text input callback for when the user finishes entering their username on the login (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_out_login_username_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered name
    strncpy(
        app->login_username_logged_out,
        app->login_username_logged_out_temp_buffer,
        app->login_username_logged_out_temp_buffer_size);

    // Store the entered name in the logged in username field
    strncpy(
        app->login_username_logged_in,
        app->login_username_logged_out_temp_buffer,
        app->login_username_logged_out_temp_buffer_size);
    strncpy(
        app->login_username_logged_in_temp_buffer,
        app->login_username_logged_out_temp_buffer,
        app->login_username_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->login_username_logged_out[app->login_username_logged_out_temp_buffer_size - 1] = '\0';

    // Update the name item text
    if(app->variable_item_logged_out_login_username) {
        variable_item_set_current_value_text(
            app->variable_item_logged_out_login_username, app->login_username_logged_out);
    }

    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_out,
        app_instance->wifi_password_logged_out,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutLogin);
}

/**
 * @brief Text input callback for when the user finishes entering their password on the login (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */

static void flip_social_logged_out_login_password_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered password
    strncpy(
        app->login_password_logged_out,
        app->login_password_logged_out_temp_buffer,
        app->login_password_logged_out_temp_buffer_size);

    // Store the entered password in the change password field
    strncpy(
        app->change_password_logged_in,
        app->login_password_logged_out_temp_buffer,
        app->login_password_logged_out_temp_buffer_size);
    strncpy(
        app->change_password_logged_in_temp_buffer,
        app->login_password_logged_out_temp_buffer,
        app->login_password_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->login_password_logged_out[app->login_password_logged_out_temp_buffer_size - 1] = '\0';

    // Update the password item text
    if(app->variable_item_logged_out_login_password) {
        // dont show the password on the screen (version 0.2)
        // variable_item_set_current_value_text(app->variable_item_logged_out_login_password, app->login_password_logged_out);
    }

    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_out,
        app_instance->wifi_password_logged_out,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutLogin);
}

/**
 * @brief Callback when the user selects a menu item in the login (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @param index The index of the selected item.
 * @return void
 */
static void flip_social_text_input_logged_out_login_item_selected(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input Username
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutLoginUsernameInput);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutLoginPasswordInput);
        break;
    case 2: // Login Button
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutProcessLogin);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief Text input callback for when the user finishes entering their username on the register (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_out_register_username_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered name
    strncpy(
        app->register_username_logged_out,
        app->register_username_logged_out_temp_buffer,
        app->register_username_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->register_username_logged_out[app->register_username_logged_out_temp_buffer_size - 1] =
        '\0';

    // Update the name item text
    if(app->variable_item_logged_out_register_username) {
        variable_item_set_current_value_text(
            app->variable_item_logged_out_register_username, app->register_username_logged_out);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutRegister);
}

/**
 * @brief Text input callback for when the user finishes entering their password on the register (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_out_register_password_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered password
    strncpy(
        app->register_password_logged_out,
        app->register_password_logged_out_temp_buffer,
        app->register_password_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->register_password_logged_out[app->register_password_logged_out_temp_buffer_size - 1] =
        '\0';

    // Update the password item text
    if(app->variable_item_logged_out_register_password) {
        variable_item_set_current_value_text(
            app->variable_item_logged_out_register_password, app->register_password_logged_out);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutRegister);
}

/**
 * @brief Text input callback for when the user finishes entering their password 2 on the register (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_out_register_password_2_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered password
    strncpy(
        app->register_password_2_logged_out,
        app->register_password_2_logged_out_temp_buffer,
        app->register_password_2_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->register_password_2_logged_out[app->register_password_2_logged_out_temp_buffer_size - 1] =
        '\0';

    // Update the password item text
    if(app->variable_item_logged_out_register_password_2) {
        variable_item_set_current_value_text(
            app->variable_item_logged_out_register_password_2,
            app->register_password_2_logged_out);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutRegister);
}

/**
 * @brief Callback when the user selects a menu item in the register (logged out) screen.
 * @param context The context - FlipSocialApp object.
 * @param index The index of the selected item.
 * @return void
 */
static void
    flip_social_text_input_logged_out_register_item_selected(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input Username
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutRegisterUsernameInput);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutRegisterPasswordInput);
        break;
    case 2: // Input Password 2
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutRegisterPassword2Input);
        break;
    case 3: // Register button
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedOutProcessRegister);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief Text input callback for when the user finishes entering their SSID on the wifi settings (logged in) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_in_wifi_settings_ssid_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered SSID
    strncpy(
        app->wifi_ssid_logged_in,
        app->wifi_ssid_logged_in_temp_buffer,
        app->wifi_ssid_logged_in_temp_buffer_size);

    // Store the entered SSID in the logged out SSID
    strncpy(
        app->wifi_ssid_logged_out,
        app->wifi_ssid_logged_in,
        app->wifi_ssid_logged_in_temp_buffer_size);
    strncpy(
        app->wifi_ssid_logged_out_temp_buffer,
        app->wifi_ssid_logged_in,
        app->wifi_ssid_logged_in_temp_buffer_size);

    // Ensure null-termination
    app->wifi_ssid_logged_in[app->wifi_ssid_logged_in_temp_buffer_size - 1] = '\0';

    // Update the name item text
    if(app->variable_item_logged_in_wifi_settings_ssid) {
        variable_item_set_current_value_text(
            app->variable_item_logged_in_wifi_settings_ssid, app->wifi_ssid_logged_in);
    }

    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_in,
        app_instance->wifi_password_logged_in,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    // update the wifi settings
    if(strlen(app->wifi_ssid_logged_in) > 0 && strlen(app->wifi_password_logged_in) > 0) {
        if(!flipper_http_save_wifi(app->wifi_ssid_logged_in, app->wifi_password_logged_in)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings via UART");
            FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        }
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSettingsWifi);
}

/**
 * @brief Text input callback for when the user finishes entering their password on the wifi settings (logged in) screen.
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_in_wifi_settings_password_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Store the entered password
    strncpy(
        app->wifi_password_logged_in,
        app->wifi_password_logged_in_temp_buffer,
        app->wifi_password_logged_in_temp_buffer_size);

    // Store the entered password in the logged out password
    strncpy(
        app->login_password_logged_out,
        app->wifi_password_logged_in,
        app->wifi_password_logged_in_temp_buffer_size);
    strncpy(
        app->login_password_logged_out_temp_buffer,
        app->wifi_password_logged_in,
        app->wifi_password_logged_in_temp_buffer_size);

    // Ensure null-termination
    app->wifi_password_logged_in[app->wifi_password_logged_in_temp_buffer_size - 1] = '\0';

    // Update the password item text
    if(app->variable_item_logged_in_wifi_settings_password) {
        // dont show the password on the screen (version 0.2)
        // variable_item_set_current_value_text(app->variable_item_logged_in_wifi_settings_password, app->wifi_password_logged_in);
    }

    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_in,
        app_instance->wifi_password_logged_in,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    // update the wifi settings
    if(strlen(app->wifi_ssid_logged_in) > 0 && strlen(app->wifi_password_logged_in) > 0) {
        if(!flipper_http_save_wifi(app->wifi_ssid_logged_in, app->wifi_password_logged_in)) {
            FURI_LOG_E(TAG, "Failed to save wifi settings via UART");
            FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        }
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSettingsWifi);
}

/**
 * @brief Callback when the user selects a menu item in the wifi settings (logged in) screen.
 * @param context The context - FlipSocialApp object.
 * @param index The index of the selected item.
 * @return void
 */
static void
    flip_social_text_input_logged_in_wifi_settings_item_selected(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Input SSID
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedInWifiSettingsSSIDInput);
        break;
    case 1: // Input Password
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedInWifiSettingsPasswordInput);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief Text input callback for when the user finishes entering their message on the compose (logged in) screen for Add Text
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_in_compose_pre_save_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    // check if the message is empty or if adding in the message would exceed the MAX_PRE_SAVED_MESSAGES
    if(app->compose_pre_save_logged_in_temp_buffer_size == 0 ||
       app->pre_saved_messages.count >= MAX_PRE_SAVED_MESSAGES) {
        FURI_LOG_E(
            TAG, "Message is empty or would exceed the maximum number of pre-saved messages");
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInCompose);
        return;
    }

    // Store the entered message
    strncpy(
        app->compose_pre_save_logged_in,
        app->compose_pre_save_logged_in_temp_buffer,
        app->compose_pre_save_logged_in_temp_buffer_size);

    // Ensure null-termination
    app->compose_pre_save_logged_in[app->compose_pre_save_logged_in_temp_buffer_size - 1] = '\0';

    // add the item to the submenu
    submenu_reset(app->submenu_compose);

    // loop through the items and add them to the submenu
    app->pre_saved_messages.messages[app->pre_saved_messages.count] =
        app->compose_pre_save_logged_in;
    app->pre_saved_messages.count++;

    submenu_add_item(
        app->submenu_compose,
        "Add Pre-Save",
        FlipSocialSubmenuComposeIndexAddPreSave,
        flip_social_callback_submenu_choices,
        app);
    for(uint32_t i = 0; i < app->pre_saved_messages.count; i++) {
        submenu_add_item(
            app->submenu_compose,
            app->pre_saved_messages.messages[i],
            FlipSocialSubemnuComposeIndexStartIndex + i,
            flip_social_callback_submenu_choices,
            app);
    }

    // save playlist
    save_playlist(&app->pre_saved_messages);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInCompose);
}

/**
 * @brief Text input callback for when the user finishes entering their message on the profile (logged in) screen for change password
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_in_profile_change_password_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    // Correct type: old_pass should be a pointer to a string (char *)
    const char* old_password = app->login_password_logged_out;

    // Store the entered message
    strncpy(
        app->change_password_logged_in,
        app->change_password_logged_in_temp_buffer,
        app->change_password_logged_in_temp_buffer_size);

    // store the entered password in the logged out password
    strncpy(
        app->login_password_logged_out,
        app->change_password_logged_in,
        app->login_password_logged_out_temp_buffer_size);
    strncpy(
        app->login_password_logged_out_temp_buffer,
        app->change_password_logged_in,
        app->login_password_logged_out_temp_buffer_size);

    // Ensure null-termination
    app->change_password_logged_in[app->change_password_logged_in_temp_buffer_size - 1] = '\0';

    // Update the message item text
    if(app->variable_item_logged_in_profile_change_password) {
        // dont show the password on the screen (version 0.2)
        // variable_item_set_current_value_text(app->variable_item_logged_in_profile_change_password, app->change_password_logged_in);
    }

    // send post request to change password
    char payload[256];
    snprintf(
        payload,
        sizeof(payload),
        "{\"username\":\"%s\",\"old_password\":\"%s\",\"new_password\":\"%s\"}",
        app->login_username_logged_out,
        old_password,
        app->change_password_logged_in);
    char* headers = jsmn("Content-Type", "application/json");
    if(!flipper_http_post_request_with_headers(
           "https://www.flipsocial.net/api/user/change-password/", headers, payload)) {
        FURI_LOG_E(TAG, "Failed to send post request to change password");
        FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        free(headers);
        return;
    }
    free(headers);
    // Save the settings
    save_settings(
        app_instance->wifi_ssid_logged_out,
        app_instance->wifi_password_logged_out,
        app_instance->login_username_logged_out,
        app_instance->login_username_logged_in,
        app_instance->login_password_logged_out,
        app_instance->change_password_logged_in,
        app_instance->is_logged_in);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInProfile);
}

/**
 * @brief Callback when a user selects a menu item in the profile (logged in) screen.
 * @param context The context - FlipSocialApp object.
 * @param index The index of the selected item.
 * @return void
 */
static void flip_social_text_input_logged_in_profile_item_selected(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case 0: // Change Username
        // do nothing since username cannot be changed
        break;
    case 1: // Change Password
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedInChangePasswordInput);
        break;
    case 2: // Friends
        // get friends then switch to the friends screen
        if(flip_social_get_friends()) // start the async friends request
        {
            furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        }
        while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
            // Wait for the friends to be received
            furi_delay_ms(100);
        }
        furi_timer_stop(fhttp.get_timeout_timer);
        if(!flip_social_parse_json_friends()) // parse the JSON before switching to the friends (synchonous)
        {
            FURI_LOG_E(TAG, "Failed to parse the JSON friends...");
            return; // just return for now, no temporary friends yet
            // show a popup message saving wifi is disconnected
        }
        furi_timer_stop(fhttp.get_timeout_timer);
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInFriendsSubmenu);
        break;
    default:
        FURI_LOG_E(TAG, "Unknown configuration item index");
        break;
    }
}

/**
 * @brief Callback when a user selects a menu item in the settings (logged in) screen.
 * @param context The context - FlipSocialApp object.
 * @param index The index of the selected item.
 * @return void
 */
static void
    flip_social_text_input_logged_in_settings_item_selected(void* context, uint32_t index) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    switch(index) {
    case 0: // About
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSettingsAbout);
        break;
    case 1: // Wifi
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSettingsWifi);
        break;
    default:
        break;
    }
}

/**
 * @brief Text input callback for when the user finishes entering their message to send to the selected user choice (user choice messages view)
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_in_messages_user_choice_message_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    // check if the message is empty
    if(app->message_user_choice_logged_in_temp_buffer_size == 0) {
        FURI_LOG_E(TAG, "Message is empty");
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedInMessagesNewMessageUserChoicesInput);
        return;
    }

    // Store the entered message
    strncpy(
        app->message_user_choice_logged_in,
        app->message_user_choice_logged_in_temp_buffer,
        app->message_user_choice_logged_in_temp_buffer_size);

    // Ensure null-termination
    app->message_user_choice_logged_in[app->message_user_choice_logged_in_temp_buffer_size - 1] =
        '\0';

    // send post request to send message
    char url[128];
    char payload[256];
    snprintf(
        url,
        sizeof(url),
        "https://www.flipsocial.net/api/messages/%s/post/",
        app->login_username_logged_in);
    snprintf(
        payload,
        sizeof(payload),
        "{\"receiver\":\"%s\",\"content\":\"%s\"}",
        flip_social_explore->usernames[flip_social_explore->index],
        app->message_user_choice_logged_in);
    char* headers = jsmn("Content-Type", "application/json");

    if(flipper_http_post_request_with_headers(url, headers, payload)) // start the async request
    {
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
        free(headers);
    } else {
        FURI_LOG_E(TAG, "Failed to send post request to send message");
        FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        free(headers);
        return;
    }
    while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
        // Wait for the request to be received
        furi_delay_ms(100);
    }
    furi_timer_stop(fhttp.get_timeout_timer);

    // add user to the message list
    strncpy(
        flip_social_message_users->usernames[flip_social_message_users->count],
        flip_social_explore->usernames[flip_social_explore->index],
        strlen(flip_social_explore->usernames[flip_social_explore->index]));
    flip_social_message_users->count++;

    // redraw submenu
    flip_social_update_messages_submenu();
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInMessagesSubmenu);
}

/**
 * @brief Text input callback for when the user finishes entering their message to the selected user (messages view)
 * @param context The context - FlipSocialApp object.
 * @return void
 */
static void flip_social_logged_in_messages_new_message_updated(void* context) {
    FlipSocialApp* app = (FlipSocialApp*)context;
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    // check if the message is empty
    if(app->messages_new_message_logged_in_temp_buffer_size == 0) {
        FURI_LOG_E(TAG, "Message is empty");
        view_dispatcher_switch_to_view(
            app->view_dispatcher, FlipSocialViewLoggedInMessagesNewMessageInput);
        return;
    }

    // Store the entered message
    strncpy(
        app->messages_new_message_logged_in,
        app->messages_new_message_logged_in_temp_buffer,
        app->messages_new_message_logged_in_temp_buffer_size);

    // Ensure null-termination
    app->messages_new_message_logged_in[app->messages_new_message_logged_in_temp_buffer_size - 1] =
        '\0';

    // send post request to send message
    char url[128];
    char payload[256];
    snprintf(
        url,
        sizeof(url),
        "https://www.flipsocial.net/api/messages/%s/post/",
        app->login_username_logged_in);
    snprintf(
        payload,
        sizeof(payload),
        "{\"receiver\":\"%s\",\"content\":\"%s\"}",
        flip_social_message_users->usernames[flip_social_message_users->index],
        app->messages_new_message_logged_in);
    char* headers = jsmn("Content-Type", "application/json");

    if(flipper_http_post_request_with_headers(url, headers, payload)) // start the async request
    {
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
        free(headers);
    } else {
        FURI_LOG_E(TAG, "Failed to send post request to send message");
        FURI_LOG_E(TAG, "Make sure the Flipper is connected to the Wifi Dev Board");
        free(headers);
        return;
    }
    while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
        // Wait for the request to be received
        furi_delay_ms(100);
    }
    furi_timer_stop(fhttp.get_timeout_timer);
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInMessagesSubmenu);
}

#endif // FLIP_SOCIAL_CALLBACK_H
