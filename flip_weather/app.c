#include <flip_weather_e.h>
#include <flip_weather_storage.h>
#include "flip_weather_parse.h"
#include <flip_weather_callback.h>
#include <flip_weather_i.h>
#include <flip_weather_free.h>

// Entry point for the FlipWeather application
int32_t flip_weather_app(void* p) {
    // Suppress unused parameter warning
    UNUSED(p);

    // Initialize the FlipWeather application
    FlipWeatherApp* app = flip_weather_app_alloc();
    if(!app) {
        FURI_LOG_E(TAG, "Failed to allocate FlipWeatherApp");
        return -1;
    }

    if(!flipper_http_ping()) {
        FURI_LOG_E(TAG, "Failed to ping the device");
        return -1;
    }

    // Run the view dispatcher
    view_dispatcher_run(app->view_dispatcher);

    // Free the resources used by the FlipWeather application
    flip_weather_app_free(app);

    // Return 0 to indicate success
    return 0;
}
