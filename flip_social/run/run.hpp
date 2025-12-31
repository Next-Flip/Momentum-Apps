#pragma once
#include "easy_flipper/easy_flipper.h"
#include "loading/loading.hpp"
#include "run/keyboard.hpp"

#define MAX_PRE_SAVED_MESSAGES 20 // Maximum number of pre-saved messages
#define MAX_MESSAGE_LENGTH 100    // Maximum length of a message in the feed
#define MAX_EXPLORE_USERS 50      // Maximum number of users to explore
#define MAX_USER_LENGTH 32        // Maximum length of a username
#define MAX_FRIENDS 50            // Maximum number of friends
#define MAX_FEED_ITEMS 25         // Maximum number of feed items
#define MAX_MESSAGE_USERS 40      // Maximum number of users to display in the submenu
#define MAX_MESSAGES 20           // Maximum number of messages between each user
#define MAX_COMMENTS 20           // Maximum number of comments per feed item

typedef enum
{
    SocialViewMenu = -1,        // main menu view
    SocialViewFeed = 0,         // feed view
    SocialViewPost = 1,         // post view
    SocialViewMessageUsers = 2, // (initial) messages view
    SocialViewExplore = 3,      // explore view
    SocialViewProfile = 4,      // profile view
    SocialViewLogin = 5,        // login view
    SocialViewRegistration = 6, // registration view
    SocialViewUserInfo = 7,     // user info view
    SocialViewMessages = 8,     // messages view
    SocialViewComments = 9,     // comments view
} SocialView;

typedef enum
{
    LoginCredentialsMissing = -1, // Credentials missing
    LoginSuccess = 0,             // Login successful
    LoginUserNotFound = 1,        // User not found
    LoginWrongPassword = 2,       // Wrong password
    LoginWaiting = 3,             // Waiting for response
    LoginNotStarted = 4,          // Login not started
    LoginRequestError = 5,        // Request error
} LoginStatus;

typedef enum
{
    RegistrationCredentialsMissing = -1,      // Credentials missing
    RegistrationSuccess = 0,                  // Registration successful
    RegistrationUserExists = 1,               // User already exists
    RegistrationRequestError = 2,             // Request error
    RegistrationNotStarted = 3,               // Registration not started
    RegistrationWaiting = 4,                  // Waiting for response
    RegistrationErrorAllOneLetter = 5,        // Error: All one letter username/password
    RegistrationErrorAllNumbers = 6,          // Error: All numbers username/password
    RegistrationErrorUsernameTooLong = 7,     // Error: Username too long
    RegistrationErrorUsernameTooShort = 8,    // Error: Username too short
    RegistrationErrorPasswordTooLong = 9,     // Error: Password too long
    RegistrationErrorPasswordTooShort = 10,   // Error: Password too short
    RegistrationErrorUsernameNotAllowed = 11, // Error: Username not allowed
} RegistrationStatus;

typedef enum
{
    UserInfoCredentialsMissing = -1, // Credentials missing
    UserInfoSuccess = 0,             // User info fetched successfully
    UserInfoRequestError = 1,        // Request error
    UserInfoNotStarted = 2,          // User info request not started
    UserInfoWaiting = 3,             // Waiting for response
    UserInfoParseError = 4,          // Error parsing user info
} UserInfoStatus;

typedef enum
{
    RequestTypeLogin = 0,            // Request login (login the user)
    RequestTypeRegistration = 1,     // Request registration (register the user)
    RequestTypeUserInfo = 2,         // Request user info (fetch user info)
    RequestTypeFeed = 3,             // Request feed (fetch user feed)
    RequestTypeFlipPost = 4,         // Request flip post (flip the current seelected post)
    RequestTypeCommentFetch = 5,     // Request comments (fetch comments for a post)
    RequestTypeCommentPost = 6,      // Request comment post (post a comment on a post)
    RequestTypeCommentFlip = 7,      // Request comment flip (flip a comment)
    RequestTypeMessagesUserList = 8, // Request messages (fetch list of users who sent messages)
    RequestTypeMessagesWithUser = 9, // Request messages with a specific user
    RequestTypeMessageSend = 10,     // Request to send a message to the current user
    RequestTypeExplore = 11,         // Request explore (fetch users to explore)
    RequestTypePost = 12,            // Request post (send a post to the feed)
} RequestType;

typedef enum
{
    ProfileElementBio,
    ProfileElementFriends,
    ProfileElementJoined,
    ProfileElementMAX
} ProfileElement;

typedef enum
{
    FeedNotStarted = 0,   // Feed not started
    FeedWaiting = 1,      // Waiting for feed response
    FeedSuccess = 2,      // Feed fetched successfully
    FeedParseError = 3,   // Error parsing feed
    FeedRequestError = 4, // Error in feed request
} FeedStatus;

typedef enum
{
    MessageUsersNotStarted = 0,   // Messages not started
    MessageUsersWaiting = 1,      // Waiting for messages response
    MessageUsersSuccess = 2,      // Messages fetched successfully
    MessageUsersParseError = 3,   // Error parsing messages
    MessageUsersRequestError = 4, // Error in messages request
} MessageUsersStatus;

typedef enum
{
    MessagesNotStarted = 0,   // Messages not started
    MessagesWaiting = 1,      // Waiting for messages response
    MessagesSuccess = 2,      // Messages fetched successfully
    MessagesParseError = 3,   // Error parsing messages
    MessagesRequestError = 4, // Error in messages request
    MessagesKeyboard = 5,     // Keyboard for messages view (sending messages)
    MessagesSending = 6,      // Sending message
} MessagesStatus;

typedef enum
{
    ExploreNotStarted = 0,      // Explore not started (here after keyboard)
    ExploreWaiting = 1,         // Waiting for explore response
    ExploreSuccess = 2,         // Explore fetched successfully
    ExploreParseError = 3,      // Error parsing explore data
    ExploreRequestError = 4,    // Error in explore request
    ExploreKeyboardUsers = 5,   // Keyboard for explore view (we'll start here)
    ExploreKeyboardMessage = 6, // Keyboard for explore view (sending messages)
    ExploreSending = 7,         // Sending message in explore view
} ExploreStatus;

typedef enum
{
    PostNotStarted = 0,   // Post not started
    PostWaiting = 1,      // while message is sending, we're waiting
    PostSuccess = 2,      // Post sent successfully
    PostParseError = 3,   // Error parsing post data
    PostRequestError = 4, // Error in post request
    PostKeyboard = 5,     // Keyboard for post view (to create a new pre-saved post only)
    PostChoose = 6,       // Choosing a pre-saved post to send
} PostStatus;

typedef enum
{
    CommentsNotStarted = 0,   // Comment not started (send request to fetch comments) - start here
    CommentsWaiting = 1,      // Wait for fetch comments request to finish
    CommentsSuccess = 2,      // Comments fetched successfully
    CommentsParseError = 3,   // Error parsing fetched comments
    CommentsRequestError = 4, // Error in comment request
    CommentsKeyboard = 5,     // Keyboard for comment view (to create a new comment)
    CommentsSending = 6,      // Sending comment
} CommentsStatus;

class FlipSocialApp;

class FlipSocialRun
{
    void *appContext;                                // reference to the app context
    uint8_t commentsIndex;                           // current comment index
    bool commentIsValid;                             // flag to check if the comment is valid
    uint16_t commentItemID;                          // current comment item ID
    CommentsStatus commentsStatus;                   // current comment status
    SocialView currentMenuIndex;                     // current menu index
    uint8_t currentProfileElement;                   // current profile element being viewed
    SocialView currentView;                          // current view of the social run
    uint8_t exploreIndex;                            // current explore menu index
    ExploreStatus exploreStatus;                     // current explore status
    uint16_t feedItemID;                             // current feed item ID
    uint8_t feedItemIndex;                           // current feed item index
    uint8_t feedIteration;                           // current feed iteration
    FeedStatus feedStatus;                           // current feed status
    bool feedItemFlipOverride[MAX_FEED_ITEMS];       // local override for flip status to show immediate feedback
    bool feedItemFlipOverrideActive[MAX_FEED_ITEMS]; // track which items have local overrides
    bool inputHeld;                                  // flag to check if input is held
    InputKey lastInput;                              // last input key pressed
    std::unique_ptr<Keyboard> keyboard;              // keyboard instance for input handling
    std::unique_ptr<Loading> loading;                // loading animation instance
    LoginStatus loginStatus;                         // current login status
    MessagesStatus messagesStatus;                   // current messages status
    MessageUsersStatus messageUsersStatus;           // current messages status
    uint8_t messagesIndex;                           // index of the message in the messages submenu
    uint8_t messageUserIndex;                        // index of the user in the Message Users submenu
    uint8_t postIndex;                               // index of the post in the Post submenu
    PostStatus postStatus;                           // current post status
    RegistrationStatus registrationStatus;           // current registration status
    bool shouldDebounce;                             // flag to debounce input
    bool shouldReturnToMenu;                         // Flag to signal return to menu
    UserInfoStatus userInfoStatus;                   // current user info status
    //
    void debounceInput();                                                                                                                                     // debounce input to prevent multiple triggers
    void drawCommentsView(Canvas *canvas);                                                                                                                    // draw the comments view
    void drawExploreView(Canvas *canvas);                                                                                                                     // draw the explore view
    void drawFeedItem(Canvas *canvas, char *username, char *message, char *flipped, char *flips, char *date_created, char *comments, bool isComment = false); // draw a single feed item
    void drawFeedMessage(Canvas *canvas, const char *user_message, int x, int y);                                                                             // draw the feed message with wrapping
    void drawFeedView(Canvas *canvas);                                                                                                                        // draw the feed view
    void drawLoginView(Canvas *canvas);                                                                                                                       // draw the login view
    void drawMainMenuView(Canvas *canvas);                                                                                                                    // draw the main menu view
    void drawMessagesView(Canvas *canvas);                                                                                                                    // draw the messages view
    void drawMessageUsersView(Canvas *canvas);                                                                                                                // draw the message users view
    void drawPostView(Canvas *canvas);                                                                                                                        // draw the post view
    void drawProfileView(Canvas *canvas);                                                                                                                     // draw the profile view
    void drawRegistrationView(Canvas *canvas);                                                                                                                // draw the registration view
    void drawUserInfoView(Canvas *canvas);                                                                                                                    // draw the user info view
    void drawWrappedBio(Canvas *canvas, const char *text, uint8_t x, uint8_t y);                                                                              // draw wrapped text on the canvas
    bool getMessageUser(char *buffer, size_t buffer_size);                                                                                                    // get the message user at the specified messageUserIndex
    bool getSelectedPost(char *buffer, size_t buffer_size);                                                                                                   // get the selected post at the specified postIndex
    bool httpRequestIsFinished();                                                                                                                             // check if the HTTP request is finished
    void updateFeedItemFlipStatus();                                                                                                                          // update the flip status of the current feed item in cached data
    void userRequest(RequestType requestType);                                                                                                                // Send a user request to the server based on the request type
public:
    FlipSocialRun(void *appContext);
    ~FlipSocialRun();
    //
    void drawMenu(Canvas *canvas, uint8_t selectedIndex, const char **menuItems, uint8_t menuCount); // Generic menu drawer
    bool isActive() const { return shouldReturnToMenu == false; }                                    // Check if the run is active
    void updateDraw(Canvas *canvas);                                                                 // update and draw the run
    void updateInput(InputEvent *event);                                                             // update input for the run
};
