// flip_social_e.h
#ifndef FLIP_SOCIAL_E
#define FLIP_SOCIAL_E
#include <easy_flipper.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <flipper_http.h>
#include <input/input.h>
#include <flip_social_icons.h>
#define TAG "FlipSocial"

#define MAX_PRE_SAVED_MESSAGES 25 // Maximum number of pre-saved messages
#define MAX_MESSAGE_LENGTH     100 // Maximum length of a message in the feed
#define MAX_EXPLORE_USERS      50 // Maximum number of users to explore
#define MAX_USER_LENGTH        32 // Maximum length of a username
#define MAX_FRIENDS            50 // Maximum number of friends
#define MAX_TOKENS             450 // Adjust based on expected JSON tokens
#define MAX_FEED_ITEMS         41 // Maximum number of feed items
#define MAX_LINE_LENGTH        30
#define MAX_MESSAGE_USERS      20 // Maximum number of users to display in the submenu
#define MAX_MESSAGES           20 // Maximum number of meesages between each user

// Define the submenu items for our Hello World application
typedef enum {
    FlipSocialSubmenuLoggedOutIndexLogin, // click to go to the login screen
    FlipSocialSubmenuLoggedOutIndexRegister, // click to go to the register screen
    FlipSocialSubmenuLoggedOutIndexAbout, // click to go to the about screen
    FlipSocialSubmenuLoggedOutIndexWifiSettings, // click to go to the wifi settings screen
    //
    FlipSocialSubmenuLoggedInIndexProfile, // click to go to the profile screen
    FlipSocialSubmenuExploreIndex, // click to go to the explore
    FlipSocialSubmenuLoggedInIndexFeed, // click to go to the feed screen
    FlipSocialSubmenuLoggedInIndexMessages, // click to go to the messages screen
    FlipSocialSubmenuLoggedInIndexCompose, // click to go to the compose screen
    FlipSocialSubmenuLoggedInIndexSettings, // click to go to the settings screen
    FlipSocialSubmenuLoggedInSignOutButton, // click to sign out
    //
    FlipSocialSubmenuLoggedInIndexMessagesNewMessage, // click to add a new message
    //
    FlipSocialSubmenuComposeIndexAddPreSave, // click to add a pre-saved message
    FlipSocialSubemnuComposeIndexStartIndex =
        100, // starting index for the first pre saved message
    //
    FlipSocialSubmenuExploreIndexStartIndex = 150, // starting index for the users to explore
    //
    FlipSocialSubmenuLoggedInIndexFriendsStart = 200, // starting index for the friends
    //
    FlipSocialSubmenuLoggedInIndexMessagesUsersStart = 250, // starting index for the messages
    //
    FlipSocialSubmenuLoggedInIndexMessagesUserChoicesIndexStart =
        300, // click to select a user to message
} FlipSocialSubmenuIndex;

typedef enum {
    ActionNone,
    ActionBack,
    ActionNext,
    ActionPrev,
    ActionFlip,
} Action;

static Action action = ActionNone;

// Define the ScriptPlaylist structure
typedef struct {
    char* messages[MAX_PRE_SAVED_MESSAGES];
    size_t count;
} PreSavedPlaylist;

typedef struct {
    char* usernames[MAX_FEED_ITEMS];
    char* messages[MAX_FEED_ITEMS];
    bool is_flipped[MAX_FEED_ITEMS];
    int ids[MAX_FEED_ITEMS];
    int flips[MAX_FEED_ITEMS];
    size_t count;
    size_t index;
} FlipSocialFeed;

typedef struct {
    char* usernames[MAX_EXPLORE_USERS];
    int count;
    int index;
} FlipSocialModel;

typedef struct {
    char* usernames[MAX_MESSAGE_USERS];
    int count;
    int index;
} FlipSocialModel2;

typedef struct {
    char* usernames[MAX_MESSAGES];
    char* messages[MAX_MESSAGES];
    int count;
    int index;
} FlipSocialMessage;

// Define views for our Hello World application
typedef enum {
    FlipSocialViewLoggedOutSubmenu, // The menu if the user is not logged in
    FlipSocialViewLoggedOutLogin, // The login screen
    FlipSocialViewLoggedOutRegister, // The register screen
    FlipSocialViewLoggedOutAbout, // The about screen
    FlipSocialViewLoggedOutWifiSettings, // The wifi settings screen
    //
    FlipSocialViewLoggedOutLoginUsernameInput, // Text input screen for username input on login screen
    FlipSocialViewLoggedOutLoginPasswordInput, // Text input screen for password input on login screen
    FlipSocialViewLoggedOutRegisterUsernameInput, // Text input screen for username input on register screen
    FlipSocialViewLoggedOutRegisterPasswordInput, // Text input screen for password input on register screen
    FlipSocialViewLoggedOutRegisterPassword2Input, // Text input screen for password 2 input on register screen
    FlipSocialViewLoggedOutWifiSettingsSSIDInput, // Text input screen for SSID input on wifi screen
    FlipSocialViewLoggedOutWifiSettingsPasswordInput, // Text input screen for Password input on wifi screen
    FlipSocialViewLoggedOutProcessLogin, // The screen displayed after clicking login
    FlipSocialViewLoggedOutProcessRegister, // The screen displayed after clicking register
    //
    FlipSocialViewLoggedInSubmenu, // The menu if the user is logged in
    FlipSocialViewLoggedInProfile, // The profile screen
    FlipSocialViewLoggedInFeed, // The feed screen
    FlipSocialViewLoggedInCompose, // The compose screen
    FlipSocialViewLoggedInSettings, // The settings screen
    //
    FlipSocialViewLoggedInChangePasswordInput, // Text input screen for password input on change password screen
    FlipSocialViewLoggedInComposeAddPreSaveInput, // Text input screen for add text input on compose screen
    //
    FlipSocialViewLoggedInMessagesNewMessageInput, // Text input screen for new message input on messages screen
    FlipSocialViewLoggedInMessagesNewMessageUserChoicesInput, // Text input screen for new message input on messages screen
    FlipSocialViewLoggedInMessagesUserChoices, // the view after clicking [New Message] - select a user to message, then direct to input view
    //
    FlipSocialViewLoggedInSettingsAbout, // The about screen
    FlipSocialViewLoggedInSettingsWifi, // The wifi settings screen
    FlipSocialViewLoggedInWifiSettingsSSIDInput, // Text input screen for SSID input on wifi screen
    FlipSocialViewLoggedInWifiSettingsPasswordInput, // Text input screen for Password input on wifi screen
    FlipSocialViewLoggedInProcessCompose, // The dialog view to delete or send the clicked pre-saved text
    //
    FlipSocialViewLoggedInSignOut, // The view after clicking the sign out button
    //
    FlipSocialViewLoggedInExploreSubmenu, // The view after clicking the explore button
    FlipSocialViewLoggedInExploreProccess, // The view after clicking on a user in the explore screen
    FlipSocialViewLoggedInFriendsSubmenu, // The view after clicking the friends button on the profile screen
    FlipSocialViewLoggedInFriendsProcess, // The view after clicking on a friend in the friends screen
    FlipSocialViewLoggedInMessagesSubmenu, // The view after clicking the messages button on the profile screen
    FlipSocialViewLoggedInMessagesProcess, // The view after clicking on a user in the messages screen
} FlipSocialView;

// Define the application structure
typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    Submenu* submenu_logged_out; // The application submenu (logged out)
    Submenu* submenu_logged_in; // The application submenu (logged in)
    Submenu* submenu_compose; // The application submenu (compose)
    Submenu* submenu_explore; // The application submenu (explore)
    Submenu* submenu_friends; // The application submenu (friends)
    Submenu* submenu_messages; // The application submenu (messages)
    Submenu* submenu_messages_user_choices; // The application submenu (messages user choices)
    Widget* widget_logged_out_about; // The about screen (logged out)
    Widget* widget_logged_in_about; // The about screen (logged in)

    View* view_process_login; // The screen displayed after clicking login
    View* view_process_register; // The screen displayed after clicking register
    View* view_process_feed; // Dialog for the feed screen
    View* view_process_compose; // Dialog for the compose screen (delete or send)
    View* view_process_explore; // Dialog for the explore screen (view user profile - add or delete friend)
    View* view_process_friends; // Dialog for the friends screen (view user profile - add or delete friend)
    View* view_process_messages; // Dialog for the messages screen (next, previous, send message)

    VariableItemList* variable_item_list_logged_out_wifi_settings; // The wifi settings menu
    VariableItemList* variable_item_list_logged_out_login; // The login menu
    VariableItemList* variable_item_list_logged_out_register; // The register menu
    //
    VariableItemList* variable_item_list_logged_in_profile; // The profile menu
    VariableItemList* variable_item_list_logged_in_settings; // The settings menu
    VariableItemList* variable_item_list_logged_in_settings_wifi; // The wifi settings menu

    TextInput*
        text_input_logged_out_wifi_settings_ssid; // Text input for ssid input on wifi settings screen
    TextInput*
        text_input_logged_out_wifi_settings_password; // Text input for password input on wifi settings screen
    TextInput* text_input_logged_out_login_username; // Text input for username input on login screen
    TextInput* text_input_logged_out_login_password; // Text input for password input on login screen
    TextInput*
        text_input_logged_out_register_username; // Text input for username input on register screen
    TextInput*
        text_input_logged_out_register_password; // Text input for password input on register screen
    TextInput*
        text_input_logged_out_register_password_2; // Text input for password 2 input on register screen
    //
    TextInput*
        text_input_logged_in_change_password; // Text input for password input on change password screen
    TextInput*
        text_input_logged_in_compose_pre_save_input; // Text input for pre save input on compose screen
    TextInput*
        text_input_logged_in_wifi_settings_ssid; // Text input for ssid input on wifi settings screen
    TextInput*
        text_input_logged_in_wifi_settings_password; // Text input for password input on wifi settings screen
    //
    TextInput*
        text_input_logged_in_messages_new_message; // Text input for new message input on messages screen
    TextInput* text_input_logged_in_messages_new_message_user_choices; //

    VariableItem*
        variable_item_logged_out_wifi_settings_ssid; // Reference to the ssid configuration item
    VariableItem*
        variable_item_logged_out_wifi_settings_password; // Reference to the password configuration item
    VariableItem*
        variable_item_logged_out_login_username; // Reference to the username configuration item
    VariableItem*
        variable_item_logged_out_login_password; // Reference to the password configuration item
    VariableItem*
        variable_item_logged_out_login_button; // Reference to the login button configuration item
    VariableItem*
        variable_item_logged_out_register_username; // Reference to the username configuration item
    VariableItem*
        variable_item_logged_out_register_password; // Reference to the password configuration item
    VariableItem*
        variable_item_logged_out_register_password_2; // Reference to the password 2 configuration item
    VariableItem*
        variable_item_logged_out_register_button; // Reference to the register button configuration item
    //
    VariableItem*
        variable_item_logged_in_profile_username; // Reference to the username configuration item
    VariableItem*
        variable_item_logged_in_profile_change_password; // Reference to the change password configuration item
    VariableItem*
        variable_item_logged_in_settings_about; // Reference to the about configuration item
    VariableItem*
        variable_item_logged_in_settings_wifi; // Reference to the wifi settings configuration item
    VariableItem*
        variable_item_logged_in_wifi_settings_ssid; // Reference to the ssid configuration item
    VariableItem*
        variable_item_logged_in_wifi_settings_password; // Reference to the password configuration item
    //
    VariableItem*
        variable_item_logged_in_profile_friends; // Reference to the friends configuration item
    //
    FuriPubSub* input_event_queue;
    FuriPubSubSubscription* input_event;

    PreSavedPlaylist pre_saved_messages; // Pre-saved messages for the feed screen

    char* is_logged_in; // Store the login status
    uint32_t is_logged_in_size; // Size of the login status buffer

    char* login_username_logged_in; // Store the entered login username
    char* login_username_logged_in_temp_buffer; // Temporary buffer for login username text input
    uint32_t
        login_username_logged_in_temp_buffer_size; // Size of the login username temporary buffer

    char* wifi_ssid_logged_out; // Store the entered wifi ssid
    char* wifi_ssid_logged_out_temp_buffer; // Temporary buffer for wifi ssid text input
    uint32_t wifi_ssid_logged_out_temp_buffer_size; // Size of the wifi ssid temporary buffer

    char* wifi_password_logged_out; // Store the entered wifi password
    char* wifi_password_logged_out_temp_buffer; // Temporary buffer for wifi_password text input
    uint32_t wifi_password_logged_out_temp_buffer_size; // Size of the wifi_password temporary buffer

    char* login_username_logged_out; // Store the entered login username
    char* login_username_logged_out_temp_buffer; // Temporary buffer for login username text input
    uint32_t
        login_username_logged_out_temp_buffer_size; // Size of the login username temporary buffer

    char* login_password_logged_out; // Store the entered login password
    char* login_password_logged_out_temp_buffer; // Temporary buffer for login password text input
    uint32_t
        login_password_logged_out_temp_buffer_size; // Size of the login password temporary buffer

    char* register_username_logged_out; // Store the entered register username
    char* register_username_logged_out_temp_buffer; // Temporary buffer for register username text input
    uint32_t
        register_username_logged_out_temp_buffer_size; // Size of the register username temporary buffer

    char* register_password_logged_out; // Store the entered register password
    char* register_password_logged_out_temp_buffer; // Temporary buffer for register password text input
    uint32_t
        register_password_logged_out_temp_buffer_size; // Size of the register password temporary buffer

    char* register_password_2_logged_out; // Store the entered register password 2
    char* register_password_2_logged_out_temp_buffer; // Temporary buffer for register password 2 text input
    uint32_t
        register_password_2_logged_out_temp_buffer_size; // Size of the register password 2 temporary buffer

    //
    char* change_password_logged_in; // Store the entered change password
    char* change_password_logged_in_temp_buffer; // Temporary buffer for change password text input
    uint32_t
        change_password_logged_in_temp_buffer_size; // Size of the change password temporary buffer

    char* compose_pre_save_logged_in; // Store the entered add text
    char* compose_pre_save_logged_in_temp_buffer; // Temporary buffer for add text text input
    uint32_t compose_pre_save_logged_in_temp_buffer_size; // Size of the add text temporary buffer

    char* wifi_ssid_logged_in; // Store the entered wifi ssid
    char* wifi_ssid_logged_in_temp_buffer; // Temporary buffer for wifi ssid text input
    uint32_t wifi_ssid_logged_in_temp_buffer_size; // Size of the wifi ssid temporary buffer

    char* wifi_password_logged_in; // Store the entered wifi password
    char* wifi_password_logged_in_temp_buffer; // Temporary buffer for wifi_password text input
    uint32_t wifi_password_logged_in_temp_buffer_size; // Size of the wifi_password temporary buffer

    //
    char* messages_new_message_logged_in; // Store the entered new message
    char* messages_new_message_logged_in_temp_buffer; // Temporary buffer for new message text input
    uint32_t
        messages_new_message_logged_in_temp_buffer_size; // Size of the new message temporary buffer

    char* message_user_choice_logged_in; // Store the entered message to send to the selected user
    char* message_user_choice_logged_in_temp_buffer; // Temporary buffer for message to send to the selected user
    uint32_t
        message_user_choice_logged_in_temp_buffer_size; // Size of the message to send to the selected user temporary buffer
} FlipSocialApp;

static FlipSocialFeed* flip_social_feed = NULL; // Store the feed
static FlipSocialModel* flip_social_friends = NULL; // Store the friends
static FlipSocialModel2* flip_social_message_users =
    NULL; // Store the users that have sent messages to the logged in user
static FlipSocialModel* flip_social_explore = NULL; // Store the users to explore
static FlipSocialMessage* flip_social_messages =
    NULL; // Store the messages between the logged in user and the selected user

// include strndup (otherwise NULL pointer dereference)
char* strndup(const char* s, size_t n) {
    char* result;
    size_t len = strlen(s);

    if(n < len) len = n;

    result = (char*)malloc(len + 1);
    if(!result) return NULL;

    result[len] = '\0';
    return (char*)memcpy(result, s, len);
}

static FlipSocialApp* app_instance = NULL;
static void flip_social_logged_in_compose_pre_save_updated(void* context);
static void flip_social_callback_submenu_choices(void* context, uint32_t index);
#endif
