#pragma once
#include "easy_flipper/easy_flipper.h"
#include "loading/loading.hpp"

class FlipDownloaderApp;

#define MAX_APP_NAME_LENGTH 32         // maximum length of app name
#define MAX_ID_LENGTH 32               // maximum length of app ID
#define MAX_APP_DESCRIPTION_LENGTH 100 // maximum length of app description
#define MAX_APP_VERSION_LENGTH 5       // maximum length of app version
#define MAX_RECEIVED_APPS 4            // maximum number of apps received per request
#define MAX_GITHUB_REPO_FILES 30       // maximum number of files in a GitHub repository

typedef struct
{
    char app_name[MAX_APP_NAME_LENGTH];
    char app_id[MAX_APP_NAME_LENGTH];
    char app_build_id[MAX_ID_LENGTH];
    char app_version[MAX_APP_VERSION_LENGTH];
    char app_description[MAX_APP_DESCRIPTION_LENGTH];
} FlipperAppInfo;

typedef enum
{
    RunViewMainMenu,
    RunViewCatalog,
    RunViewESP32,
    RunViewVGM,
    RunViewGitHubAuthor,
    RunViewGitHubRepo,
    RunViewGitHubProgress,
    RunViewDownloadProgress,
    RunViewApps,
} FlipDownloaderRunView;

typedef enum
{
    // ESP32
    DownloadLinkFirmwareMarauderLink1,
    DownloadLinkFirmwareMarauderLink2,
    DownloadLinkFirmwareMarauderLink3,
    DownloadLinkFirmwareFlipperHTTPLink1,
    DownloadLinkFirmwareFlipperHTTPLink2,
    DownloadLinkFirmwareFlipperHTTPLink3,
    DownloadLinkFirmwareBlackMagicLink1,
    DownloadLinkFirmwareBlackMagicLink2,
    DownloadLinkFirmwareBlackMagicLink3,
    // VGM
    DownloadLinkVGMFlipperHTTP,
    DownloadLinkVGMPicoware
} FlipDownloaderDownloadLink;

typedef enum
{
    CategoryUnknown = -1,
    CategoryBluetooth = 0,
    CategoryGames,
    CategoryGPIO,
    CategoryInfrared,
    CategoryIButton,
    CategoryMedia,
    CategoryNFC,
    CategoryRFID,
    CategorySubGHz,
    CategoryTools,
    CategoryUSB,
} FlipperAppCategory;

class FlipDownloaderRun
{
    void *appContext = nullptr;                           // Pointer to the app context
    uint8_t currentAppIteration = 0;                      // Current app iteration for pagination
    FlipperAppCategory currentCategory = CategoryUnknown; // Current selected category
    uint8_t currentDownloadIndex;                         // Current download being processed
    uint8_t currentView = RunViewMainMenu;                // Current view in the run
    FlipDownloaderDownloadLink downloadQueue[3];          // Queue of files to download
    uint8_t downloadQueueSize;                            // Number of items in download queue
    bool isProcessingQueue;                               // Flag to indicate if processing download queue
    char github_author[64];                               // GitHub author buffer
    char github_repo[64];                                 // GitHub repo buffer
    int github_current_file = 0;                          // Current file being downloaded from GitHub
    int github_total_files = 0;                           // Total files to download from GitHub
    uint8_t github_download_delay_counter = 0;            // Delay counter to prevent rapid-fire downloads
    uint16_t idleCheckCounter = 0;                        // Counter to delay completion check
    bool isDownloading = false;                           // Flag to indicate if a download is in progress
    bool isDownloadComplete = false;                      // Flag to indicate if the download is complete
    bool isGitHubDownloading = false;                     // Flag to indicate if a GitHub file download is in progress
    bool isGitHubDownloadComplete = false;                // Flag to indicate if GitHub downloads are complete
    bool isDownloadingIndividualApp = false;              // Flag to indicate if downloading an individual app
    bool isLoadingNextApps = false;                       // Flag to indicate if loading next batch of apps
    std::unique_ptr<Loading> loading;                     // Loading screen instance
    char savePath[256];                                   // Buffer to store the save path
    uint8_t selectedIndexMain = 0;                        // Currently selected menu item
    uint8_t selectedIndexCatalog = 0;                     // Currently selected catalog item
    uint8_t selectedIndexESP32 = 0;                       // Currently selected ESP32 item
    uint8_t selectedIndexVGM = 0;                         // Currently selected VGM item
    uint8_t selectedIndexApps = 0;                        // Currently selected app item
    bool shouldReturnToMenu = false;                      // Flag to signal return to menu
    uint8_t text_input_cursor_x;                          // X position of cursor on virtual keyboard
    uint8_t text_input_cursor_y;                          // Y position of cursor on virtual keyboard
    uint8_t text_input_keyboard_mode;                     // 0=lowercase, 1=uppercase, 2=numbers
    bool text_input_caps_lock;                            // Caps lock state for persistent uppercase

    //
    void clearDownloadQueue();                                                                          // Clear the download queue
    uint8_t countAppsInCategory(FlipperAppCategory category);                                           // Count the actual number of apps in a category
    int countChar(const char *s, char c);                                                               // Count occurrences of a character in a string
    bool createDirectory(const char *dirPath);                                                          // Create a directory for saving files
    void deleteFile(const char *filePath);                                                              // Delete a file from the storage
    void drawDownloadProgress(Canvas *canvas);                                                          // Draw the download progress
    void drawMainMenu(Canvas *canvasx);                                                                 // Draw the main menu
    void drawMenu(Canvas *canvas, uint8_t selectedIndex, const char **menuItems, uint8_t menuCount);    // Generic menu drawer
    void drawMenuCatalog(Canvas *canvasx);                                                              // Draw the app catalog menu
    void drawMenuESP32(Canvas *canvasx);                                                                // Draw the firmware menu
    void drawMenuVGM(Canvas *canvasx);                                                                  // Draw the VGM menu
    void drawApps(Canvas *canvas);                                                                      // Draw the apps view
    void drawGitHubInput(Canvas *canvas);                                                               // Draw GitHub input screen
    void drawGitHubProgress(Canvas *canvas);                                                            // Draw GitHub download progress
    void handleGitHubKeyboardInput(InputEvent *event);                                                  // Handle virtual keyboard input for GitHub author/repo
    bool downloadApp(const char *appId, const char *buildId, const char *category);                     // Download an app based on the category and index
    bool downloadCategoryInfo(FlipperAppCategory category, uint8_t iteration);                          // Download category information based on the category and iteration
    bool downloadFile(FlipDownloaderDownloadLink link);                                                 // Download a file based on the link type
    const char *findNthArrayObject(const char *text, uint8_t index);                                    // Find the nth object in an array
    const char *findStringValue(const char *text, const char *key, char *buffer, size_t bufferSize);    // Find a string value by key in a text buffer
    const char *getAppCategory(FlipperAppCategory category);                                            // Get the app category based on the index
    FlipperAppCategory getAppCategory(const char *category);                                            // Get the app category based on the ID
    const char *getAppCategoryId(const char *category);                                                 // Get the app category ID based on the category name
    const char *getAppCategoryId(FlipperAppCategory category);                                          // Get the app category ID based on the index
    std::unique_ptr<FlipperAppInfo> getAppInfo(FlipperAppCategory category, uint8_t index);             // Get the saved/fetched app info based on the category and index
    bool githubDownloadRepositoryFile(const char *author, const char *repo, int fileIndex);             // Download a file from a GitHub repository
    bool githubFetchRepositoryInfo(const char *author, const char *repo);                               // Fetch the contents of a GitHub repository and create necessary directories
    bool hasAppsAvailable(FlipperAppCategory category);                                                 // Check if any apps are available in a category
    bool loadFileChunk(const char *filePath, char *buffer, size_t sizeOfChunk, uint8_t iteration);      // Load a file chunk from storage
    bool loadFileChunkFromOffset(const char *filePath, char *buffer, size_t bufferSize, size_t offset); // Load a file chunk from a specific offset
    void processDownloadQueue();                                                                        // Process the next item in the download queue
    bool githubParseRepositoryInfo(const char *author, const char *repo);                               // Parse the GitHub repository info and create necessary files
    void queueDownload(FlipDownloaderDownloadLink link);                                                // Add a file to the download queue
    bool saveCharWithPath(const char *fullPath, const char *value);                                     // Save a string to storage with a specific path
    void setCategorySavePath(FlipperAppCategory category);                                              // Set the save path based on the category
    void setSavePath(FlipDownloaderDownloadLink link);                                                  // Get the download link based on the enum type
    void startDownloadQueue();                                                                          // Start processing the download queue
    void startGitHubDownload();                                                                         // start GitHub repository download
    bool startTextInput(uint32_t view);                                                                 // start the text input for a specific Github view

public:
    FlipDownloaderRun();
    ~FlipDownloaderRun();
    //
    bool init(void *appContext);                                  // Initialize the run with app context
    bool isActive() const { return shouldReturnToMenu == false; } // Check if the run is active
    void updateDraw(Canvas *canvas);                              // update and draw the run
    void updateInput(InputEvent *event);                          // update input for the run
};
