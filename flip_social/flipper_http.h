// flipper_http.h
#ifndef FLIPPER_HTTP_H
#define FLIPPER_HTTP_H

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_serial.h>
#include <storage/storage.h>

// STORAGE_EXT_PATH_PREFIX is defined in the Furi SDK as /ext

#define HTTP_TAG               "FlipSocial" // change this to your app name
#define http_tag               "flip_social" // change this to your app id
#define UART_CH                (FuriHalSerialIdUsart) // UART channel
#define TIMEOUT_DURATION_TICKS (6 * 1000) // 6 seconds
#define BAUDRATE               (115200) // UART baudrate
#define RX_BUF_SIZE            128 // UART RX buffer size
#define RX_LINE_BUFFER_SIZE    8192 // UART RX line buffer size (increase for large responses)

// Forward declaration for callback
typedef void (*FlipperHTTP_Callback)(const char* line, void* context);

// Functions
bool flipper_http_init(FlipperHTTP_Callback callback, void* context);
void flipper_http_deinit();
//---
void flipper_http_rx_callback(const char* line, void* context);
bool flipper_http_send_data(const char* data);
//---
bool flipper_http_connect_wifi();
bool flipper_http_disconnect_wifi();
bool flipper_http_ping();
bool flipper_http_scan_wifi();
bool flipper_http_save_wifi(const char* ssid, const char* password);
//---
bool flipper_http_get_request(const char* url);
bool flipper_http_get_request_with_headers(const char* url, const char* headers);
bool flipper_http_post_request_with_headers(
    const char* url,
    const char* headers,
    const char* payload);
bool flipper_http_put_request_with_headers(
    const char* url,
    const char* headers,
    const char* payload);
bool flipper_http_delete_request_with_headers(
    const char* url,
    const char* headers,
    const char* payload);
//---
bool flipper_http_save_received_data(size_t bytes_received, const char line_buffer[]);
static char* trim(const char* str);
//
bool flipper_http_process_response_async(bool (*http_request)(void), bool (*parse_json)(void));

// State variable to track the UART state
typedef enum {
    INACTIVE, // Inactive state
    IDLE, // Default state
    RECEIVING, // Receiving data
    SENDING, // Sending data
    ISSUE, // Issue with connection
} SerialState;

// Event Flags for UART Worker Thread
typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

// FlipperHTTP Structure
typedef struct {
    FuriStreamBuffer* flipper_http_stream; // Stream buffer for UART communication
    FuriHalSerialHandle* serial_handle; // Serial handle for UART communication
    FuriThread* rx_thread; // Worker thread for UART
    uint8_t rx_buf[RX_BUF_SIZE]; // Buffer for received data
    FuriThreadId rx_thread_id; // Worker thread ID
    FlipperHTTP_Callback handle_rx_line_cb; // Callback for received lines
    void* callback_context; // Context for the callback
    SerialState state; // State of the UART

    // variable to store the last received data from the UART
    char* last_response;

    // Timer-related members
    FuriTimer* get_timeout_timer; // Timer for HTTP request timeout
    char* received_data; // Buffer to store received data

    bool started_receiving_get; // Indicates if a GET request has started
    bool just_started_get; // Indicates if GET data reception has just started

    bool started_receiving_post; // Indicates if a POST request has started
    bool just_started_post; // Indicates if POST data reception has just started

    bool started_receiving_put; // Indicates if a PUT request has started
    bool just_started_put; // Indicates if PUT data reception has just started

    bool started_receiving_delete; // Indicates if a DELETE request has started
    bool just_started_delete; // Indicates if DELETE data reception has just started
} FlipperHTTP;

FlipperHTTP fhttp;

// fhttp.received_data holds the received data from HTTP requests
// fhttp.last_response holds the last received data from the UART, which could be [GET/END], [POST/END], [PUT/END], [DELETE/END], etc

// Timer callback function
/**
 * @brief      Callback function for the GET timeout timer.
 * @return     0
 * @param      context   The context to pass to the callback.
 * @note       This function will be called when the GET request times out.
 */
void get_timeout_timer_callback(void* context) {
    UNUSED(context);
    FURI_LOG_E(HTTP_TAG, "Timeout reached: 2 seconds without receiving the end.");

    // Reset the state
    fhttp.started_receiving_get = false;
    fhttp.started_receiving_post = false;
    fhttp.started_receiving_put = false;
    fhttp.started_receiving_delete = false;

    // Free received data if any
    if(fhttp.received_data) {
        free(fhttp.received_data);
        fhttp.received_data = NULL;
    }

    // Update UART state
    fhttp.state = ISSUE;
}

// UART RX Handler Callback (Interrupt Context)
/**
 * @brief      A private callback function to handle received data asynchronously.
 * @return     void
 * @param      handle    The UART handle.
 * @param      event     The event type.
 * @param      context   The context to pass to the callback.
 * @note       This function will handle received data asynchronously via the callback.
 */
static void _flipper_http_rx_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    UNUSED(context);
    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(fhttp.flipper_http_stream, &data, 1, 0);
        furi_thread_flags_set(fhttp.rx_thread_id, WorkerEvtRxDone);
    }
}

// UART worker thread
/**
 * @brief      Worker thread to handle UART data asynchronously.
 * @return     0
 * @param      context   The context to pass to the callback.
 * @note       This function will handle received data asynchronously via the callback.
 */
static int32_t flipper_http_worker(void* context) {
    UNUSED(context);
    size_t rx_line_pos = 0;
    char* rx_line_buffer = (char*)malloc(RX_LINE_BUFFER_SIZE);

    if(!rx_line_buffer) {
        // Handle malloc failure
        FURI_LOG_E(HTTP_TAG, "Failed to allocate memory for rx_line_buffer");
        return -1;
    }

    while(1) {
        uint32_t events = furi_thread_flags_wait(
            WorkerEvtStop | WorkerEvtRxDone, FuriFlagWaitAny, FuriWaitForever);
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(
                fhttp.flipper_http_stream, fhttp.rx_buf, RX_BUF_SIZE + 1, 0);
            for(size_t i = 0; i < len; i++) {
                char c = fhttp.rx_buf[i];
                if(c == '\n' || rx_line_pos >= RX_LINE_BUFFER_SIZE - 1) {
                    rx_line_buffer[rx_line_pos] = '\0';
                    // Invoke the callback with the complete line
                    if(fhttp.handle_rx_line_cb) {
                        fhttp.handle_rx_line_cb(rx_line_buffer, fhttp.callback_context);
                    }
                    // Reset the line buffer
                    rx_line_pos = 0;
                } else {
                    rx_line_buffer[rx_line_pos++] = c;
                }
            }
        }
    }

    // Free the allocated memory before exiting the thread
    free(rx_line_buffer);

    return 0;
}

// UART initialization function
/**
 * @brief      Initialize UART.
 * @return     true if the UART was initialized successfully, false otherwise.
 * @param      callback  The callback function to handle received data (ex. flipper_http_rx_callback).
 * @param      context   The context to pass to the callback.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_init(FlipperHTTP_Callback callback, void* context) {
    if(!context) {
        FURI_LOG_E(HTTP_TAG, "Invalid context provided to flipper_http_init.");
        return false;
    }
    if(!callback) {
        FURI_LOG_E(HTTP_TAG, "Invalid callback provided to flipper_http_init.");
        return false;
    }
    fhttp.flipper_http_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    if(!fhttp.flipper_http_stream) {
        FURI_LOG_E(HTTP_TAG, "Failed to allocate UART stream buffer.");
        return false;
    }

    fhttp.rx_thread = furi_thread_alloc();
    if(!fhttp.rx_thread) {
        FURI_LOG_E(HTTP_TAG, "Failed to allocate UART thread.");
        furi_stream_buffer_free(fhttp.flipper_http_stream);
        return false;
    }

    furi_thread_set_name(fhttp.rx_thread, "FlipperHTTP_RxThread");
    furi_thread_set_stack_size(fhttp.rx_thread, 1024);
    furi_thread_set_context(fhttp.rx_thread, &fhttp);
    furi_thread_set_callback(fhttp.rx_thread, flipper_http_worker);

    fhttp.handle_rx_line_cb = callback;
    fhttp.callback_context = context;

    furi_thread_start(fhttp.rx_thread);
    fhttp.rx_thread_id = furi_thread_get_id(fhttp.rx_thread);

    // handle when the UART control is busy to avoid furi_check failed
    if(furi_hal_serial_control_is_busy(UART_CH)) {
        FURI_LOG_E(HTTP_TAG, "UART control is busy.");
        return false;
    }

    fhttp.serial_handle = furi_hal_serial_control_acquire(UART_CH);
    if(!fhttp.serial_handle) {
        FURI_LOG_E(HTTP_TAG, "Failed to acquire UART control - handle is NULL");
        // Cleanup resources
        furi_thread_free(fhttp.rx_thread);
        furi_stream_buffer_free(fhttp.flipper_http_stream);
        return false;
    }

    // Initialize UART with acquired handle
    furi_hal_serial_init(fhttp.serial_handle, BAUDRATE);

    // Enable RX direction
    furi_hal_serial_enable_direction(fhttp.serial_handle, FuriHalSerialDirectionRx);

    // Start asynchronous RX with the callback
    furi_hal_serial_async_rx_start(fhttp.serial_handle, _flipper_http_rx_callback, &fhttp, false);

    // Wait for the TX to complete to ensure UART is ready
    furi_hal_serial_tx_wait_complete(fhttp.serial_handle);

    // Allocate the timer for handling timeouts
    fhttp.get_timeout_timer = furi_timer_alloc(
        get_timeout_timer_callback, // Callback function
        FuriTimerTypeOnce, // One-shot timer
        &fhttp // Context passed to callback
    );

    if(!fhttp.get_timeout_timer) {
        FURI_LOG_E(HTTP_TAG, "Failed to allocate HTTP request timeout timer.");
        // Cleanup resources
        furi_hal_serial_async_rx_stop(fhttp.serial_handle);
        furi_hal_serial_disable_direction(fhttp.serial_handle, FuriHalSerialDirectionRx);
        furi_hal_serial_control_release(fhttp.serial_handle);
        furi_hal_serial_deinit(fhttp.serial_handle);
        furi_thread_flags_set(fhttp.rx_thread_id, WorkerEvtStop);
        furi_thread_join(fhttp.rx_thread);
        furi_thread_free(fhttp.rx_thread);
        furi_stream_buffer_free(fhttp.flipper_http_stream);
        return false;
    }

    // Set the timer thread priority if needed
    furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);

    // FURI_LOG_I(HTTP_TAG, "UART initialized successfully.");
    return true;
}

// Deinitialize UART
/**
 * @brief      Deinitialize UART.
 * @return     void
 * @note       This function will stop the asynchronous RX, release the serial handle, and free the resources.
 */
void flipper_http_deinit() {
    if(fhttp.serial_handle == NULL) {
        FURI_LOG_E(HTTP_TAG, "UART handle is NULL. Already deinitialized?");
        return;
    }
    // Stop asynchronous RX
    furi_hal_serial_async_rx_stop(fhttp.serial_handle);

    // Release and deinitialize the serial handle
    furi_hal_serial_disable_direction(fhttp.serial_handle, FuriHalSerialDirectionRx);
    furi_hal_serial_control_release(fhttp.serial_handle);
    furi_hal_serial_deinit(fhttp.serial_handle);

    // Signal the worker thread to stop
    furi_thread_flags_set(fhttp.rx_thread_id, WorkerEvtStop);
    // Wait for the thread to finish
    furi_thread_join(fhttp.rx_thread);
    // Free the thread resources
    furi_thread_free(fhttp.rx_thread);

    // Free the stream buffer
    furi_stream_buffer_free(fhttp.flipper_http_stream);

    // Free the timer
    if(fhttp.get_timeout_timer) {
        furi_timer_free(fhttp.get_timeout_timer);
        fhttp.get_timeout_timer = NULL;
    }

    // Free received data if any
    if(fhttp.received_data) {
        free(fhttp.received_data);
        fhttp.received_data = NULL;
    }

    // Free the last response
    if(fhttp.last_response) {
        free(fhttp.last_response);
        fhttp.last_response = NULL;
    }

    // FURI_LOG_I("FlipperHTTP", "UART deinitialized successfully.");
}

// Function to send data over UART with newline termination
/**
 * @brief      Send data over UART with newline termination.
 * @return     true if the data was sent successfully, false otherwise.
 * @param      data  The data to send over UART.
 * @note       The data will be sent over UART with a newline character appended.
 */
bool flipper_http_send_data(const char* data) {
    size_t data_length = strlen(data);
    if(data_length == 0) {
        FURI_LOG_E("FlipperHTTP", "Attempted to send empty data.");
        return false;
    }

    // Create a buffer with data + '\n'
    size_t send_length = data_length + 1; // +1 for '\n'
    if(send_length > 256) { // Ensure buffer size is sufficient
        FURI_LOG_E("FlipperHTTP", "Data too long to send over FHTTP.");
        return false;
    }

    char send_buffer[257]; // 256 + 1 for safety
    strncpy(send_buffer, data, 256);
    send_buffer[data_length] = '\n'; // Append newline
    send_buffer[data_length + 1] = '\0'; // Null-terminate

    if(fhttp.state == INACTIVE && ((strstr(send_buffer, "[PING]") == NULL) &&
                                   (strstr(send_buffer, "[WIFI/CONNECT]") == NULL))) {
        FURI_LOG_E("FlipperHTTP", "Cannot send data while INACTIVE.");
        fhttp.last_response = "Cannot send data while INACTIVE.";
        return false;
    }

    fhttp.state = SENDING;
    furi_hal_serial_tx(fhttp.serial_handle, (const uint8_t*)send_buffer, send_length);

    // Uncomment below line to log the data sent over UART
    // FURI_LOG_I("FlipperHTTP", "Sent data over UART: %s", send_buffer);
    fhttp.state = IDLE;
    return true;
}

// Function to send a PING request
/**
 * @brief      Send a PING request to check if the Wifi Dev Board is connected.
 * @return     true if the request was successful, false otherwise.
 * @note       The received data will be handled asynchronously via the callback.
 * @note       This is best used to check if the Wifi Dev Board is connected.
 * @note       The state will remain INACTIVE until a PONG is received.
 */
bool flipper_http_ping() {
    const char* command = "[PING]";
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send PING command.");
        return false;
    }
    // set state as INACTIVE to be made IDLE if PONG is received
    fhttp.state = INACTIVE;
    // The response will be handled asynchronously via the callback
    return true;
}

// Function to scan for WiFi networks
/**
 * @brief      Send a command to scan for WiFi networks.
 * @return     true if the request was successful, false otherwise.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_scan_wifi() {
    const char* command = "[WIFI/SCAN]";
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send WiFi scan command.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}

// Function to save WiFi settings (returns true if successful)
/**
 * @brief      Send a command to save WiFi settings.
 * @return     true if the request was successful, false otherwise.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_save_wifi(const char* ssid, const char* password) {
    if(!ssid || !password) {
        FURI_LOG_E("FlipperHTTP", "Invalid arguments provided to flipper_http_save_wifi.");
        return false;
    }
    char buffer[256];
    int ret = snprintf(
        buffer, sizeof(buffer), "[WIFI/SAVE]{\"ssid\":\"%s\",\"password\":\"%s\"}", ssid, password);
    if(ret < 0 || ret >= (int)sizeof(buffer)) {
        FURI_LOG_E("FlipperHTTP", "Failed to format WiFi save command.");
        return false;
    }

    if(!flipper_http_send_data(buffer)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send WiFi save command.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}

// Function to disconnect from WiFi (returns true if successful)
/**
 * @brief      Send a command to disconnect from WiFi.
 * @return     true if the request was successful, false otherwise.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_disconnect_wifi() {
    const char* command = "[WIFI/DISCONNECT]";
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send WiFi disconnect command.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}

// Function to connect to WiFi (returns true if successful)
/**
 * @brief      Send a command to connect to WiFi.
 * @return     true if the request was successful, false otherwise.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_connect_wifi() {
    const char* command = "[WIFI/CONNECT]";
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send WiFi connect command.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}

// Function to send a GET request
/**
 * @brief      Send a GET request to the specified URL.
 * @return     true if the request was successful, false otherwise.
 * @param      url  The URL to send the GET request to.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_get_request(const char* url) {
    if(!url) {
        FURI_LOG_E("FlipperHTTP", "Invalid arguments provided to flipper_http_get_request.");
        return false;
    }

    // Prepare GET request command
    char command[256];
    int ret = snprintf(command, sizeof(command), "[GET]%s", url);
    if(ret < 0 || ret >= (int)sizeof(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to format GET request command.");
        return false;
    }

    // Send GET request via UART
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send GET request command.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}
// Function to send a GET request with headers
/**
 * @brief      Send a GET request to the specified URL.
 * @return     true if the request was successful, false otherwise.
 * @param      url  The URL to send the GET request to.
 * @param      headers  The headers to send with the GET request.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_get_request_with_headers(const char* url, const char* headers) {
    if(!url || !headers) {
        FURI_LOG_E(
            "FlipperHTTP", "Invalid arguments provided to flipper_http_get_request_with_headers.");
        return false;
    }

    // Prepare GET request command with headers
    char command[256];
    int ret = snprintf(
        command, sizeof(command), "[GET/HTTP]{\"url\":\"%s\",\"headers\":%s}", url, headers);
    if(ret < 0 || ret >= (int)sizeof(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to format GET request command with headers.");
        return false;
    }

    // Send GET request via UART
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send GET request command with headers.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}
// Function to send a POST request with headers
/**
 * @brief      Send a POST request to the specified URL.
 * @return     true if the request was successful, false otherwise.
 * @param      url  The URL to send the POST request to.
 * @param      headers  The headers to send with the POST request.
 * @param      data  The data to send with the POST request.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_post_request_with_headers(
    const char* url,
    const char* headers,
    const char* payload) {
    if(!url || !headers || !payload) {
        FURI_LOG_E(
            "FlipperHTTP",
            "Invalid arguments provided to flipper_http_post_request_with_headers.");
        return false;
    }

    // Prepare POST request command with headers and data
    char command[256];
    int ret = snprintf(
        command,
        sizeof(command),
        "[POST/HTTP]{\"url\":\"%s\",\"headers\":%s,\"payload\":%s}",
        url,
        headers,
        payload);
    if(ret < 0 || ret >= (int)sizeof(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to format POST request command with headers and data.");
        return false;
    }

    // Send POST request via UART
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send POST request command with headers and data.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}
// Function to send a PUT request with headers
/**
 * @brief      Send a PUT request to the specified URL.
 * @return     true if the request was successful, false otherwise.
 * @param      url  The URL to send the PUT request to.
 * @param      headers  The headers to send with the PUT request.
 * @param      data  The data to send with the PUT request.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_put_request_with_headers(
    const char* url,
    const char* headers,
    const char* payload) {
    if(!url || !headers || !payload) {
        FURI_LOG_E(
            "FlipperHTTP", "Invalid arguments provided to flipper_http_put_request_with_headers.");
        return false;
    }

    // Prepare PUT request command with headers and data
    char command[256];
    int ret = snprintf(
        command,
        sizeof(command),
        "[PUT/HTTP]{\"url\":\"%s\",\"headers\":%s,\"payload\":%s}",
        url,
        headers,
        payload);
    if(ret < 0 || ret >= (int)sizeof(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to format PUT request command with headers and data.");
        return false;
    }

    // Send PUT request via UART
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send PUT request command with headers and data.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}
// Function to send a DELETE request with headers
/**
 * @brief      Send a DELETE request to the specified URL.
 * @return     true if the request was successful, false otherwise.
 * @param      url  The URL to send the DELETE request to.
 * @param      headers  The headers to send with the DELETE request.
 * @param      data  The data to send with the DELETE request.
 * @note       The received data will be handled asynchronously via the callback.
 */
bool flipper_http_delete_request_with_headers(
    const char* url,
    const char* headers,
    const char* payload) {
    if(!url || !headers || !payload) {
        FURI_LOG_E(
            "FlipperHTTP",
            "Invalid arguments provided to flipper_http_delete_request_with_headers.");
        return false;
    }

    // Prepare DELETE request command with headers and data
    char command[256];
    int ret = snprintf(
        command,
        sizeof(command),
        "[DELETE/HTTP]{\"url\":\"%s\",\"headers\":%s,\"payload\":%s}",
        url,
        headers,
        payload);
    if(ret < 0 || ret >= (int)sizeof(command)) {
        FURI_LOG_E(
            "FlipperHTTP", "Failed to format DELETE request command with headers and data.");
        return false;
    }

    // Send DELETE request via UART
    if(!flipper_http_send_data(command)) {
        FURI_LOG_E("FlipperHTTP", "Failed to send DELETE request command with headers and data.");
        return false;
    }

    // The response will be handled asynchronously via the callback
    return true;
}
// Function to handle received data asynchronously
/**
 * @brief      Callback function to handle received data asynchronously.
 * @return     void
 * @param      line     The received line.
 * @param      context  The context passed to the callback.
 * @note       The received data will be handled asynchronously via the callback and handles the state of the UART.
 */
void flipper_http_rx_callback(const char* line, void* context) {
    if(!line || !context) {
        FURI_LOG_E(HTTP_TAG, "Invalid arguments provided to flipper_http_rx_callback.");
        return;
    }

    // Trim the received line to check if it's empty
    char* trimmed_line = trim(line);
    if(trimmed_line != NULL && trimmed_line[0] != '\0') {
        fhttp.last_response = (char*)line;
    }
    free(trimmed_line); // Free the allocated memory for trimmed_line

    if(fhttp.state != INACTIVE && fhttp.state != ISSUE) {
        fhttp.state = RECEIVING;
    }

    // Uncomment below line to log the data received over UART
    FURI_LOG_I(HTTP_TAG, "Received UART line: %s", line);

    // Check if we've started receiving data from a GET request
    if(fhttp.started_receiving_get) {
        // Restart the timeout timer each time new data is received
        furi_timer_restart(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);

        if(strstr(line, "[GET/END]") != NULL) {
            FURI_LOG_I(HTTP_TAG, "GET request completed.");
            // Stop the timer since we've completed the GET request
            furi_timer_stop(fhttp.get_timeout_timer);

            if(fhttp.received_data) {
                // uncomment if you want to save the received data to the external storage
                // flipper_http_save_received_data(strlen(fhttp.received_data), fhttp.received_data);
                fhttp.started_receiving_get = false;
                fhttp.just_started_get = false;
                fhttp.state = IDLE;
                return;
            } else {
                FURI_LOG_E(HTTP_TAG, "No data received.");
                fhttp.started_receiving_get = false;
                fhttp.just_started_get = false;
                fhttp.state = IDLE;
                return;
            }
        }

        // Append the new line to the existing data
        if(fhttp.received_data == NULL) {
            fhttp.received_data =
                (char*)malloc(strlen(line) + 2); // +2 for newline and null terminator
            if(fhttp.received_data) {
                strcpy(fhttp.received_data, line);
                fhttp.received_data[strlen(line)] = '\n'; // Add newline
                fhttp.received_data[strlen(line) + 1] = '\0'; // Null terminator
            }
        } else {
            size_t current_len = strlen(fhttp.received_data);
            size_t new_size = current_len + strlen(line) + 2; // +2 for newline and null terminator
            fhttp.received_data = (char*)realloc(fhttp.received_data, new_size);
            if(fhttp.received_data) {
                memcpy(
                    fhttp.received_data + current_len,
                    line,
                    strlen(line)); // Copy line at the end of the current data
                fhttp.received_data[current_len + strlen(line)] = '\n'; // Add newline
                fhttp.received_data[current_len + strlen(line) + 1] = '\0'; // Null terminator
            }
        }

        if(!fhttp.just_started_get) {
            fhttp.just_started_get = true;
        }
        return;
    }

    // Check if we've started receiving data from a POST request
    else if(fhttp.started_receiving_post) {
        // Restart the timeout timer each time new data is received
        furi_timer_restart(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);

        if(strstr(line, "[POST/END]") != NULL) {
            FURI_LOG_I(HTTP_TAG, "POST request completed.");
            // Stop the timer since we've completed the POST request
            furi_timer_stop(fhttp.get_timeout_timer);

            if(fhttp.received_data) {
                // uncomment if you want to save the received data to the external storage
                // flipper_http_save_received_data(strlen(fhttp.received_data), fhttp.received_data);
                fhttp.started_receiving_post = false;
                fhttp.just_started_post = false;
                fhttp.state = IDLE;
                return;
            } else {
                FURI_LOG_E(HTTP_TAG, "No data received.");
                fhttp.started_receiving_post = false;
                fhttp.just_started_post = false;
                fhttp.state = IDLE;
                return;
            }
        }

        // Append the new line to the existing data
        if(fhttp.received_data == NULL) {
            fhttp.received_data =
                (char*)malloc(strlen(line) + 2); // +2 for newline and null terminator
            if(fhttp.received_data) {
                strcpy(fhttp.received_data, line);
                fhttp.received_data[strlen(line)] = '\n'; // Add newline
                fhttp.received_data[strlen(line) + 1] = '\0'; // Null terminator
            }
        } else {
            size_t current_len = strlen(fhttp.received_data);
            size_t new_size = current_len + strlen(line) + 2; // +2 for newline and null terminator
            fhttp.received_data = (char*)realloc(fhttp.received_data, new_size);
            if(fhttp.received_data) {
                memcpy(
                    fhttp.received_data + current_len,
                    line,
                    strlen(line)); // Copy line at the end of the current data
                fhttp.received_data[current_len + strlen(line)] = '\n'; // Add newline
                fhttp.received_data[current_len + strlen(line) + 1] = '\0'; // Null terminator
            }
        }

        if(!fhttp.just_started_post) {
            fhttp.just_started_post = true;
        }
        return;
    }

    // Check if we've started receiving data from a PUT request
    else if(fhttp.started_receiving_put) {
        // Restart the timeout timer each time new data is received
        furi_timer_restart(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);

        if(strstr(line, "[PUT/END]") != NULL) {
            FURI_LOG_I(HTTP_TAG, "PUT request completed.");
            // Stop the timer since we've completed the PUT request
            furi_timer_stop(fhttp.get_timeout_timer);

            if(fhttp.received_data) {
                // uncomment if you want to save the received data to the external storage
                // flipper_http_save_received_data(strlen(fhttp.received_data), fhttp.received_data);
                fhttp.started_receiving_put = false;
                fhttp.just_started_put = false;
                fhttp.state = IDLE;
                return;
            } else {
                FURI_LOG_E(HTTP_TAG, "No data received.");
                fhttp.started_receiving_put = false;
                fhttp.just_started_put = false;
                fhttp.state = IDLE;
                return;
            }
        }

        // Append the new line to the existing data
        if(fhttp.received_data == NULL) {
            fhttp.received_data =
                (char*)malloc(strlen(line) + 2); // +2 for newline and null terminator
            if(fhttp.received_data) {
                strcpy(fhttp.received_data, line);
                fhttp.received_data[strlen(line)] = '\n'; // Add newline
                fhttp.received_data[strlen(line) + 1] = '\0'; // Null terminator
            }
        } else {
            size_t current_len = strlen(fhttp.received_data);
            size_t new_size = current_len + strlen(line) + 2; // +2 for newline and null terminator
            fhttp.received_data = (char*)realloc(fhttp.received_data, new_size);
            if(fhttp.received_data) {
                memcpy(
                    fhttp.received_data + current_len,
                    line,
                    strlen(line)); // Copy line at the end of the current data
                fhttp.received_data[current_len + strlen(line)] = '\n'; // Add newline
                fhttp.received_data[current_len + strlen(line) + 1] = '\0'; // Null terminator
            }
        }

        if(!fhttp.just_started_put) {
            fhttp.just_started_put = true;
        }
        return;
    }

    // Check if we've started receiving data from a DELETE request
    else if(fhttp.started_receiving_delete) {
        // Restart the timeout timer each time new data is received
        furi_timer_restart(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);

        if(strstr(line, "[DELETE/END]") != NULL) {
            FURI_LOG_I(HTTP_TAG, "DELETE request completed.");
            // Stop the timer since we've completed the DELETE request
            furi_timer_stop(fhttp.get_timeout_timer);

            if(fhttp.received_data) {
                // uncomment if you want to save the received data to the external storage
                // flipper_http_save_received_data(strlen(fhttp.received_data), fhttp.received_data);
                fhttp.started_receiving_delete = false;
                fhttp.just_started_delete = false;
                fhttp.state = IDLE;
                return;
            } else {
                FURI_LOG_E(HTTP_TAG, "No data received.");
                fhttp.started_receiving_delete = false;
                fhttp.just_started_delete = false;
                fhttp.state = IDLE;
                return;
            }
        }

        // Append the new line to the existing data
        if(fhttp.received_data == NULL) {
            fhttp.received_data =
                (char*)malloc(strlen(line) + 2); // +2 for newline and null terminator
            if(fhttp.received_data) {
                strcpy(fhttp.received_data, line);
                fhttp.received_data[strlen(line)] = '\n'; // Add newline
                fhttp.received_data[strlen(line) + 1] = '\0'; // Null terminator
            }
        } else {
            size_t current_len = strlen(fhttp.received_data);
            size_t new_size = current_len + strlen(line) + 2; // +2 for newline and null terminator
            fhttp.received_data = (char*)realloc(fhttp.received_data, new_size);
            if(fhttp.received_data) {
                memcpy(
                    fhttp.received_data + current_len,
                    line,
                    strlen(line)); // Copy line at the end of the current data
                fhttp.received_data[current_len + strlen(line)] = '\n'; // Add newline
                fhttp.received_data[current_len + strlen(line) + 1] = '\0'; // Null terminator
            }
        }

        if(!fhttp.just_started_delete) {
            fhttp.just_started_delete = true;
        }
        return;
    }

    // Handle different types of responses
    if(strstr(line, "[SUCCESS]") != NULL || strstr(line, "[CONNECTED]") != NULL) {
        // FURI_LOG_I(HTTP_TAG, "Operation succeeded.");
    } else if(strstr(line, "[INFO]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "Received info: %s", line);

        if(fhttp.state == INACTIVE && strstr(line, "[INFO] Already connected to Wifi.") != NULL) {
            fhttp.state = IDLE;
        }
    } else if(strstr(line, "[GET/SUCCESS]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "GET request succeeded.");
        fhttp.started_receiving_get = true;
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
        fhttp.received_data = NULL;
        return;
    } else if(strstr(line, "[POST/SUCCESS]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "POST request succeeded.");
        fhttp.started_receiving_post = true;
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
        fhttp.received_data = NULL;
        return;
    } else if(strstr(line, "[PUT/SUCCESS]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "PUT request succeeded.");
        fhttp.started_receiving_put = true;
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
        fhttp.received_data = NULL;
        return;
    } else if(strstr(line, "[DELETE/SUCCESS]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "DELETE request succeeded.");
        fhttp.started_receiving_delete = true;
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
        fhttp.received_data = NULL;
        return;
    } else if(strstr(line, "[DISCONNECTED]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "WiFi disconnected successfully.");
    } else if(strstr(line, "[ERROR]") != NULL) {
        FURI_LOG_E(HTTP_TAG, "Received error: %s", line);
        fhttp.state = ISSUE;
        return;
    } else if(strstr(line, "[PONG]") != NULL) {
        FURI_LOG_I(HTTP_TAG, "Received PONG response: Wifi Dev Board is still alive.");

        // send command to connect to WiFi
        if(fhttp.state == INACTIVE) {
            fhttp.state = IDLE;
            return;
        }
    }

    if(fhttp.state == INACTIVE && strstr(line, "[PONG]") != NULL) {
        fhttp.state = IDLE;
    } else if(fhttp.state == INACTIVE && strstr(line, "[PONG]") == NULL) {
        fhttp.state = INACTIVE;
    } else {
        fhttp.state = IDLE;
    }
}
// Function to save received data to a file
/**
 * @brief      Save the received data to a file.
 * @return     true if the data was saved successfully, false otherwise.
 * @param      bytes_received  The number of bytes received.
 * @param      line_buffer     The buffer containing the received data.
 * @note       The data will be saved to a file in the STORAGE_EXT_PATH_PREFIX "/apps_data/" http_tag "/received_data.txt" directory.
 */
bool flipper_http_save_received_data(size_t bytes_received, const char line_buffer[]) {
    const char* output_file_path = STORAGE_EXT_PATH_PREFIX "/apps_data/" http_tag
                                                           "/received_data.txt";

    // Ensure the directory exists
    char directory_path[128];
    snprintf(
        directory_path, sizeof(directory_path), STORAGE_EXT_PATH_PREFIX "/apps_data/" http_tag);

    Storage* _storage = NULL;
    File* _file = NULL;
    // Open the storage if not opened already
    // Initialize storage and create the directory if it doesn't exist
    _storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(_storage, directory_path); // Create directory if it doesn't exist
    _file = storage_file_alloc(_storage);

    // Open file for writing and append data line by line
    if(!storage_file_open(_file, output_file_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(HTTP_TAG, "Failed to open output file for writing.");
        storage_file_free(_file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    // Write each line received from the UART to the file
    if(bytes_received > 0 && _file) {
        storage_file_write(_file, line_buffer, bytes_received);
        storage_file_write(_file, "\n", 1); // Add a newline after each line
    } else {
        FURI_LOG_E(HTTP_TAG, "No data received.");
        return false;
    }

    if(_file) {
        storage_file_close(_file);
        storage_file_free(_file);
        _file = NULL;
    }
    if(_storage) {
        furi_record_close(RECORD_STORAGE);
        _storage = NULL;
    }

    return true;
}
// Function to trim leading and trailing spaces and newlines from a constant string
char* trim(const char* str) {
    const char* end;
    char* trimmed_str;
    size_t len;

    // Trim leading space
    while(isspace((unsigned char)*str))
        str++;

    // All spaces?
    if(*str == 0) return strdup(""); // Return an empty string if all spaces

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end))
        end--;

    // Set length for the trimmed string
    len = end - str + 1;

    // Allocate space for the trimmed string and null terminator
    trimmed_str = (char*)malloc(len + 1);
    if(trimmed_str == NULL) {
        return NULL; // Handle memory allocation failure
    }

    // Copy the trimmed part of the string into trimmed_str
    strncpy(trimmed_str, str, len);
    trimmed_str[len] = '\0'; // Null terminate the string

    return trimmed_str;
}

/**
 * @brief Process requests and parse JSON data asynchronously
 * @param http_request The function to send the request
 * @param parse_json The function to parse the JSON
 * @return true if successful, false otherwise
 */
bool flipper_http_process_response_async(bool (*http_request)(void), bool (*parse_json)(void)) {
    if(http_request()) // start the async request
    {
        furi_timer_start(fhttp.get_timeout_timer, TIMEOUT_DURATION_TICKS);
        fhttp.state = RECEIVING;
    } else {
        FURI_LOG_E(HTTP_TAG, "Failed to send request");
        return false;
    }
    while(fhttp.state == RECEIVING && furi_timer_is_running(fhttp.get_timeout_timer) > 0) {
        // Wait for the request to be received
        furi_delay_ms(100);
    }
    furi_timer_stop(fhttp.get_timeout_timer);
    if(!parse_json()) // parse the JSON before switching to the view (synchonous)
    {
        FURI_LOG_E(HTTP_TAG, "Failed to parse the JSON...");
        return false;
    }
    furi_timer_stop(fhttp.get_timeout_timer);
    return true;
}

#endif // FLIPPER_HTTP_H
