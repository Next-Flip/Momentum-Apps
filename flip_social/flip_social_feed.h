#ifndef FLIP_SOCIAL_FEED_H
#define FLIP_SOCIAL_FEED_H

// Set failure FlipSocialFeed object
static bool flip_social_temp_feed() {
    if(flip_social_feed == NULL) {
        flip_social_feed = malloc(sizeof(FlipSocialFeed));
        if(flip_social_feed == NULL) {
            FURI_LOG_E(TAG, "Failed to allocate memory for feed");
            return false;
        }
    }
    for(int i = 0; i < 3; i++) {
        if(flip_social_feed->usernames[i] == NULL) {
            flip_social_feed->usernames[i] = malloc(MAX_USER_LENGTH);
            if(flip_social_feed->usernames[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for username %zu", i);
                return false;
            }
        }
        if(flip_social_feed->messages[i] == NULL) {
            flip_social_feed->messages[i] = malloc(MAX_MESSAGE_LENGTH);
            if(flip_social_feed->messages[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for message %zu", i);
                return false;
            }
        }
    }
    flip_social_feed->usernames[0] = "JBlanked";
    flip_social_feed->usernames[1] = "FlipperKing";
    flip_social_feed->usernames[2] = "FlipperQueen";
    //
    flip_social_feed->messages[0] =
        "Welcome. This is a temp message. Either the feed didn't load or there was a server error.";
    flip_social_feed->messages[1] = "I am the Chosen Flipper.";
    flip_social_feed->messages[2] = "No one can flip like me.";
    //
    flip_social_feed->is_flipped[0] = true;
    flip_social_feed->is_flipped[1] = false;
    flip_social_feed->is_flipped[2] = true;
    //
    flip_social_feed->ids[0] = 0;
    flip_social_feed->ids[1] = 1;
    flip_social_feed->ids[2] = 2;
    //
    flip_social_feed->flips[0] = 51;
    flip_social_feed->flips[1] = 8;
    flip_social_feed->flips[2] = 23;
    //
    flip_social_feed->count = 3;
    flip_social_feed->index = 0;

    return true;
}

// Allocate memory for each feed item if not already allocated
static FlipSocialFeed* flip_social_feed_alloc() {
    // Initialize the feed
    FlipSocialFeed* feed = (FlipSocialFeed*)malloc(sizeof(FlipSocialFeed));
    if(!feed) {
        FURI_LOG_E(TAG, "Failed to allocate memory for feed");
        return feed;
    }
    for(size_t i = 0; i < MAX_FEED_ITEMS; i++) {
        if(feed->usernames[i] == NULL) {
            feed->usernames[i] = malloc(MAX_USER_LENGTH);
            if(feed->usernames[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for username %zu", i);
                return NULL;
            }
        }
        if(feed->messages[i] == NULL) {
            feed->messages[i] = malloc(MAX_MESSAGE_LENGTH);
            if(feed->messages[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for message %zu", i);
                return NULL;
            }
        }
    }
    return feed;
}

static void flip_social_free_feed() {
    if(!flip_social_feed) {
        FURI_LOG_E(TAG, "Feed model is NULL");
        return;
    }
    for(uint32_t i = 0; i < flip_social_feed->count; i++) {
        free(flip_social_feed->usernames[i]);
    }
}

static bool flip_social_get_feed() {
    // Get the feed from the server
    if(app_instance->login_username_logged_out == NULL) {
        FURI_LOG_E(TAG, "Username is NULL");
        return false;
    }
    char command[128];
    snprintf(
        command,
        128,
        "https://www.flipsocial.net/api/feed/40/%s/extended/",
        app_instance->login_username_logged_out);
    bool success =
        flipper_http_get_request_with_headers(command, jsmn("Content-Type", "application/json"));
    if(!success) {
        FURI_LOG_E(TAG, "Failed to send HTTP request for feed");
        return false;
    }
    fhttp.state = RECEIVING;
    return true;
}

static bool flip_social_parse_json_feed() {
    if(fhttp.received_data == NULL) {
        FURI_LOG_E(TAG, "No data received.");
        return false;
    }

    // Allocate memory for each feed item if not already allocated
    flip_social_feed = flip_social_feed_alloc();
    if(flip_social_feed == NULL) {
        return false;
    }
    // Remove newlines
    char* pos = fhttp.received_data;
    while((pos = strchr(pos, '\n')) != NULL) {
        *pos = ' ';
    }

    // Initialize feed count
    flip_social_feed->count = 0;

    // Iterate through the feed array
    for(int i = 0; i < MAX_FEED_ITEMS; i++) {
        // Parse each item in the array
        char* item = get_json_array_value("feed", i, fhttp.received_data, MAX_TOKENS);
        if(item == NULL) {
            break;
        }

        // Extract individual fields from the JSON object
        char* username = get_json_value("username", item, MAX_TOKENS);
        char* message = get_json_value("message", item, MAX_TOKENS);
        char* flipped = get_json_value("flipped", item, MAX_TOKENS);
        char* flips = get_json_value("flip_count", item, MAX_TOKENS);
        char* id = get_json_value("id", item, MAX_TOKENS);

        if(username == NULL || message == NULL || flipped == NULL || id == NULL) {
            FURI_LOG_E(TAG, "Failed to parse item fields.");
            free(item);
            free(username);
            free(message);
            free(flipped);
            free(flips);
            free(id);
            continue;
        }

        // Safely copy strings with bounds checking
        strncpy(flip_social_feed->usernames[i], username, MAX_USER_LENGTH - 1);
        flip_social_feed->usernames[i][MAX_USER_LENGTH - 1] = '\0';

        strncpy(flip_social_feed->messages[i], message, MAX_MESSAGE_LENGTH - 1);
        flip_social_feed->messages[i][MAX_MESSAGE_LENGTH - 1] = '\0';

        // Store boolean and integer values
        flip_social_feed->is_flipped[i] = strstr(flipped, "true") != NULL;
        flip_social_feed->ids[i] = atoi(id);
        flip_social_feed->flips[i] = atoi(flips);

        flip_social_feed->count++;

        // Free allocated memory
        free(item);
        free(username);
        free(message);
        free(flipped);
        free(flips);
        free(id);
    }

    return flip_social_feed->count > 0;
}

#endif // FLIP_SOCIAL_FEED_H
