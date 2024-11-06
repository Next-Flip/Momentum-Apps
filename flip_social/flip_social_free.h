// flip_social_free.h
#ifndef FLIP_SOCIAL_FREE_H
#define FLIP_SOCIAL_FREE_H

/**
 * @brief Function to free the resources used by FlipSocialApp.
 * @details Cleans up all allocated resources before exiting the application.
 * @param app The FlipSocialApp object to free.
 * @return void
 */
static void flip_social_app_free(FlipSocialApp* app) {
    if(!app) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }
    if(!app->view_dispatcher) {
        FURI_LOG_E(TAG, "ViewDispatcher is NULL");
        return;
    }

    // Free Submenu(s)
    if(app->submenu_logged_out) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutSubmenu);
        submenu_free(app->submenu_logged_out);
    }
    if(app->submenu_logged_in) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInSubmenu);
        submenu_free(app->submenu_logged_in);
    }
    if(app->submenu_compose) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInCompose);
        submenu_free(app->submenu_compose);
    }
    if(app->submenu_explore) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInExploreSubmenu);
        submenu_free(app->submenu_explore);
    }
    if(app->submenu_friends) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInFriendsSubmenu);
        submenu_free(app->submenu_friends);
    }
    if(app->submenu_messages) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInMessagesSubmenu);
        submenu_free(app->submenu_messages);
    }
    if(app->submenu_messages_user_choices) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInMessagesUserChoices);
        submenu_free(app->submenu_messages_user_choices);
    }

    // Free Variable Item List(s)
    if(app->variable_item_list_logged_out_wifi_settings) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutWifiSettings);
        variable_item_list_free(app->variable_item_list_logged_out_wifi_settings);
    }
    if(app->variable_item_list_logged_out_login) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutLogin);
        variable_item_list_free(app->variable_item_list_logged_out_login);
    }
    if(app->variable_item_list_logged_out_register) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutRegister);
        variable_item_list_free(app->variable_item_list_logged_out_register);
    }
    if(app->variable_item_list_logged_in_profile) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInProfile);
        variable_item_list_free(app->variable_item_list_logged_in_profile);
    }
    if(app->variable_item_list_logged_in_settings) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInSettings);
        variable_item_list_free(app->variable_item_list_logged_in_settings);
    }
    if(app->variable_item_list_logged_in_settings_wifi) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInSettingsWifi);
        variable_item_list_free(app->variable_item_list_logged_in_settings_wifi);
    }

    // Free Text Input(s)
    if(app->text_input_logged_out_wifi_settings_ssid) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutWifiSettingsSSIDInput);
        text_input_free(app->text_input_logged_out_wifi_settings_ssid);
    }
    if(app->text_input_logged_out_wifi_settings_password) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutWifiSettingsPasswordInput);
        text_input_free(app->text_input_logged_out_wifi_settings_password);
    }
    if(app->text_input_logged_out_login_username) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutLoginUsernameInput);
        text_input_free(app->text_input_logged_out_login_username);
    }
    if(app->text_input_logged_out_login_password) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutLoginPasswordInput);
        text_input_free(app->text_input_logged_out_login_password);
    }
    if(app->text_input_logged_out_register_username) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutRegisterUsernameInput);
        text_input_free(app->text_input_logged_out_register_username);
    }
    if(app->text_input_logged_out_register_password) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutRegisterPasswordInput);
        text_input_free(app->text_input_logged_out_register_password);
    }
    if(app->text_input_logged_out_register_password_2) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedOutRegisterPassword2Input);
        text_input_free(app->text_input_logged_out_register_password_2);
    }
    if(app->text_input_logged_in_change_password) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInChangePasswordInput);
        text_input_free(app->text_input_logged_in_change_password);
    }
    if(app->text_input_logged_in_compose_pre_save_input) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInComposeAddPreSaveInput);
        text_input_free(app->text_input_logged_in_compose_pre_save_input);
    }
    if(app->text_input_logged_in_wifi_settings_ssid) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInWifiSettingsSSIDInput);
        text_input_free(app->text_input_logged_in_wifi_settings_ssid);
    }
    if(app->text_input_logged_in_wifi_settings_password) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInWifiSettingsPasswordInput);
        text_input_free(app->text_input_logged_in_wifi_settings_password);
    }
    if(app->text_input_logged_in_messages_new_message) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInMessagesNewMessageInput);
        text_input_free(app->text_input_logged_in_messages_new_message);
    }
    if(app->text_input_logged_in_messages_new_message_user_choices) {
        view_dispatcher_remove_view(
            app->view_dispatcher, FlipSocialViewLoggedInMessagesNewMessageUserChoicesInput);
        text_input_free(app->text_input_logged_in_messages_new_message_user_choices);
    }

    // Free Widget(s)
    if(app->widget_logged_out_about) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutAbout);
        widget_free(app->widget_logged_out_about);
    }
    if(app->widget_logged_in_about) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInSettingsAbout);
        widget_free(app->widget_logged_in_about);
    }

    // Free View(s)
    if(app->view_process_login) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutProcessLogin);
        view_free(app->view_process_login);
    }
    if(app->view_process_register) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedOutProcessRegister);
        view_free(app->view_process_register);
    }
    if(app->view_process_feed) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInFeed);
        view_free(app->view_process_feed);
    }
    if(app->view_process_compose) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInProcessCompose);
        view_free(app->view_process_compose);
    }
    if(app->view_process_explore) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInExploreProccess);
        view_free(app->view_process_explore);
    }
    if(app->view_process_friends) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInFriendsProcess);
        view_free(app->view_process_friends);
    }
    if(app->view_process_messages) {
        view_dispatcher_remove_view(app->view_dispatcher, FlipSocialViewLoggedInMessagesProcess);
        view_free(app->view_process_messages);
    }

    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);

    // Free the app structure members
    if(app->wifi_ssid_logged_out) free(app->wifi_ssid_logged_out);
    if(app->wifi_ssid_logged_out_temp_buffer) free(app->wifi_ssid_logged_out_temp_buffer);
    if(app->wifi_password_logged_out) free(app->wifi_password_logged_out);
    if(app->wifi_password_logged_out_temp_buffer) free(app->wifi_password_logged_out_temp_buffer);
    if(app->login_username_logged_out) free(app->login_username_logged_out);
    if(app->login_username_logged_out_temp_buffer)
        free(app->login_username_logged_out_temp_buffer);
    if(app->login_password_logged_out) free(app->login_password_logged_out);
    if(app->login_password_logged_out_temp_buffer)
        free(app->login_password_logged_out_temp_buffer);
    if(app->register_username_logged_out) free(app->register_username_logged_out);
    if(app->register_username_logged_out_temp_buffer)
        free(app->register_username_logged_out_temp_buffer);
    if(app->register_password_logged_out) free(app->register_password_logged_out);
    if(app->register_password_logged_out_temp_buffer)
        free(app->register_password_logged_out_temp_buffer);
    if(app->register_password_2_logged_out) free(app->register_password_2_logged_out);
    if(app->register_password_2_logged_out_temp_buffer)
        free(app->register_password_2_logged_out_temp_buffer);
    if(app->change_password_logged_in) free(app->change_password_logged_in);
    if(app->change_password_logged_in_temp_buffer)
        free(app->change_password_logged_in_temp_buffer);
    if(app->compose_pre_save_logged_in) free(app->compose_pre_save_logged_in);
    if(app->compose_pre_save_logged_in_temp_buffer)
        free(app->compose_pre_save_logged_in_temp_buffer);
    if(app->wifi_ssid_logged_in) free(app->wifi_ssid_logged_in);
    if(app->wifi_ssid_logged_in_temp_buffer) free(app->wifi_ssid_logged_in_temp_buffer);
    if(app->wifi_password_logged_in) free(app->wifi_password_logged_in);
    if(app->wifi_password_logged_in_temp_buffer) free(app->wifi_password_logged_in_temp_buffer);
    if(app->is_logged_in) free(app->is_logged_in);
    if(app->login_username_logged_in) free(app->login_username_logged_in);
    if(app->login_username_logged_in_temp_buffer) free(app->login_username_logged_in_temp_buffer);
    if(app->messages_new_message_logged_in) free(app->messages_new_message_logged_in);
    if(app->messages_new_message_logged_in_temp_buffer)
        free(app->messages_new_message_logged_in_temp_buffer);
    if(app->message_user_choice_logged_in) free(app->message_user_choice_logged_in);
    if(app->message_user_choice_logged_in_temp_buffer)
        free(app->message_user_choice_logged_in_temp_buffer);

    if(app->input_event && app->input_event_queue)
        furi_pubsub_unsubscribe(app->input_event_queue, app->input_event);

    // free received_data
    if(fhttp.received_data) free(fhttp.received_data);

    // free playlist and explore page
    flip_social_free_explore();
    flip_social_free_feed();
    flip_social_free_friends();
    flip_social_free_message_users();
    flip_social_free_messages();

    // DeInit UART
    flipper_http_deinit();

    // Free the app structure
    if(app_instance) free(app_instance);
}

#endif // FLIP_SOCIAL_FREE_H
