#ifndef FLIP_SOCIAL_MESSAGES_H
#define FLIP_SOCIAL_MESSAGES_H

static FlipSocialModel2* flip_social_messages_alloc() {
    // Allocate memory for each username only if not already allocated
    FlipSocialModel2* users = malloc(sizeof(FlipSocialModel2));
    if(users == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for message users");
        return NULL;
    }
    for(size_t i = 0; i < MAX_MESSAGE_USERS; i++) {
        if(users->usernames[i] == NULL) {
            users->usernames[i] = malloc(MAX_USER_LENGTH);
            if(users->usernames[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for username %zu", i);
                return NULL; // Return false on memory allocation failure
            }
        }
    }
    return users;
}

static FlipSocialMessage* flip_social_user_messages_alloc() {
    // Allocate memory for each username only if not already allocated
    FlipSocialMessage* messages = malloc(sizeof(FlipSocialMessage));
    if(messages == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for messages");
        return NULL;
    }
    for(size_t i = 0; i < MAX_MESSAGE_USERS; i++) {
        if(messages->usernames[i] == NULL) {
            messages->usernames[i] = malloc(MAX_USER_LENGTH);
            if(messages->usernames[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for username %zu", i);
                return NULL; // Return false on memory allocation failure
            }
        }
        if(messages->messages[i] == NULL) {
            messages->messages[i] = malloc(MAX_MESSAGE_LENGTH);
            if(messages->messages[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for message %zu", i);
                return NULL; // Return false on memory allocation failure
            }
        }
    }
    return messages;
}

static void flip_social_free_message_users() {
    if(flip_social_message_users == NULL) {
        FURI_LOG_E(TAG, "Message users model is NULL");
        return;
    }
    for(int i = 0; i < flip_social_message_users->count; i++) {
        free(flip_social_message_users->usernames[i]);
    }
}

static void flip_social_free_messages() {
    if(flip_social_messages == NULL) {
        FURI_LOG_E(TAG, "Messages model is NULL");
        return;
    }
    for(int i = 0; i < flip_social_messages->count; i++) {
        free(flip_social_messages->usernames[i]);
        free(flip_social_messages->messages[i]);
    }
}

static bool flip_social_update_messages_submenu() {
    if(app_instance->submenu_messages == NULL) {
        FURI_LOG_E(TAG, "Submenu is NULL");
        return false;
    }
    if(flip_social_message_users == NULL) {
        FURI_LOG_E(TAG, "Message users model is NULL");
        return false;
    }
    submenu_reset(app_instance->submenu_messages);
    submenu_set_header(app_instance->submenu_messages, "Messages");
    submenu_add_item(
        app_instance->submenu_messages,
        "[New Message]",
        FlipSocialSubmenuLoggedInIndexMessagesNewMessage,
        flip_social_callback_submenu_choices,
        app_instance);
    for(int i = 0; i < flip_social_message_users->count; i++) {
        submenu_add_item(
            app_instance->submenu_messages,
            flip_social_message_users->usernames[i],
            FlipSocialSubmenuLoggedInIndexMessagesUsersStart + i,
            flip_social_callback_submenu_choices,
            app_instance);
    }
    return true;
}

static bool flip_social_update_submenu_user_choices() {
    if(app_instance->submenu_messages_user_choices == NULL) {
        FURI_LOG_E(TAG, "Submenu is NULL");
        return false;
    }
    if(flip_social_explore == NULL) {
        FURI_LOG_E(TAG, "Explore model is NULL");
        return false;
    }
    submenu_reset(app_instance->submenu_messages_user_choices);
    submenu_set_header(app_instance->submenu_messages_user_choices, "Users");
    for(int i = 0; i < flip_social_explore->count; i++) {
        submenu_add_item(
            app_instance->submenu_messages_user_choices,
            flip_social_explore->usernames[i],
            FlipSocialSubmenuLoggedInIndexMessagesUserChoicesIndexStart + i,
            flip_social_callback_submenu_choices,
            app_instance);
    }
    return true;
}

// Get all the users that have sent messages to the logged in user
static bool flip_social_get_message_users() {
    if(app_instance->login_username_logged_out == NULL) {
        FURI_LOG_E(TAG, "Username is NULL");
        return false;
    }
    char command[128];
    snprintf(
        command,
        128,
        "https://www.flipsocial.net/api/messages/%s/get/list/",
        app_instance->login_username_logged_out);
    bool success =
        flipper_http_get_request_with_headers(command, "{\"Content-Type\":\"application/json\"}");
    if(!success) {
        FURI_LOG_E(TAG, "Failed to send HTTP request for messages");
        return false;
    }
    fhttp.state = RECEIVING;
    return true;
}

// Get all the messages between the logged in user and the selected user
static bool flip_social_get_messages_with_user() {
    if(app_instance->login_username_logged_out == NULL) {
        FURI_LOG_E(TAG, "Username is NULL");
        return false;
    }
    char command[128];
    snprintf(
        command,
        128,
        "https://www.flipsocial.net/api/messages/%s/get/%s/",
        app_instance->login_username_logged_out,
        flip_social_message_users->usernames[flip_social_message_users->index]);
    bool success =
        flipper_http_get_request_with_headers(command, "{\"Content-Type\":\"application/json\"}");
    if(!success) {
        FURI_LOG_E(TAG, "Failed to send HTTP request for messages");
        return false;
    }
    fhttp.state = RECEIVING;
    return true;
}

// Parse the users that have sent messages to the logged-in user
static bool flip_social_parse_json_message_users() {
    if(fhttp.received_data == NULL) {
        FURI_LOG_E(TAG, "No data received.");
        return false;
    }

    // Allocate memory for each username only if not already allocated
    flip_social_message_users = flip_social_messages_alloc();
    if(flip_social_message_users == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for message users.");
        return false;
    }

    // Remove newlines
    char* pos = fhttp.received_data;
    while((pos = strchr(pos, '\n')) != NULL) {
        *pos = ' ';
    }

    // Initialize message users count
    flip_social_message_users->count = 0;

    // Extract the users array from the JSON
    char* json_users = get_json_value("users", fhttp.received_data, MAX_TOKENS);
    if(json_users == NULL) {
        FURI_LOG_E(TAG, "Failed to parse users array.");
        return false;
    }

    // Manual tokenization for comma-separated values
    char* start = json_users + 1; // Skip the opening bracket
    char* end;
    while((end = strchr(start, ',')) != NULL &&
          flip_social_message_users->count < MAX_MESSAGE_USERS) {
        *end = '\0'; // Null-terminate the current token

        // Remove quotes
        if(*start == '"') start++;
        if(*(end - 1) == '"') *(end - 1) = '\0';

        // Copy username to pre-allocated memory
        strncpy(
            flip_social_message_users->usernames[flip_social_message_users->count],
            start,
            MAX_USER_LENGTH - 1);
        flip_social_message_users
            ->usernames[flip_social_message_users->count][MAX_USER_LENGTH - 1] =
            '\0'; // Ensure null termination
        flip_social_message_users->count++;
        start = end + 1;
    }

    // Handle the last token
    if(*start != '\0' && flip_social_message_users->count < MAX_MESSAGE_USERS) {
        if(*start == '"') start++;
        if(*(start + strlen(start) - 1) == ']') *(start + strlen(start) - 1) = '\0';
        if(*(start + strlen(start) - 1) == '"') *(start + strlen(start) - 1) = '\0';

        strncpy(
            flip_social_message_users->usernames[flip_social_message_users->count],
            start,
            MAX_USER_LENGTH - 1);
        flip_social_message_users
            ->usernames[flip_social_message_users->count][MAX_USER_LENGTH - 1] =
            '\0'; // Ensure null termination
        flip_social_message_users->count++;
    }

    // Add submenu items for the users
    flip_social_update_messages_submenu();

    // Free the JSON data
    free(json_users);
    free(start);
    free(end);

    return true;
}

// Parse the users that the logged in user can message
static bool flip_social_parse_json_message_user_choices() {
    if(fhttp.received_data == NULL) {
        FURI_LOG_E(TAG, "No data received.");
        return false;
    }

    // Allocate memory for each username only if not already allocated
    flip_social_explore = flip_social_explore_alloc();
    if(flip_social_explore == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for explore usernames.");
        return false;
    }

    // Remove newlines
    char* pos = fhttp.received_data;
    while((pos = strchr(pos, '\n')) != NULL) {
        *pos = ' ';
    }

    // Initialize explore count
    flip_social_explore->count = 0;

    // Extract the users array from the JSON
    char* json_users = get_json_value("users", fhttp.received_data, MAX_TOKENS);
    if(json_users == NULL) {
        FURI_LOG_E(TAG, "Failed to parse users array.");
        return false;
    }

    // Manual tokenization for comma-separated values
    char* start = json_users + 1; // Skip the opening bracket
    char* end;
    while((end = strchr(start, ',')) != NULL && flip_social_explore->count < MAX_EXPLORE_USERS) {
        *end = '\0'; // Null-terminate the current token

        // Remove quotes
        if(*start == '"') start++;
        if(*(end - 1) == '"') *(end - 1) = '\0';

        // Copy username to pre-allocated memory
        strncpy(
            flip_social_explore->usernames[flip_social_explore->count],
            start,
            MAX_USER_LENGTH - 1);
        flip_social_explore->usernames[flip_social_explore->count][MAX_USER_LENGTH - 1] =
            '\0'; // Ensure null termination
        flip_social_explore->count++;
        start = end + 1;
    }

    // Handle the last token
    if(*start != '\0' && flip_social_explore->count < MAX_EXPLORE_USERS) {
        if(*start == '"') start++;
        if(*(start + strlen(start) - 1) == ']') *(start + strlen(start) - 1) = '\0';
        if(*(start + strlen(start) - 1) == '"') *(start + strlen(start) - 1) = '\0';

        strncpy(
            flip_social_explore->usernames[flip_social_explore->count],
            start,
            MAX_USER_LENGTH - 1);
        flip_social_explore->usernames[flip_social_explore->count][MAX_USER_LENGTH - 1] =
            '\0'; // Ensure null termination
        flip_social_explore->count++;
    }

    // Add submenu items for the users
    flip_social_update_submenu_user_choices();

    // Free the JSON data
    free(json_users);
    free(start);
    free(end);

    return true;
}

// parse messages between the logged in user and the selected user
static bool flip_social_parse_json_messages() {
    if(fhttp.received_data == NULL) {
        FURI_LOG_E(TAG, "No data received.");
        return false;
    }

    // Allocate memory for each message only if not already allocated
    flip_social_messages = flip_social_user_messages_alloc();
    if(!flip_social_messages) {
        FURI_LOG_E(TAG, "Failed to allocate memory for messages.");
        return false;
    }

    // Remove newlines
    char* pos = fhttp.received_data;
    while((pos = strchr(pos, '\n')) != NULL) {
        *pos = ' ';
    }

    // Initialize messages count
    flip_social_messages->count = 0;

    // Iterate through the messages array
    for(int i = 0; i < MAX_MESSAGES; i++) {
        // Parse each item in the array
        char* item = get_json_array_value("conversations", i, fhttp.received_data, MAX_TOKENS);
        if(item == NULL) {
            break;
        }

        // Extract individual fields from the JSON object
        char* sender = get_json_value("sender", item, MAX_TOKENS);
        char* content = get_json_value("content", item, MAX_TOKENS);

        if(sender == NULL || content == NULL) {
            FURI_LOG_E(TAG, "Failed to parse item fields.");
            free(item);
            continue;
        }

        // Store parsed values
        strncpy(flip_social_messages->usernames[i], sender, MAX_USER_LENGTH - 1);
        flip_social_messages->usernames[i][MAX_USER_LENGTH - 1] = '\0';
        strncpy(flip_social_messages->messages[i], content, MAX_MESSAGE_LENGTH - 1);
        flip_social_messages->messages[i][MAX_MESSAGE_LENGTH - 1] = '\0';
        flip_social_messages->count++;

        free(item);
        free(sender);
        free(content);
    }

    return true;
}

#endif // FLIP_SOCIAL_MESSAGES_H
