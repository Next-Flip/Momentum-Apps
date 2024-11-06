// flip_social_storage.h
#ifndef FLIP_SOCIAL_STORAGE_H
#define FLIP_SOCIAL_STORAGE_H

#include <furi.h>
#include <storage/storage.h>

#define SETTINGS_PATH STORAGE_EXT_PATH_PREFIX "/apps_data/flip_social/settings.bin"
#define PRE_SAVED_MESSAGES_PATH \
    STORAGE_EXT_PATH_PREFIX "/apps_data/flip_social/pre_saved_messages.txt"

// Function to save the playlist
void save_playlist(const PreSavedPlaylist* playlist) {
    if(!playlist) {
        FURI_LOG_E(TAG, "Playlist is NULL");
        return;
    }
    // Create the directory for saving settings
    char directory_path[128];
    snprintf(
        directory_path, sizeof(directory_path), STORAGE_EXT_PATH_PREFIX "/apps_data/flip_social");

    // Create the directory
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(storage, directory_path);

    // Open the settings file
    File* file = storage_file_alloc(storage);
    if(!storage_file_open(file, PRE_SAVED_MESSAGES_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open settings file for writing: %s", PRE_SAVED_MESSAGES_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    // Write each playlist message on a separate line
    for(size_t i = 0; i < playlist->count; ++i) {
        // Add a newline character after each message
        if(storage_file_write(file, playlist->messages[i], strlen(playlist->messages[i])) !=
           strlen(playlist->messages[i])) {
            FURI_LOG_E(TAG, "Failed to write playlist message %zu", i);
        }

        // Write a newline after each message
        if(storage_file_write(file, "\n", 1) != 1) {
            FURI_LOG_E(TAG, "Failed to write newline after message %zu", i);
        }
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

// Function to load the playlist
// Function to load the playlist
bool load_playlist(PreSavedPlaylist* playlist) {
    // Ensure playlist is not NULL
    if(!playlist) {
        FURI_LOG_E(TAG, "Playlist is NULL");
        return false;
    }

    // Ensure playlist->messages is not NULL and allocate memory for each message
    for(size_t i = 0; i < MAX_PRE_SAVED_MESSAGES; ++i) {
        if(!playlist->messages[i]) // Check if memory is already allocated
        {
            playlist->messages[i] = (char*)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
            if(!playlist->messages[i]) {
                FURI_LOG_E(TAG, "Failed to allocate memory for message %zu", i);
                return false; // Return false on memory allocation failure
            }
        }
    }

    // Open the storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, PRE_SAVED_MESSAGES_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(
            TAG,
            "Failed to open pre-saved messages file for reading: %s",
            PRE_SAVED_MESSAGES_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false; // Return false if the file does not exist
    }

    // Initialize the playlist count
    playlist->count = 0;

    // Read the file byte by byte to simulate reading lines
    char ch;
    size_t message_pos = 0;
    bool message_started = false;

    while(storage_file_read(file, &ch, 1) == 1) // Read one character at a time
    {
        message_started = true;

        if(ch == '\n' ||
           message_pos >= (MAX_MESSAGE_LENGTH - 1)) // End of line or message is too long
        {
            playlist->messages[playlist->count][message_pos] = '\0'; // Null-terminate the message
            playlist->count++; // Move to the next message
            message_pos = 0; // Reset for the next message
            message_started = false;

            // Ensure the playlist count does not exceed the maximum
            if(playlist->count >= MAX_PRE_SAVED_MESSAGES) {
                FURI_LOG_W(TAG, "Reached maximum playlist messages");
                break;
            }
        } else {
            playlist->messages[playlist->count][message_pos++] =
                ch; // Add character to current message
        }
    }

    // Handle the case where the last message does not end with a newline
    if(message_started && message_pos > 0) {
        playlist->messages[playlist->count][message_pos] = '\0'; // Null-terminate the last message
        playlist->count++; // Increment the count for the last message

        // Ensure the playlist count does not exceed the maximum
        if(playlist->count >= MAX_PRE_SAVED_MESSAGES) {
            FURI_LOG_W(TAG, "Reached maximum playlist messages");
        }
    }

    // Close the file and storage
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}
static void save_settings(
    const char* ssid,
    const char* password,
    const char* login_username_logged_out,
    const char* login_username_logged_in,
    const char* login_password_logged_out,
    const char* change_password_logged_in,
    const char* is_logged_in) {
    // Create the directory for saving settings
    char directory_path[128];
    snprintf(
        directory_path, sizeof(directory_path), STORAGE_EXT_PATH_PREFIX "/apps_data/flip_social");

    // Create the directory
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(storage, directory_path);

    // Open the settings file
    File* file = storage_file_alloc(storage);
    if(!storage_file_open(file, SETTINGS_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open settings file for writing: %s", SETTINGS_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    // Save the ssid length and data
    size_t ssid_length = strlen(ssid) + 1; // Include null terminator
    if(storage_file_write(file, &ssid_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, ssid, ssid_length) != ssid_length) {
        FURI_LOG_E(TAG, "Failed to write SSID");
    }

    // Save the password length and data
    size_t password_length = strlen(password) + 1; // Include null terminator
    if(storage_file_write(file, &password_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, password, password_length) != password_length) {
        FURI_LOG_E(TAG, "Failed to write password");
    }

    // Save the login_username_logged_out length and data
    size_t username_out_length = strlen(login_username_logged_out) + 1; // Include null terminator
    if(storage_file_write(file, &username_out_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, login_username_logged_out, username_out_length) !=
           username_out_length) {
        FURI_LOG_E(TAG, "Failed to write login_username_logged_out");
    }

    // Save the login_username_logged_in length and data
    size_t username_in_length = strlen(login_username_logged_in) + 1; // Include null terminator
    if(storage_file_write(file, &username_in_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, login_username_logged_in, username_in_length) !=
           username_in_length) {
        FURI_LOG_E(TAG, "Failed to write login_username_logged_in");
    }

    // Save the login_password_logged_out length and data
    size_t password_out_length = strlen(login_password_logged_out) + 1; // Include null terminator
    if(storage_file_write(file, &password_out_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, login_password_logged_out, password_out_length) !=
           password_out_length) {
        FURI_LOG_E(TAG, "Failed to write login_password_logged_out");
    }

    // Save the change_password_logged_in length and data
    size_t change_password_length =
        strlen(change_password_logged_in) + 1; // Include null terminator
    if(storage_file_write(file, &change_password_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, change_password_logged_in, change_password_length) !=
           change_password_length) {
        FURI_LOG_E(TAG, "Failed to write change_password_logged_in");
    }

    // Save the is_logged_in length and data
    size_t is_logged_in_length = strlen(is_logged_in) + 1; // Include null terminator
    if(storage_file_write(file, &is_logged_in_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, is_logged_in, is_logged_in_length) != is_logged_in_length) {
        FURI_LOG_E(TAG, "Failed to write is_logged_in");
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

static bool load_settings(
    char* ssid,
    size_t ssid_size,
    char* password,
    size_t password_size,
    char* login_username_logged_out,
    size_t username_out_size,
    char* login_username_logged_in,
    size_t username_in_size,
    char* login_password_logged_out,
    size_t password_out_size,
    char* change_password_logged_in,
    size_t change_password_size,
    char* is_logged_in,
    size_t is_logged_in_size) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, SETTINGS_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open settings file for reading: %s", SETTINGS_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false; // Return false if the file does not exist
    }

    // Load the ssid
    size_t ssid_length;
    if(storage_file_read(file, &ssid_length, sizeof(size_t)) != sizeof(size_t) ||
       ssid_length > ssid_size || storage_file_read(file, ssid, ssid_length) != ssid_length) {
        FURI_LOG_E(TAG, "Failed to read SSID");
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    } else {
        ssid[ssid_length - 1] = '\0'; // Ensure null-termination
    }

    // Load the password
    size_t password_length;
    if(storage_file_read(file, &password_length, sizeof(size_t)) != sizeof(size_t) ||
       password_length > password_size ||
       storage_file_read(file, password, password_length) != password_length) {
        FURI_LOG_E(TAG, "Failed to read password");
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    } else {
        password[password_length - 1] = '\0'; // Ensure null-termination
    }

    // Load the login_username_logged_out
    size_t username_out_length;
    if(storage_file_read(file, &username_out_length, sizeof(size_t)) != sizeof(size_t) ||
       username_out_length > username_out_size ||
       storage_file_read(file, login_username_logged_out, username_out_length) !=
           username_out_length) {
        FURI_LOG_E(TAG, "Failed to read login_username_logged_out");
        // storage_file_close(file);
        // storage_file_free(file);
        // furi_record_close(RECORD_STORAGE);
        // return false;
    } else {
        login_username_logged_out[username_out_length - 1] = '\0'; // Ensure null-termination
    }

    // Load the login_username_logged_in
    size_t username_in_length;
    if(storage_file_read(file, &username_in_length, sizeof(size_t)) != sizeof(size_t) ||
       username_in_length > username_in_size ||
       storage_file_read(file, login_username_logged_in, username_in_length) !=
           username_in_length) {
        FURI_LOG_E(TAG, "Failed to read login_username_logged_in");
        // storage_file_close(file);
        // storage_file_free(file);
        // furi_record_close(RECORD_STORAGE);
        // return false;
    } else {
        login_username_logged_in[username_in_length - 1] = '\0'; // Ensure null-termination
    }

    // Load the login_password_logged_out
    size_t password_out_length;
    if(storage_file_read(file, &password_out_length, sizeof(size_t)) != sizeof(size_t) ||
       password_out_length > password_out_size ||
       storage_file_read(file, login_password_logged_out, password_out_length) !=
           password_out_length) {
        FURI_LOG_E(TAG, "Failed to read login_password_logged_out");
        // storage_file_close(file);
        // storage_file_free(file);
        // furi_record_close(RECORD_STORAGE);
        // return false;
    } else {
        login_password_logged_out[password_out_length - 1] = '\0'; // Ensure null-termination
    }

    // Load the change_password_logged_in
    size_t change_password_length;
    if(storage_file_read(file, &change_password_length, sizeof(size_t)) != sizeof(size_t) ||
       change_password_length > change_password_size ||
       storage_file_read(file, change_password_logged_in, change_password_length) !=
           change_password_length) {
        FURI_LOG_E(TAG, "Failed to read change_password_logged_in");
        // storage_file_close(file);
        // storage_file_free(file);
        // furi_record_close(RECORD_STORAGE);
        //  return false;
    } else {
        change_password_logged_in[change_password_length - 1] = '\0'; // Ensure null-termination
    }

    // Load the is_logged_in
    size_t is_logged_in_length;
    if(storage_file_read(file, &is_logged_in_length, sizeof(size_t)) != sizeof(size_t) ||
       is_logged_in_length > is_logged_in_size ||
       storage_file_read(file, is_logged_in, is_logged_in_length) != is_logged_in_length) {
        FURI_LOG_E(TAG, "Failed to read is_logged_in");
        // storage_file_close(file);
        // storage_file_free(file);
        // furi_record_close(RECORD_STORAGE);
        //  return false;
    } else {
        is_logged_in[is_logged_in_length - 1] = '\0'; // Ensure null-termination
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}

#endif // FLIP_SOCIAL_STORAGE_H
