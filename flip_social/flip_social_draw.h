#ifndef FLIP_SOCIAL_DRAW_H
#define FLIP_SOCIAL_DRAW_H

bool flip_social_sent_login_request = false;
bool flip_social_sent_register_request = false;
bool flip_social_login_success = false;
bool flip_social_register_success = false;
bool flip_social_dialog_shown = false;
bool flip_social_dialog_stop = false;
char* last_explore_response = "";
static bool flip_social_update_friends();

bool flip_social_board_is_active(Canvas* canvas) {
    if(fhttp.state == INACTIVE) {
        canvas_draw_str(canvas, 0, 7, "Wifi Dev Board disconnected.");
        canvas_draw_str(canvas, 0, 17, "Please connect to the board.");
        canvas_draw_str(canvas, 0, 32, "If your board is connected,");
        canvas_draw_str(canvas, 0, 42, "make sure you have flashed");
        canvas_draw_str(canvas, 0, 52, "your WiFi Devboard with the");
        canvas_draw_str(canvas, 0, 62, "latest FlipperHTTP flash.");
        return false;
    }
    return true;
}

void flip_social_handle_error(Canvas* canvas) {
    if(fhttp.received_data != NULL) {
        if(strstr(fhttp.received_data, "[ERROR] Not connected to Wifi. Failed to reconnect.") !=
           NULL) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
            canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        } else if(strstr(fhttp.received_data, "[ERROR] Failed to connect to Wifi.") != NULL) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "[ERROR] Not connected to Wifi.");
            canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
        } else {
            canvas_draw_str(canvas, 0, 42, "Failed...");
            canvas_draw_str(canvas, 0, 52, "Update your credentials.");
            canvas_draw_str(canvas, 0, 62, "Press BACK to return.");
        }
    } else {
        canvas_draw_str(canvas, 0, 42, "Failed...");
        canvas_draw_str(canvas, 0, 52, "Update your credentials.");
        canvas_draw_str(canvas, 0, 62, "Press BACK to return.");
    }
}

static void on_input(const void* event, void* ctx) {
    UNUSED(ctx);

    InputKey key = ((InputEvent*)event)->key;
    InputType type = ((InputEvent*)event)->type;

    if(type != InputTypeRelease) {
        return;
    }

    switch(key) {
    case InputKeyOk:
        action = ActionFlip;
        break;
    case InputKeyBack:
        action = ActionBack;
        break;
    case InputKeyRight:
        action = ActionNext;
        break;
    case InputKeyLeft:
        action = ActionPrev;
        break;
    case InputKeyUp:
        action = ActionPrev;
        break;
    case InputKeyDown:
        action = ActionNext;
        break;
    default:
        action = ActionNone;
        break;
    }
}

// Function to draw the message on the canvas with word wrapping
void draw_user_message(Canvas* canvas, const char* user_message, int x, int y) {
    if(user_message == NULL) {
        FURI_LOG_E(TAG, "User message is NULL.");
        return;
    }

    size_t msg_length = strlen(user_message);
    size_t start = 0;
    int line_num = 0;
    char line[MAX_LINE_LENGTH + 1]; // Buffer for the current line (+1 for null terminator)

    while(start < msg_length && line_num < 4) {
        size_t remaining = msg_length - start;
        size_t len = (remaining > MAX_LINE_LENGTH) ? MAX_LINE_LENGTH : remaining;

        if(remaining > MAX_LINE_LENGTH) {
            // Find the last space within the first 'len' characters
            size_t last_space = len;
            while(last_space > 0 && user_message[start + last_space - 1] != ' ') {
                last_space--;
            }

            if(last_space > 0) {
                len = last_space; // Adjust len to the position of the last space
            }
        }

        // Copy the substring to 'line' and null-terminate it
        memcpy(line, user_message + start, len);
        line[len] = '\0'; // Ensure the string is null-terminated

        // Draw the string on the canvas
        // Adjust the y-coordinate based on the line number
        canvas_draw_str_aligned(canvas, x, y + line_num * 10, AlignLeft, AlignTop, line);

        // Update the start position for the next line
        start += len;

        // Skip any spaces to avoid leading spaces on the next line
        while(start < msg_length && user_message[start] == ' ') {
            start++;
        }

        // Increment the line number
        line_num++;
    }
}

static void flip_social_callback_draw_compose(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }
    if(!app_instance) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    char* message = app_instance->pre_saved_messages.messages[flip_social_feed->index];

    if(!flip_social_dialog_shown) {
        flip_social_dialog_shown = true;
        app_instance->input_event_queue = furi_record_open(RECORD_INPUT_EVENTS);
        app_instance->input_event =
            furi_pubsub_subscribe(app_instance->input_event_queue, on_input, NULL);
    }

    draw_user_message(canvas, message, 0, 2);

    canvas_draw_icon(canvas, 0, 53, &I_ButtonLeft_4x7);
    canvas_draw_str_aligned(canvas, 7, 54, AlignLeft, AlignTop, "Delete");
    canvas_draw_icon(canvas, 52, 53, &I_ButtonBACK_10x8);
    canvas_draw_str_aligned(canvas, 64, 54, AlignLeft, AlignTop, "Back");
    canvas_draw_icon(canvas, 100, 53, &I_ButtonRight_4x7);
    canvas_draw_str_aligned(canvas, 107, 54, AlignLeft, AlignTop, "Post");

    // handle action
    switch(action) {
    case ActionNone:
        break;
    case ActionBack:
        flip_social_dialog_stop = true;
        break;
    case ActionNext:
        // send message
        if(message && app_instance->login_username_logged_in) {
            // Send the message
            char command[128];
            snprintf(
                command,
                sizeof(command),
                "{\"username\":\"%s\",\"content\":\"%s\"}",
                app_instance->login_username_logged_in,
                message);

            bool success = flipper_http_post_request_with_headers(
                "https://www.flipsocial.net/api/feed/post/",
                "{\"Content-Type\":\"application/json\"}",
                command);

            if(!success) {
                FURI_LOG_E(TAG, "Failed to send HTTP request for feed");
                furi_check(success); // Log the error with furi_check
                return; // Exit early to avoid further errors
            }

            fhttp.state = RECEIVING;
            furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        } else {
            FURI_LOG_E(TAG, "Message or username is NULL");
            furi_check(false); // Log as an error and return
            return;
        }

        int i = 0;
        while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
            // Wait for the feed to be received
            furi_delay_ms(100);

            char dots_str[64] = "Receiving";

            // Append dots to the string based on the value of i
            int dot_count = i % 4;
            int len = strlen(dots_str);
            snprintf(dots_str + len, sizeof(dots_str) - len, "%.*s", dot_count, "....");

            // Draw the resulting string on the canvas
            canvas_draw_str(canvas, 0, 30, dots_str);

            i++;
        }
        flip_social_dialog_stop = true;
        furi_timer_stop(fhttp.get_timeout_timer);
        break;
    case ActionPrev:
        // delete message
        app_instance->pre_saved_messages.messages[flip_social_feed->index] = NULL;

        for(uint32_t i = flip_social_feed->index; i < app_instance->pre_saved_messages.count - 1;
            i++) {
            app_instance->pre_saved_messages.messages[i] =
                app_instance->pre_saved_messages.messages[i + 1];
        }
        app_instance->pre_saved_messages.count--;

        // add the item to the submenu
        submenu_reset(app_instance->submenu_compose);

        submenu_add_item(
            app_instance->submenu_compose,
            "Add Pre-Save",
            FlipSocialSubmenuComposeIndexAddPreSave,
            flip_social_callback_submenu_choices,
            app_instance);

        for(uint32_t i = 0; i < app_instance->pre_saved_messages.count; i++) {
            submenu_add_item(
                app_instance->submenu_compose,
                app_instance->pre_saved_messages.messages[i],
                FlipSocialSubemnuComposeIndexStartIndex + i,
                flip_social_callback_submenu_choices,
                app_instance);
        }

        // save playlist
        save_playlist(&app_instance->pre_saved_messages);

        flip_social_dialog_stop = true;
        break;
    default:
        action = ActionNone;
        break;
    }

    if(flip_social_dialog_stop) {
        furi_pubsub_unsubscribe(app_instance->input_event_queue, app_instance->input_event);
        flip_social_dialog_shown = false;
        flip_social_dialog_stop = false;
        if(action == ActionNext) {
            action = ActionNone;
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipSocialViewLoggedInFeed);
        } else if(action == ActionBack) {
            action = ActionNone;
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipSocialViewLoggedInSubmenu);
        } else {
            action = ActionNone;
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipSocialViewLoggedInCompose);
        }
    }
}
// function to draw the dialog canvas
static void flip_social_canvas_draw_message(
    Canvas* canvas,
    char* user_username,
    char* user_message,
    bool is_flipped,
    bool show_prev,
    bool show_next,
    int flip_count) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, user_username);
    canvas_set_font(canvas, FontSecondary);

    char flip_count_str[12];
    if(flip_count == 1) {
        snprintf(flip_count_str, sizeof(flip_count_str), "%d Flip", flip_count);
        canvas_draw_str_aligned(canvas, 106, 54, AlignLeft, AlignTop, flip_count_str);
    } else {
        snprintf(flip_count_str, sizeof(flip_count_str), "%d Flips", flip_count);

        if(flip_count < 10) {
            canvas_draw_str_aligned(canvas, 100, 54, AlignLeft, AlignTop, flip_count_str);
        } else if(flip_count < 100) {
            canvas_draw_str_aligned(canvas, 94, 54, AlignLeft, AlignTop, flip_count_str);
        } else {
            canvas_draw_str_aligned(canvas, 88, 54, AlignLeft, AlignTop, flip_count_str);
        }
    }

    draw_user_message(canvas, user_message, 0, 12);

    // combine and shift icons/labels around if not show_prev or show_next
    if(show_prev && show_next && !is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonLeft_4x7);
        canvas_draw_str_aligned(canvas, 6, 54, AlignLeft, AlignTop, "Prev");
        canvas_draw_icon(canvas, 30, 54, &I_ButtonRight_4x7);
        canvas_draw_str_aligned(canvas, 36, 54, AlignLeft, AlignTop, "Next");
        canvas_draw_icon(canvas, 58, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 67, 54, AlignLeft, AlignTop, "Flip");
    } else if(show_prev && !show_next && !is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonLeft_4x7);
        canvas_draw_str_aligned(canvas, 6, 54, AlignLeft, AlignTop, "Prev");
        canvas_draw_icon(canvas, 28, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 37, 54, AlignLeft, AlignTop, "Flip");
    } else if(!show_prev && show_next && !is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonRight_4x7);
        canvas_draw_str_aligned(canvas, 6, 54, AlignLeft, AlignTop, "Next");
        canvas_draw_icon(canvas, 28, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 37, 54, AlignLeft, AlignTop, "Flip");
    } else if(show_prev && show_next && is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonLeft_4x7);
        canvas_draw_str_aligned(canvas, 6, 54, AlignLeft, AlignTop, "Prev");
        canvas_draw_icon(canvas, 28, 54, &I_ButtonRight_4x7);
        canvas_draw_str_aligned(canvas, 34, 54, AlignLeft, AlignTop, "Next");
        canvas_draw_icon(canvas, 54, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 63, 54, AlignLeft, AlignTop, "UnFlip");
    } else if(show_prev && !show_next && is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonLeft_4x7);
        canvas_draw_str_aligned(canvas, 6, 54, AlignLeft, AlignTop, "Prev");
        canvas_draw_icon(canvas, 28, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 37, 54, AlignLeft, AlignTop, "UnFlip");
    } else if(!show_prev && show_next && is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonRight_4x7);
        canvas_draw_str_aligned(canvas, 6, 54, AlignLeft, AlignTop, "Next");
        canvas_draw_icon(canvas, 28, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 37, 54, AlignLeft, AlignTop, "UnFlip");
    } else if(!show_prev && !show_next && is_flipped) {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 9, 54, AlignLeft, AlignTop, "UnFlip");
    } else {
        canvas_draw_icon(canvas, 0, 54, &I_ButtonOK_7x7);
        canvas_draw_str_aligned(canvas, 9, 54, AlignLeft, AlignTop, "Flip");
    }
}
// Callback function to handle the feed dialog
static void flip_social_callback_draw_feed(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }
    if(!app_instance) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    if(!flip_social_dialog_shown) {
        flip_social_dialog_shown = true;
        app_instance->input_event_queue = furi_record_open(RECORD_INPUT_EVENTS);
        app_instance->input_event =
            furi_pubsub_subscribe(app_instance->input_event_queue, on_input, NULL);
    }

    // handle action
    switch(action) {
    case ActionNone:
        flip_social_canvas_draw_message(
            canvas,
            flip_social_feed->usernames[flip_social_feed->index],
            flip_social_feed->messages[flip_social_feed->index],
            flip_social_feed->is_flipped[flip_social_feed->index],
            flip_social_feed->index > 0,
            flip_social_feed->index < flip_social_feed->count - 1,
            flip_social_feed->flips[flip_social_feed->index]);
        break;
    case ActionNext:
        canvas_clear(canvas);
        if(flip_social_feed->index < flip_social_feed->count - 1) {
            flip_social_feed->index++;
        }
        flip_social_canvas_draw_message(
            canvas,
            flip_social_feed->usernames[flip_social_feed->index],
            flip_social_feed->messages[flip_social_feed->index],
            flip_social_feed->is_flipped[flip_social_feed->index],
            flip_social_feed->index > 0,
            flip_social_feed->index < flip_social_feed->count - 1,
            flip_social_feed->flips[flip_social_feed->index]);
        action = ActionNone;
        break;
    case ActionPrev:
        canvas_clear(canvas);
        if(flip_social_feed->index > 0) {
            flip_social_feed->index--;
        }
        flip_social_canvas_draw_message(
            canvas,
            flip_social_feed->usernames[flip_social_feed->index],
            flip_social_feed->messages[flip_social_feed->index],
            flip_social_feed->is_flipped[flip_social_feed->index],
            flip_social_feed->index > 0,
            flip_social_feed->index < flip_social_feed->count - 1,
            flip_social_feed->flips[flip_social_feed->index]);
        action = ActionNone;
        break;
    case ActionFlip:
        canvas_clear(canvas);
        // Moved to above the is_flipped check
        if(!flip_social_feed->is_flipped[flip_social_feed->index]) {
            // increase the flip count
            flip_social_feed->flips[flip_social_feed->index]++;
        } else {
            // decrease the flip count
            flip_social_feed->flips[flip_social_feed->index]--;
        }
        // change the flip status
        flip_social_feed->is_flipped[flip_social_feed->index] =
            !flip_social_feed->is_flipped[flip_social_feed->index];
        // send post request to flip the message
        if(app_instance->login_username_logged_in == NULL) {
            FURI_LOG_E(TAG, "Username is NULL");
            return;
        }
        char payload[256];
        snprintf(
            payload,
            sizeof(payload),
            "{\"username\":\"%s\",\"post_id\":\"%u\"}",
            app_instance->login_username_logged_in,
            flip_social_feed->ids[flip_social_feed->index]);
        flipper_http_post_request_with_headers(
            "https://www.flipsocial.net/api/feed/flip/",
            "{\"Content-Type\":\"application/json\"}",
            payload);
        flip_social_canvas_draw_message(
            canvas,
            flip_social_feed->usernames[flip_social_feed->index],
            flip_social_feed->messages[flip_social_feed->index],
            flip_social_feed->is_flipped[flip_social_feed->index],
            flip_social_feed->index > 0,
            flip_social_feed->index < flip_social_feed->count - 1,
            flip_social_feed->flips[flip_social_feed->index]);
        action = ActionNone;
        break;
    case ActionBack:
        canvas_clear(canvas);
        flip_social_dialog_stop = true;
        flip_social_feed->index = 0;
        action = ActionNone;
        break;
    default:
        break;
    }

    if(flip_social_dialog_stop) {
        furi_pubsub_unsubscribe(app_instance->input_event_queue, app_instance->input_event);
        flip_social_dialog_shown = false;
        flip_social_dialog_stop = false;
        action = ActionNone;
    }
}
/**
 * @brief Navigation callback for asynchonously handling the login process.
 * @param canvas The canvas to draw on.
 * @param model The model - unused
 * @return void
 */
static void flip_social_callback_draw_login(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }

    canvas_set_font(canvas, FontSecondary);

    if(!flip_social_board_is_active(canvas)) {
        return;
    }

    canvas_draw_str(canvas, 0, 7, "Logging in...");

    // Perform login request
    if(!flip_social_sent_login_request) {
        if(!app_instance->login_username_logged_out || !app_instance->login_password_logged_out ||
           strlen(app_instance->login_username_logged_out) == 0 ||
           strlen(app_instance->login_password_logged_out) == 0) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "Please enter your credentials.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            return;
        }

        flip_social_sent_login_request = true;

        char buffer[256];
        snprintf(
            buffer,
            sizeof(buffer),
            "{\"username\":\"%s\",\"password\":\"%s\"}",
            app_instance->login_username_logged_out,
            app_instance->login_password_logged_out);
        flip_social_login_success = flipper_http_post_request_with_headers(
            "https://www.flipsocial.net/api/user/login/",
            "{\"Content-Type\":\"application/json\"}",
            buffer);
        if(flip_social_login_success) {
            fhttp.state = RECEIVING;
            return;
        } else {
            fhttp.state = ISSUE;
            return;
        }
    }
    // handle response
    if(flip_social_sent_login_request && flip_social_login_success) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 0, 17, "Request Sent!");
        canvas_draw_str(canvas, 0, 32, "Awaiting reponse...");

        if(fhttp.state == IDLE && fhttp.received_data != NULL) {
            // read response
            if(strstr(fhttp.received_data, "[SUCCESS]") != NULL ||
               strstr(fhttp.received_data, "User found") != NULL) {
                canvas_draw_str(canvas, 0, 42, "Login successful!");
                canvas_draw_str(canvas, 0, 62, "Welcome back!");

                app_instance->is_logged_in = "true";

                // set the logged_in_username and change_password_logged_in
                if(app_instance->login_username_logged_out) {
                    strcpy(
                        app_instance->login_username_logged_in,
                        app_instance->login_username_logged_out);
                }
                if(app_instance->login_password_logged_out) {
                    app_instance->change_password_logged_in =
                        app_instance->login_password_logged_out;
                }

                save_settings(
                    app_instance->wifi_ssid_logged_out,
                    app_instance->wifi_password_logged_out,
                    app_instance->login_username_logged_out,
                    app_instance->login_username_logged_in,
                    app_instance->login_password_logged_out,
                    app_instance->change_password_logged_in,
                    app_instance->is_logged_in);

                // send user to the logged in submenu
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipSocialViewLoggedInSubmenu);
            } else if(strstr(fhttp.received_data, "User not found") != NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "Account not found...");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else {
                flip_social_handle_error(canvas);
            }
        } else if((fhttp.state == ISSUE || fhttp.state == INACTIVE) && fhttp.received_data != NULL) {
            flip_social_handle_error(canvas);
        } else if(fhttp.state == IDLE && fhttp.received_data == NULL) {
            flip_social_handle_error(canvas);
        }
    } else if(flip_social_sent_login_request && !flip_social_login_success) {
        canvas_clear(canvas);
        canvas_draw_str(canvas, 0, 10, "Failed sending request.");
        canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
        canvas_draw_str(canvas, 0, 62, "Press BACK to return.");
    }
}

/**
 * @brief Navigation callback for asynchonously handling the register process.
 * @param canvas The canvas to draw on.
 * @param model The model - unused
 * @return void
 */
static void flip_social_callback_draw_register(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }

    canvas_set_font(canvas, FontSecondary);

    if(!flip_social_board_is_active(canvas)) {
        return;
    }

    canvas_draw_str(canvas, 0, 7, "Registering...");

    // Perform login request
    if(!flip_social_sent_register_request) {
        // check if the username and password are valid
        if(!app_instance->register_username_logged_out ||
           !app_instance->register_password_logged_out ||
           strlen(app_instance->register_username_logged_out) == 0 ||
           strlen(app_instance->register_password_logged_out) == 0) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "Please enter your credentials.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            return;
        }

        // check if both passwords match
        if(strcmp(
               app_instance->register_password_logged_out,
               app_instance->register_password_2_logged_out) != 0) {
            canvas_clear(canvas);
            canvas_draw_str(canvas, 0, 10, "Passwords do not match.");
            canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            return;
        }

        char buffer[128];
        snprintf(
            buffer,
            sizeof(buffer),
            "{\"username\":\"%s\",\"password\":\"%s\"}",
            app_instance->register_username_logged_out,
            app_instance->register_password_logged_out);
        flip_social_register_success = flipper_http_post_request_with_headers(
            "https://www.flipsocial.net/api/user/register/",
            "{\"Content-Type\":\"application/json\"}",
            buffer);

        flip_social_sent_register_request = true;
        if(flip_social_register_success) {
            // Set the state to RECEIVING to ensure we continue to see the receiving message
            fhttp.state = RECEIVING;
        } else {
            fhttp.state = ISSUE;
        }
    }
    // handle response
    if(flip_social_sent_register_request && flip_social_register_success) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 0, 17, "Request Sent!");
        canvas_draw_str(canvas, 0, 32, "Awaiting reponse...");

        if(fhttp.state == IDLE) {
            // read response
            if(fhttp.received_data != NULL &&
               (strstr(fhttp.received_data, "[SUCCESS]") != NULL ||
                strstr(fhttp.received_data, "User created") != NULL)) {
                canvas_draw_str(canvas, 0, 42, "Registeration successful!");
                canvas_draw_str(canvas, 0, 62, "Welcome to FlipSocial!");

                // set the login credentials
                if(app_instance->login_username_logged_out) {
                    app_instance->login_username_logged_out =
                        app_instance->register_username_logged_out;
                }
                if(app_instance->login_password_logged_out) {
                    app_instance->login_password_logged_out =
                        app_instance->register_password_logged_out;
                    app_instance->change_password_logged_in =
                        app_instance->register_password_logged_out;
                }
                if(app_instance->login_username_logged_in) {
                    app_instance->login_username_logged_in =
                        app_instance->register_username_logged_out;
                }

                app_instance->is_logged_in = "true";

                // save the credentials
                save_settings(
                    app_instance->wifi_ssid_logged_out,
                    app_instance->wifi_password_logged_out,
                    app_instance->login_username_logged_out,
                    app_instance->login_username_logged_in,
                    app_instance->login_password_logged_out,
                    app_instance->change_password_logged_in,
                    app_instance->is_logged_in);

                // send user to the logged in submenu
                view_dispatcher_switch_to_view(
                    app_instance->view_dispatcher, FlipSocialViewLoggedInSubmenu);
            } else if(strstr(fhttp.received_data, "Username or password not provided") != NULL) {
                canvas_clear(canvas);
                canvas_draw_str(canvas, 0, 10, "Please enter your credentials.");
                canvas_draw_str(canvas, 0, 60, "Press BACK to return.");
            } else if(
                strstr(fhttp.received_data, "User already exists") != NULL ||
                strstr(fhttp.received_data, "Multiple users found") != NULL) {
                canvas_draw_str(canvas, 0, 42, "Registration failed...");
                canvas_draw_str(canvas, 0, 52, "Username already exists.");
                canvas_draw_str(canvas, 0, 62, "Press BACK to return.");
            } else {
                canvas_draw_str(canvas, 0, 42, "Registration failed...");
                canvas_draw_str(canvas, 0, 52, "Update your credentials.");
                canvas_draw_str(canvas, 0, 62, "Press BACK to return.");
            }
        } else if(fhttp.state == ISSUE || fhttp.state == INACTIVE) {
            flip_social_handle_error(canvas);
        }
    } else if(flip_social_sent_register_request && !flip_social_register_success) {
        canvas_clear(canvas);
        canvas_draw_str(canvas, 0, 10, "Failed sending request.");
        canvas_draw_str(canvas, 0, 50, "Update your WiFi settings.");
        canvas_draw_str(canvas, 0, 62, "Press BACK to return.");
    }
}

// function to draw the dialog canvas
static void flip_social_canvas_draw_explore(Canvas* canvas, char* user_username, char* content) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, user_username);
    canvas_set_font(canvas, FontSecondary);

    draw_user_message(canvas, content, 0, 12);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 0, 53, &I_ButtonLeft_4x7);
    canvas_draw_str_aligned(canvas, 9, 54, AlignLeft, AlignTop, "Remove");
    canvas_draw_icon(canvas, 98, 53, &I_ButtonRight_4x7);
    canvas_draw_str_aligned(canvas, 107, 54, AlignLeft, AlignTop, "Add");

    if(strlen(content) > 0) {
        last_explore_response = content;
    }
}

// Callback function to handle the explore dialog
static void flip_social_callback_draw_explore(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }
    if(!app_instance) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    if(!flip_social_dialog_shown) {
        flip_social_dialog_shown = true;
        app_instance->input_event_queue = furi_record_open(RECORD_INPUT_EVENTS);
        app_instance->input_event =
            furi_pubsub_subscribe(app_instance->input_event_queue, on_input, NULL);
    }
    flip_social_canvas_draw_explore(
        canvas, flip_social_explore->usernames[flip_social_explore->index], last_explore_response);

    // handle action
    switch(action) {
    case ActionNext:
        // add friend
        char add_payload[128];
        snprintf(
            add_payload,
            sizeof(add_payload),
            "{\"username\":\"%s\",\"friend\":\"%s\"}",
            app_instance->login_username_logged_in,
            flip_social_explore->usernames[flip_social_explore->index]);
        flipper_http_post_request_with_headers(
            "https://www.flipsocial.net/api/user/add-friend/",
            "{\"Content-Type\":\"application/json\"}",
            add_payload);
        canvas_clear(canvas);
        flip_social_canvas_draw_explore(
            canvas, flip_social_explore->usernames[flip_social_explore->index], "Added!");
        action = ActionNone;
        break;
    case ActionPrev:
        // remove friend
        char remove_payload[128];
        snprintf(
            remove_payload,
            sizeof(remove_payload),
            "{\"username\":\"%s\",\"friend\":\"%s\"}",
            app_instance->login_username_logged_in,
            flip_social_explore->usernames[flip_social_explore->index]);
        flipper_http_post_request_with_headers(
            "https://www.flipsocial.net/api/user/remove-friend/",
            "{\"Content-Type\":\"application/json\"}",
            remove_payload);
        canvas_clear(canvas);
        flip_social_canvas_draw_explore(
            canvas, flip_social_explore->usernames[flip_social_explore->index], "Removed!");
        action = ActionNone;
        break;
    case ActionBack:
        canvas_clear(canvas);
        flip_social_dialog_stop = true;
        last_explore_response = "";
        flip_social_dialog_shown = false;
        flip_social_explore->index = 0;
        action = ActionNone;
        break;
    default:
        break;
    }

    if(flip_social_dialog_stop) {
        furi_pubsub_unsubscribe(app_instance->input_event_queue, app_instance->input_event);
        flip_social_dialog_shown = false;
        flip_social_dialog_stop = false;
        action = ActionNone;
    }
}

// Callback function to handle the friends dialog
static void flip_social_callback_draw_friends(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }
    if(!app_instance) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    if(!flip_social_dialog_shown) {
        flip_social_dialog_shown = true;
        app_instance->input_event_queue = furi_record_open(RECORD_INPUT_EVENTS);
        app_instance->input_event =
            furi_pubsub_subscribe(app_instance->input_event_queue, on_input, NULL);
    }
    flip_social_canvas_draw_explore(
        canvas, flip_social_friends->usernames[flip_social_friends->index], last_explore_response);

    // handle action
    switch(action) {
    case ActionNext:
        // add friend
        char add_payload[128];
        snprintf(
            add_payload,
            sizeof(add_payload),
            "{\"username\":\"%s\",\"friend\":\"%s\"}",
            app_instance->login_username_logged_in,
            flip_social_friends->usernames[flip_social_friends->index]);
        if(flipper_http_post_request_with_headers(
               "https://www.flipsocial.net/api/user/add-friend/",
               "{\"Content-Type\":\"application/json\"}",
               add_payload)) {
            canvas_clear(canvas);
            flip_social_canvas_draw_explore(
                canvas, flip_social_friends->usernames[flip_social_friends->index], "Added!");

            // add the friend to the friends list
            flip_social_friends->usernames[flip_social_friends->count] =
                flip_social_friends->usernames[flip_social_friends->index];
            flip_social_friends->count++;
            if(!flip_social_update_friends()) {
                FURI_LOG_E(TAG, "Failed to update friends");
            }
        }
        action = ActionNone;
        break;
    case ActionPrev:
        // remove friend
        char remove_payload[128];
        snprintf(
            remove_payload,
            sizeof(remove_payload),
            "{\"username\":\"%s\",\"friend\":\"%s\"}",
            app_instance->login_username_logged_in,
            flip_social_friends->usernames[flip_social_friends->index]);
        if(flipper_http_post_request_with_headers(
               "https://www.flipsocial.net/api/user/remove-friend/",
               "{\"Content-Type\":\"application/json\"}",
               remove_payload)) {
            canvas_clear(canvas);
            flip_social_canvas_draw_explore(
                canvas, flip_social_friends->usernames[flip_social_friends->index], "Removed!");

            // remove the friend from the friends list
            for(int i = flip_social_friends->index; i < flip_social_friends->count - 1; i++) {
                flip_social_friends->usernames[i] = flip_social_friends->usernames[i + 1];
            }
            flip_social_friends->count--;
            if(!flip_social_update_friends()) {
                FURI_LOG_E(TAG, "Failed to update friends");
            }
        }
        action = ActionNone;
        break;
    case ActionBack:
        canvas_clear(canvas);
        flip_social_dialog_stop = true;
        last_explore_response = "";
        flip_social_dialog_shown = false;
        flip_social_friends->index = 0;
        action = ActionNone;
        break;
    default:
        break;
    }

    if(flip_social_dialog_stop) {
        furi_pubsub_unsubscribe(app_instance->input_event_queue, app_instance->input_event);
        flip_social_dialog_shown = false;
        flip_social_dialog_stop = false;
        action = ActionNone;
    }
}

static void flip_social_canvas_draw_user_message(
    Canvas* canvas,
    char* user_username,
    char* user_message,
    bool show_prev,
    bool show_next) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, user_username);
    canvas_set_font(canvas, FontSecondary);

    draw_user_message(canvas, user_message, 0, 12);

    canvas_set_font(canvas, FontSecondary);
    if(show_prev) {
        canvas_draw_icon(canvas, 0, 53, &I_ButtonLeft_4x7);
        canvas_draw_str_aligned(canvas, 9, 54, AlignLeft, AlignTop, "Prev");
    }

    canvas_draw_icon(canvas, 47, 53, &I_ButtonOK_7x7);
    canvas_draw_str_aligned(canvas, 56, 54, AlignLeft, AlignTop, "Create");

    if(show_next) {
        canvas_draw_icon(canvas, 98, 53, &I_ButtonRight_4x7);
        canvas_draw_str_aligned(canvas, 107, 54, AlignLeft, AlignTop, "Next");
    }
}

// Callback function to handle the messages dialog
static void flip_social_callback_draw_messages(Canvas* canvas, void* model) {
    UNUSED(model);
    if(!canvas) {
        FURI_LOG_E(TAG, "Canvas is NULL");
        return;
    }
    if(!app_instance) {
        FURI_LOG_E(TAG, "FlipSocialApp is NULL");
        return;
    }

    if(!flip_social_dialog_shown) {
        flip_social_dialog_shown = true;
        app_instance->input_event_queue = furi_record_open(RECORD_INPUT_EVENTS);
        app_instance->input_event =
            furi_pubsub_subscribe(app_instance->input_event_queue, on_input, NULL);
    }

    // handle action
    switch(action) {
    case ActionNone:
        flip_social_canvas_draw_user_message(
            canvas,
            flip_social_messages->usernames[flip_social_messages->index],
            flip_social_messages->messages[flip_social_messages->index],
            flip_social_messages->index > 0,
            flip_social_messages->index < flip_social_messages->count - 1);
        action = ActionNone;
        break;
    case ActionNext:
        // view next message (if any)
        canvas_clear(canvas);
        if(flip_social_messages->index < flip_social_messages->count - 1) {
            flip_social_messages->index++;
        }
        flip_social_canvas_draw_user_message(
            canvas,
            flip_social_messages->usernames[flip_social_messages->index],
            flip_social_messages->messages[flip_social_messages->index],
            flip_social_messages->index > 0,
            flip_social_messages->index < flip_social_messages->count - 1);
        action = ActionNone;
        break;
    case ActionPrev:
        // view previous message (if any)
        canvas_clear(canvas);
        if(flip_social_messages->index > 0) {
            flip_social_messages->index--;
        }
        flip_social_canvas_draw_user_message(
            canvas,
            flip_social_messages->usernames[flip_social_messages->index],
            flip_social_messages->messages[flip_social_messages->index],
            flip_social_messages->index > 0,
            flip_social_messages->index < flip_social_messages->count - 1);
        action = ActionNone;
        break;
    case ActionBack:
        //  go back to the previous view
        flip_social_dialog_stop = true;
        action = ActionNone;
        break;
    case ActionFlip:
        // go to the input view
        flip_social_dialog_stop = true;
        break;
    default:
        action = ActionNone;
        break;
    }

    if(flip_social_dialog_stop) {
        furi_pubsub_unsubscribe(app_instance->input_event_queue, app_instance->input_event);
        flip_social_dialog_shown = false;
        flip_social_dialog_stop = false;
        if(action == ActionFlip) {
            action = ActionNone;
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipSocialViewLoggedInMessagesNewMessageInput);
        } else {
            action = ActionNone;
            view_dispatcher_switch_to_view(
                app_instance->view_dispatcher, FlipSocialViewLoggedInMessagesSubmenu);
        }
    }
}

#endif // FLIP_SOCIAL_DRAW_H
