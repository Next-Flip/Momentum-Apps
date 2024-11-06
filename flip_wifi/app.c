#include <flip_wifi_e.h>
#include <flip_wifi_storage.h>
#include <flip_wifi_callback.h>
#include <flip_wifi_i.h>
#include <flip_wifi_free.h>

// Entry point for the FlipWiFi application
int32_t flip_wifi_main(void* p) {
    // Suppress unused parameter warning
    UNUSED(p);

    // Initialize the FlipWiFi application
    FlipWiFiApp* app = flip_wifi_app_alloc();
    if(!app) {
        FURI_LOG_E(TAG, "Failed to allocate FlipWiFiApp");
        return -1;
    }

    if(!flipper_http_ping()) {
        FURI_LOG_E(TAG, "Failed to ping the device");
        return -1;
    }

    // Run the view dispatcher
    view_dispatcher_run(app->view_dispatcher);

    // Free the resources used by the FlipWiFi application
    flip_wifi_app_free(app);

    // Return 0 to indicate success
    return 0;
}
