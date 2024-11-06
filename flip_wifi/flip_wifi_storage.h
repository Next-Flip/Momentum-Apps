#ifndef FLIP_WIFI_STORAGE_H
#define FLIP_WIFI_STORAGE_H

// define the paths for all of the FlipperHTTP apps
#define WIFI_SSID_LIST_PATH STORAGE_EXT_PATH_PREFIX "/apps_data/flip_wifi/wifi_list.txt"

// Function to save the playlist
void save_playlist(const WiFiPlaylist* playlist) {
    if(!playlist) {
        FURI_LOG_E(TAG, "Playlist is NULL");
        return;
    }

    // Create the directory for saving settings
    char directory_path[128];
    snprintf(
        directory_path, sizeof(directory_path), STORAGE_EXT_PATH_PREFIX "/apps_data/flip_wifi");

    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        FURI_LOG_E(TAG, "Failed to open storage record");
        return;
    }

    // Create the directory
    storage_common_mkdir(storage, directory_path);

    // Open the settings file
    File* file = storage_file_alloc(storage);
    if(!file) {
        FURI_LOG_E(TAG, "Failed to allocate file handle");
        furi_record_close(RECORD_STORAGE);
        return;
    }
    if(!storage_file_open(file, WIFI_SSID_LIST_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open settings file for writing: %s", WIFI_SSID_LIST_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    for(size_t i = 0; i < playlist->count; i++) {
        if(!playlist->ssids[i] || !playlist->passwords[i]) {
            FURI_LOG_E(TAG, "Invalid SSID or password at index %zu", i);
            continue;
        }
        size_t ssid_length = strlen(playlist->ssids[i]);
        size_t password_length = strlen(playlist->passwords[i]);
        if(storage_file_write(file, playlist->ssids[i], ssid_length) != ssid_length ||
           storage_file_write(file, ",", 1) != 1 ||
           storage_file_write(file, playlist->passwords[i], password_length) != password_length ||
           storage_file_write(file, "\n", 1) != 1) {
            FURI_LOG_E(TAG, "Failed to write playlist");
        }
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

// Function to load the playlist
bool load_playlist(WiFiPlaylist* playlist) {
    if(!playlist) {
        FURI_LOG_E(TAG, "Playlist is NULL");
        return false;
    }

    // Initialize playlist count
    playlist->count = 0;

    // Allocate memory for SSIDs and passwords if not already allocated
    for(size_t i = 0; i < MAX_WIFI_NETWORKS; i++) {
        if(!playlist->ssids[i]) {
            playlist->ssids[i] = malloc(64); // Adjust size as needed
            if(!playlist->ssids[i]) {
                FURI_LOG_E(TAG, "Memory allocation failed for ssids[%zu]", i);
                // Handle memory allocation failure (e.g., clean up and return)
                return false;
            }
        }

        if(!playlist->passwords[i]) {
            playlist->passwords[i] = malloc(64); // Adjust size as needed
            if(!playlist->passwords[i]) {
                FURI_LOG_E(TAG, "Memory allocation failed for passwords[%zu]", i);
                // Handle memory allocation failure (e.g., clean up and return)
                return false;
            }
        }
    }

    // Open the settings file
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage) {
        FURI_LOG_E(TAG, "Failed to open storage record");
        return false;
    }

    File* file = storage_file_alloc(storage);
    if(!file) {
        FURI_LOG_E(TAG, "Failed to allocate file handle");
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    if(!storage_file_open(file, WIFI_SSID_LIST_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open settings file for reading: %s", WIFI_SSID_LIST_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false; // Return false if the file does not exist
    }

    // Buffer to hold each line
    char line_buffer[128];
    size_t line_pos = 0;
    char ch;

    while(storage_file_read(file, &ch, 1) == 1) {
        if(ch == '\n') {
            // Null-terminate the line
            line_buffer[line_pos] = '\0';

            // Split the line into SSID and Password
            char* comma_pos = strchr(line_buffer, ',');
            if(comma_pos) {
                *comma_pos = '\0'; // Replace comma with null character

                // Copy SSID
                strncpy(playlist->ssids[playlist->count], line_buffer, 63);
                playlist->ssids[playlist->count][63] = '\0'; // Ensure null-termination

                // Copy Password
                strncpy(playlist->passwords[playlist->count], comma_pos + 1, 63);
                playlist->passwords[playlist->count][63] = '\0'; // Ensure null-termination

                playlist->count++;

                if(playlist->count >= MAX_WIFI_NETWORKS) {
                    FURI_LOG_W(
                        TAG, "Reached maximum number of WiFi networks: %d", MAX_WIFI_NETWORKS);
                    break;
                }
            } else {
                FURI_LOG_E(TAG, "Invalid line format (no comma found): %s", line_buffer);
            }

            // Reset line buffer position for the next line
            line_pos = 0;
        } else {
            if(line_pos < sizeof(line_buffer) - 1) {
                line_buffer[line_pos++] = ch;
            } else {
                FURI_LOG_E(TAG, "Line buffer overflow");
                // Optionally handle line overflow (e.g., skip the rest of the line)
                line_pos = 0;
            }
        }
    }

    // Handle the last line if it does not end with a newline
    if(line_pos > 0) {
        line_buffer[line_pos] = '\0';
        char* comma_pos = strchr(line_buffer, ',');
        if(comma_pos) {
            *comma_pos = '\0'; // Replace comma with null character

            // Copy SSID
            strncpy(playlist->ssids[playlist->count], line_buffer, 63);
            playlist->ssids[playlist->count][63] = '\0'; // Ensure null-termination

            // Copy Password
            strncpy(playlist->passwords[playlist->count], comma_pos + 1, 63);
            playlist->passwords[playlist->count][63] = '\0'; // Ensure null-termination

            playlist->count++;

            if(playlist->count >= MAX_WIFI_NETWORKS) {
                FURI_LOG_W(TAG, "Reached maximum number of WiFi networks: %d", MAX_WIFI_NETWORKS);
            }
        } else {
            FURI_LOG_E(TAG, "Invalid line format (no comma found): %s", line_buffer);
        }
    }

    // Close and free file resources
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}

char* app_ids[7] = {
    "flip_wifi",
    "flip_store",
    "flip_social",
    "flip_trader",
    "flip_weather",
    "flip_library",
    "web_crawler"};

void save_settings(const char* ssid, const char* password) {
    char edited_directory_path[128];
    char edited_file_path[128];

    for(size_t i = 0; i < 7; i++) {
        // Construct the directory and file paths for the current app
        snprintf(
            edited_directory_path,
            sizeof(edited_directory_path),
            STORAGE_EXT_PATH_PREFIX "/apps_data/%s",
            app_ids[i]);
        snprintf(
            edited_file_path,
            sizeof(edited_file_path),
            STORAGE_EXT_PATH_PREFIX "/apps_data/%s/settings.bin",
            app_ids[i]);

        // Open the storage record
        Storage* storage = furi_record_open(RECORD_STORAGE);
        if(!storage) {
            FURI_LOG_E(TAG, "Failed to open storage record for app: %s", app_ids[i]);
            continue; // Skip to the next app
        }

        // Ensure the directory exists
        storage_common_mkdir(storage, edited_directory_path);

        // Allocate a file handle
        File* file = storage_file_alloc(storage);
        if(!file) {
            FURI_LOG_E(TAG, "Failed to allocate storage file for app: %s", app_ids[i]);
            furi_record_close(RECORD_STORAGE);
            continue; // Skip to the next app
        }

        // Open the file in read mode to read existing data
        bool file_opened =
            storage_file_open(file, edited_file_path, FSAM_READ, FSOM_OPEN_EXISTING);
        size_t file_size = 0;
        uint8_t* buffer = NULL;

        if(file_opened) {
            // Get the file size
            file_size = storage_file_size(file);
            buffer = malloc(file_size);
            if(!buffer) {
                FURI_LOG_E(TAG, "Failed to allocate buffer for app: %s", app_ids[i]);
                storage_file_close(file);
                storage_file_free(file);
                furi_record_close(RECORD_STORAGE);
                continue;
            }

            // Read the existing data
            if(storage_file_read(file, buffer, file_size) != file_size) {
                FURI_LOG_E(TAG, "Failed to read settings file for app: %s", app_ids[i]);
                free(buffer);
                storage_file_close(file);
                storage_file_free(file);
                furi_record_close(RECORD_STORAGE);
                continue;
            }

            storage_file_close(file);
        } else {
            // If the file doesn't exist, initialize an empty buffer
            file_size = 0;
            buffer = NULL;
        }
        storage_file_free(file);

        // Prepare new SSID and Password
        size_t new_ssid_length = strlen(ssid) + 1; // Including null terminator
        size_t new_password_length = strlen(password) + 1; // Including null terminator

        // Calculate the new file size
        size_t new_file_size =
            sizeof(size_t) + new_ssid_length + sizeof(size_t) + new_password_length;

        // If there is additional data beyond SSID and Password, preserve it
        size_t additional_data_size = 0;
        uint8_t* additional_data = NULL;

        if(buffer) {
            // Parse existing SSID length
            if(file_size >= sizeof(size_t)) {
                size_t existing_ssid_length;
                memcpy(&existing_ssid_length, buffer, sizeof(size_t));

                // Parse existing Password length
                if(file_size >= sizeof(size_t) + existing_ssid_length + sizeof(size_t)) {
                    size_t existing_password_length;
                    memcpy(
                        &existing_password_length,
                        buffer + sizeof(size_t) + existing_ssid_length,
                        sizeof(size_t));

                    // Calculate the offset where additional data starts
                    size_t additional_offset = sizeof(size_t) + existing_ssid_length +
                                               sizeof(size_t) + existing_password_length;
                    if(additional_offset < file_size) {
                        additional_data_size = file_size - additional_offset;
                        additional_data = malloc(additional_data_size);
                        if(additional_data) {
                            memcpy(
                                additional_data, buffer + additional_offset, additional_data_size);
                        } else {
                            FURI_LOG_E(
                                TAG,
                                "Failed to allocate memory for additional data for app: %s",
                                app_ids[i]);
                            free(buffer);
                            furi_record_close(RECORD_STORAGE);
                            continue;
                        }
                    }
                } else {
                    FURI_LOG_E(TAG, "Settings file format invalid for app: %s", app_ids[i]);
                }
            } else {
                FURI_LOG_E(TAG, "Settings file too small for app: %s", app_ids[i]);
            }
        }

        // Allocate a new buffer for updated data
        size_t total_new_size = new_file_size + additional_data_size;
        uint8_t* new_buffer = malloc(total_new_size);
        if(!new_buffer) {
            FURI_LOG_E(TAG, "Failed to allocate new buffer for app: %s", app_ids[i]);
            if(buffer) free(buffer);
            if(additional_data) free(additional_data);
            furi_record_close(RECORD_STORAGE);
            continue;
        }

        size_t offset = 0;

        // Write new SSID length and SSID
        memcpy(new_buffer + offset, &new_ssid_length, sizeof(size_t));
        offset += sizeof(size_t);
        memcpy(new_buffer + offset, ssid, new_ssid_length);
        offset += new_ssid_length;

        // Write new Password length and Password
        memcpy(new_buffer + offset, &new_password_length, sizeof(size_t));
        offset += sizeof(size_t);
        memcpy(new_buffer + offset, password, new_password_length);
        offset += new_password_length;

        // Append any additional data if present
        if(additional_data_size > 0 && additional_data) {
            memcpy(new_buffer + offset, additional_data, additional_data_size);
            offset += additional_data_size;
        }

        // Free temporary buffers
        if(buffer) free(buffer);
        if(additional_data) free(additional_data);

        // Open the file in write mode with FSOM_CREATE_ALWAYS to overwrite it
        file = storage_file_alloc(storage);
        if(!file) {
            FURI_LOG_E(TAG, "Failed to allocate storage file for writing: %s", app_ids[i]);
            free(new_buffer);
            furi_record_close(RECORD_STORAGE);
            continue;
        }

        if(!storage_file_open(file, edited_file_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            FURI_LOG_E(TAG, "Failed to open settings file for writing: %s", edited_file_path);
            storage_file_free(file);
            free(new_buffer);
            furi_record_close(RECORD_STORAGE);
            continue;
        }

        // Write the updated buffer back to the file
        if(storage_file_write(file, new_buffer, total_new_size) != total_new_size) {
            FURI_LOG_E(TAG, "Failed to write updated settings for app: %s", app_ids[i]);
        }

        // Clean up
        free(new_buffer);
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
    }
}

#endif
