#include <flip_library_e.h>
#include <flip_library_storage.h>
#include <flip_library_callback.h>
#include <flip_library_i.h>
#include <flip_library_free.h>

// Entry point for the FlipLibrary application
int32_t flip_library_app(void* p) {
    // Suppress unused parameter warning
    UNUSED(p);

    // Initialize the FlipLibrary application
    FlipLibraryApp* app = flip_library_app_alloc();
    if(!app) {
        FURI_LOG_E(TAG, "Failed to allocate FlipLibraryApp");
        return -1;
    }

    if(!flipper_http_ping()) {
        FURI_LOG_E(TAG, "Failed to ping the device");
        return -1;
    }

    // Run the view dispatcher
    view_dispatcher_run(app->view_dispatcher);

    // Free the resources used by the FlipLibrary application
    flip_library_app_free(app);

    // Return 0 to indicate success
    return 0;
}
