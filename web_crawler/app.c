#include <web_crawler_e.h>
#include <flipper_http.h>
#include <web_crawler_storage.h>
#include <web_crawler_free.h>
#include <web_crawler_callback.h>
#include <web_crawler_i.h>
/**
 * @brief      Entry point for the WebCrawler application.
 * @param      p  Input parameter - unused
 * @return     0 to indicate success, -1 on failure
 */
int32_t web_crawler_app(void* p) {
    UNUSED(p);

    WebCrawlerApp* app = web_crawler_app_alloc();
    if(!app) {
        FURI_LOG_E(TAG, "Failed to allocate WebCrawlerApp");
        return -1;
    }

    if(!flipper_http_ping()) {
        FURI_LOG_E(TAG, "Failed to ping the device");
        return -1;
    }

    // Run the application
    view_dispatcher_run(app->view_dispatcher);

    // Free resources after the application loop ends
    web_crawler_app_free(app);

    return 0;
}
