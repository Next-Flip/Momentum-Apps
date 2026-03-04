#include "game/player.hpp"
#include "free_roam_icons.h"
#include "game/game.hpp"
#include "game/general.hpp"
#include "app.hpp"
#include "jsmn/jsmn.h"
#include <math.h>

Player::Player() : Entity("Player", ENTITY_PLAYER, Vector(6, 6), Vector(10, 10), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, SPRITE_3D_HUMANOID)
{
    direction = Vector(1, 0);                                // facing east initially (better for 3rd person view)
    plane = Vector(0, 0.66);                                 // camera plane perpendicular to direction
    is_player = true;                                        // Mark this entity as a player (so level doesn't delete it)
    end_position = Vector(6, 6);                             // Initialize end position
    start_position = Vector(6, 6);                           // Initialize start position
    strncpy(player_name, "Player", sizeof(player_name) - 1); // Copy default player name
    player_name[sizeof(player_name) - 1] = '\0';             // Ensure null termination
    name = player_name;                                      // Point Entity's name to our writable buffer
}

Player::~Player()
{
    // nothing to clean up for now
}

bool Player::collisionMapCheck(Vector new_position)
{
    if (currentDynamicMap == nullptr)
        return false;

    // Check multiple points around the player to prevent clipping through walls
    // This accounts for player size and floating point positions
    float offset = 0.2f; // Small offset to check around player position

    Vector checkPoints[] = {
        new_position,                                             // Center
        Vector(new_position.x - offset, new_position.y - offset), // Top-left
        Vector(new_position.x + offset, new_position.y - offset), // Top-right
        Vector(new_position.x - offset, new_position.y + offset), // Bottom-left
        Vector(new_position.x + offset, new_position.y + offset)  // Bottom-right
    };

    for (int i = 0; i < 5; i++)
    {
        Vector point = checkPoints[i];

        // Ensure we're checking within bounds
        if (point.x < 0 || point.y < 0)
            return true; // Collision (out of bounds)

        uint8_t x = (uint8_t)point.x;
        uint8_t y = (uint8_t)point.y;

        // Bounds checking
        if (x >= currentDynamicMap->getWidth() || y >= currentDynamicMap->getHeight())
        {
            // Out of bounds, treat as collision
            return true;
        }

        TileType tile = currentDynamicMap->getTile(x, y);
        if (tile == TILE_WALL)
        {
            return true; // Wall blocks movement
        }
    }

    return false; // No collision detected
}

void Player::debounceInput(Game *game)
{
    static uint8_t debounceCounter = 0;
    if (shouldDebounce)
    {
        game->input = InputKeyMAX;
        debounceCounter++;
        if (debounceCounter < 4)
        {
            return;
        }
        debounceCounter = 0;
        shouldDebounce = false;
        inputHeld = false;
    }
}

void Player::drawCurrentView(Draw *canvas)
{
    if (!canvas)
        return;

    switch (currentMainView)
    {
    case GameViewTitle:
        drawTitleView(canvas);
        break;
    case GameViewSystemMenu:
        drawSystemMenuView(canvas);
        break;
    case GameViewLobbyMenu:
        drawLobbyMenuView(canvas);
        break;
    case GameViewGameLocal:
        drawGameLocalView(canvas);
        break;
    case GameViewGameOnline:
        drawGameOnlineView(canvas);
        break;
    case GameViewWelcome:
        drawWelcomeView(canvas);
        break;
    case GameViewLogin:
        drawLoginView(canvas);
        break;
    case GameViewRegistration:
        drawRegistrationView(canvas);
        break;
    case GameViewUserInfo:
        drawUserInfoView(canvas);
        break;
    default:
        canvas->fillScreen(ColorWhite);
        canvas->text(Vector(0, 10), "Unknown View", ColorBlack);
        break;
    }
}

void Player::drawGameLocalView(Draw *canvas)
{
    if (freeRoamGame->isRunning())
    {
        if (freeRoamGame->getEngine())
        {
            if (shouldLeaveGame())
            {
                freeRoamGame->endGame();
                return;
            }
            freeRoamGame->getEngine()->updateGameInput(freeRoamGame->getCurrentInput());
            // Reset the input after processing to prevent it from being continuously pressed
            freeRoamGame->resetInput();
            freeRoamGame->getEngine()->runAsync(false);
        }
        return;
    }
    else
    {
        canvas->fillScreen(ColorWhite);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(25, 32), "Starting Game...", ColorBlack);
        bool gameStarted = freeRoamGame->startGame();
        if (gameStarted && freeRoamGame->getEngine())
        {
            freeRoamGame->getEngine()->runAsync(false); // Run the game engine immediately
        }
    }
}

void Player::drawGameOnlineView(Draw *canvas)
{
    canvas->fillScreen(ColorWhite);
    canvas->setFont(FontPrimary);
    canvas->text(Vector(0, 10), "Not available yet", ColorBlack);
}

void Player::drawLobbyMenuView(Draw *canvas)
{
    // draw lobby text
    drawMenuType1(canvas, currentLobbyMenuIndex, "Local", "Online");
}

void Player::drawLoginView(Draw *canvas)
{
    canvas->fillScreen(ColorWhite);
    canvas->setFont(FontPrimary);
    static bool loadingStarted = false;
    switch (loginStatus)
    {
    case LoginWaiting:
        if (!loadingStarted)
        {
            if (!loading)
            {
                loading = std::make_unique<Loading>(canvas);
            }
            loadingStarted = true;
            if (loading)
            {
                loading->setText("Logging in...");
            }
        }
        if (!this->httpRequestIsFinished())
        {
            if (loading)
            {
                loading->animate();
            }
        }
        else
        {
            if (loading)
            {
                loading->stop();
            }
            loadingStarted = false;
            char response[256];
            FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
            if (app && app->load_char("login", response, sizeof(response)))
            {
                if (strstr(response, "[SUCCESS]") != NULL)
                {
                    loginStatus = LoginSuccess;
                    currentMainView = GameViewTitle; // switch to title view
                }
                else if (strstr(response, "User not found") != NULL)
                {
                    loginStatus = LoginNotStarted;
                    currentMainView = GameViewRegistration;
                    registrationStatus = RegistrationWaiting;
                    userRequest(RequestTypeRegistration);
                }
                else
                {
                    loginStatus = LoginRequestError;
                }
            }
            else
            {
                loginStatus = LoginRequestError;
            }
        }
        break;
    case LoginSuccess:
        canvas->text(Vector(0, 10), "Login successful!", ColorBlack);
        canvas->text(Vector(0, 20), "Press OK to continue.", ColorBlack);
        break;
    case LoginCredentialsMissing:
        canvas->text(Vector(0, 10), "Missing credentials!", ColorBlack);
        canvas->text(Vector(0, 20), "Please set your username", ColorBlack);
        canvas->text(Vector(0, 30), "and password in the app.", ColorBlack);
        break;
    case LoginRequestError:
        canvas->text(Vector(0, 10), "Login request failed!", ColorBlack);
        canvas->text(Vector(0, 20), "Check your network and", ColorBlack);
        canvas->text(Vector(0, 30), "try again later.", ColorBlack);
        break;
    default:
        canvas->text(Vector(0, 10), "Logging in...", ColorBlack);
        break;
    }
}

void Player::drawMenuType1(Draw *canvas, uint8_t selectedIndex, const char *option1, const char *option2)
{
    canvas->fillScreen(ColorWhite);

    // rain effect
    drawRainEffect(canvas);

    // draw lobby text
    if (selectedIndex == 0)
    {
        canvas->fillRect(Vector(36, 16), Vector(56, 16), ColorBlack);
        canvas->color(ColorWhite);
        canvas->text(Vector(54, 27), option1);
        canvas->fillRect(Vector(36, 32), Vector(56, 16), ColorWhite);
        canvas->color(ColorBlack);
        canvas->text(Vector(54, 42), option2);
    }
    else if (selectedIndex == 1)
    {
        canvas->color(ColorWhite);
        canvas->fillRect(Vector(36, 16), Vector(56, 16), ColorWhite);
        canvas->color(ColorBlack);
        canvas->text(Vector(54, 27), option1);
        canvas->fillRect(Vector(36, 32), Vector(56, 16), ColorBlack);
        canvas->color(ColorWhite);
        canvas->text(Vector(54, 42), option2);
        canvas->color(ColorBlack);
    }
}

void Player::drawMenuType2(Draw *canvas, uint8_t selectedIndexMain, uint8_t selectedIndexSettings)
{
    canvas->fillScreen(ColorWhite);
    canvas->color(ColorBlack);
    canvas->icon(Vector(0, 0), &I_icon_menu_128x64px);

    switch (selectedIndexMain)
    {
    case 0: // profile
    {
        // draw info
        char health[32];
        char xp[32];
        char level[32];
        char strength[32];

        snprintf(level, sizeof(level), "Level   : %d", (int)this->level);
        snprintf(health, sizeof(health), "Health  : %d", (int)this->health);
        snprintf(xp, sizeof(xp), "XP      : %d", (int)this->xp);
        snprintf(strength, sizeof(strength), "Strength: %d", (int)this->strength);

        canvas->setFont(FontPrimary);
        if (this->name == nullptr || strlen(this->name) == 0)
        {
            canvas->text(Vector(6, 16), "Unknown");
        }
        else
        {
            canvas->text(Vector(6, 16), this->name);
        }

        canvas->setFontCustom(FONT_SIZE_SMALL);
        canvas->text(Vector(6, 30), level);
        canvas->text(Vector(6, 37), health);
        canvas->text(Vector(6, 44), xp);
        canvas->text(Vector(6, 51), strength);

        // draw a box around the selected option
        canvas->drawRect(Vector(76, 6), Vector(46, 46), ColorBlack);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(80, 16), "Profile");
        canvas->setFont(FontSecondary);
        canvas->text(Vector(80, 26), "Map");
        canvas->text(Vector(80, 36), "Settings");
        canvas->text(Vector(80, 46), "About");
    }
    break;
    case 1: // map
    {
        if (currentDynamicMap != nullptr)
        {
            currentDynamicMap->renderMiniMap(canvas, Vector(3, 3), Vector(70, 57), position, direction);
        }
        else
        {
            canvas->setFont(FontPrimary);
            canvas->text(Vector(6, 32), "No map loaded");
        }

        canvas->drawRect(Vector(76, 6), Vector(46, 46), ColorBlack);
        canvas->setFont(FontSecondary);
        canvas->text(Vector(80, 16), "Profile");
        canvas->setFont(FontPrimary);
        canvas->text(Vector(80, 26), "Map");
        canvas->setFont(FontSecondary);
        canvas->text(Vector(80, 36), "Settings");
        canvas->text(Vector(80, 46), "About");
    }
    break;
    case 2: // settings (sound on/off, vibration on/off, and leave game)
    {
        char soundStatus[16];
        char vibrationStatus[16];
        snprintf(soundStatus, sizeof(soundStatus), "Sound: %s", toggleToString(soundToggle));
        snprintf(vibrationStatus, sizeof(vibrationStatus), "Vibrate: %s", toggleToString(vibrationToggle));
        // draw settings info
        switch (selectedIndexSettings)
        {
        case 0: // none/default
            canvas->setFont(FontPrimary);
            canvas->text(Vector(6, 16), "Settings");
            canvas->setFontCustom(FONT_SIZE_SMALL);
            canvas->text(Vector(6, 30), soundStatus);
            canvas->text(Vector(6, 40), vibrationStatus);
            canvas->text(Vector(6, 50), "Leave Game");
            break;
        case 1: // sound
            canvas->setFont(FontPrimary);
            canvas->text(Vector(6, 16), "Settings");
            canvas->setFontCustom(FONT_SIZE_LARGE);
            canvas->text(Vector(6, 30), soundStatus);
            canvas->setFontCustom(FONT_SIZE_SMALL);
            canvas->text(Vector(6, 40), vibrationStatus);
            canvas->text(Vector(6, 50), "Leave Game");
            break;
        case 2: // vibration
            canvas->setFont(FontPrimary);
            canvas->text(Vector(6, 16), "Settings");
            canvas->setFontCustom(FONT_SIZE_SMALL);
            canvas->text(Vector(6, 30), soundStatus);
            canvas->setFontCustom(FONT_SIZE_LARGE);
            canvas->text(Vector(6, 40), vibrationStatus);
            canvas->setFontCustom(FONT_SIZE_SMALL);
            canvas->text(Vector(6, 50), "Leave Game");
            break;
        case 3: // leave game
            canvas->setFont(FontPrimary);
            canvas->text(Vector(6, 16), "Settings");
            canvas->setFontCustom(FONT_SIZE_SMALL);
            canvas->text(Vector(6, 30), soundStatus);
            canvas->text(Vector(6, 40), vibrationStatus);
            canvas->setFontCustom(FONT_SIZE_LARGE);
            canvas->text(Vector(6, 50), "Leave Game");
            break;
        default:
            break;
        };
        canvas->drawRect(Vector(76, 6), Vector(46, 46), ColorBlack);
        canvas->setFont(FontSecondary);
        canvas->text(Vector(80, 16), "Profile");
        canvas->text(Vector(80, 26), "Map");
        canvas->setFont(FontPrimary);
        canvas->text(Vector(79, 36), "Settings");
        canvas->setFont(FontSecondary);
        canvas->text(Vector(80, 46), "About");
    }
    break;
    case 3: // about
    {
        canvas->setFont(FontPrimary);
        canvas->text(Vector(6, 16), "Free Roam");
        canvas->setFontCustom(FONT_SIZE_SMALL);
        canvas->text(Vector(6, 25), "Creator: JBlanked");
        canvas->text(Vector(6, 59), "www.github.com/jblanked");

        // draw a box around the selected option
        canvas->drawRect(Vector(76, 6), Vector(46, 46), ColorBlack);
        canvas->setFont(FontSecondary);
        canvas->text(Vector(80, 16), "Profile");
        canvas->text(Vector(80, 26), "Map");
        canvas->text(Vector(80, 36), "Settings");
        canvas->setFont(FontPrimary);
        canvas->text(Vector(80, 46), "About");
    }
    break;
    default:
        canvas->fillScreen(ColorWhite);
        canvas->text(Vector(0, 10), "Unknown Menu", ColorBlack);
        break;
    };
}

void Player::drawRainEffect(Draw *canvas)
{
    // rain droplets/star droplets effect
    Vector _pixel = Vector(0, 0);
    for (int i = 0; i < 8; i++)
    {
        // Use pseudo-random offsets based on frame and droplet index
        uint8_t seed = (rainFrame + i * 37) & 0xFF;
        uint8_t x = (rainFrame + seed * 13) & 0x7F;
        uint8_t y = (rainFrame * 2 + seed * 7 + i * 23) & 0x3F;

        // Draw star-like droplet
        _pixel.x = x;
        _pixel.y = y;
        canvas->drawPixel(_pixel, ColorBlack);
        _pixel.x = x - 1;
        canvas->drawPixel(_pixel, ColorBlack);
        _pixel.x = x + 1;
        canvas->drawPixel(_pixel, ColorBlack);
        _pixel.x = x;
        _pixel.y = y - 1;
        canvas->drawPixel(_pixel, ColorBlack);
        _pixel.y = y + 1;
        canvas->drawPixel(_pixel, ColorBlack);
    }

    rainFrame += 1;
    if (rainFrame > 128)
    {
        rainFrame = 0;
    }
}

void Player::drawRegistrationView(Draw *canvas)
{
    canvas->fillScreen(ColorWhite);
    canvas->setFont(FontPrimary);
    static bool loadingStarted = false;
    switch (registrationStatus)
    {
    case RegistrationWaiting:
        if (!loadingStarted)
        {
            if (!loading)
            {
                loading = std::make_unique<Loading>(canvas);
            }
            loadingStarted = true;
            if (loading)
            {
                loading->setText("Registering...");
            }
        }
        if (!this->httpRequestIsFinished())
        {
            if (loading)
            {
                loading->animate();
            }
        }
        else
        {
            if (loading)
            {
                loading->stop();
            }
            loadingStarted = false;
            char response[256];
            FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
            if (app && app->load_char("register", response, sizeof(response)))
            {
                if (strstr(response, "[SUCCESS]") != NULL)
                {
                    registrationStatus = RegistrationSuccess;
                    currentMainView = GameViewTitle; // switch to title view
                }
                else if (strstr(response, "Username or password not provided") != NULL)
                {
                    registrationStatus = RegistrationCredentialsMissing;
                }
                else if (strstr(response, "User already exists") != NULL)
                {
                    registrationStatus = RegistrationUserExists;
                }
                else
                {
                    registrationStatus = RegistrationRequestError;
                }
            }
            else
            {
                registrationStatus = RegistrationRequestError;
            }
        }
        break;
    case RegistrationSuccess:
        canvas->text(Vector(0, 10), "Registration successful!", ColorBlack);
        canvas->text(Vector(0, 20), "Press OK to continue.", ColorBlack);
        break;
    case RegistrationCredentialsMissing:
        canvas->text(Vector(0, 10), "Missing credentials!", ColorBlack);
        canvas->text(Vector(0, 20), "Please update your username", ColorBlack);
        canvas->text(Vector(0, 30), "and password in the settings.", ColorBlack);
        break;
    case RegistrationRequestError:
        canvas->text(Vector(0, 10), "Registration request failed!", ColorBlack);
        canvas->text(Vector(0, 20), "Check your network and", ColorBlack);
        canvas->text(Vector(0, 30), "try again later.", ColorBlack);
        break;
    default:
        canvas->text(Vector(0, 10), "Registering...", ColorBlack);
        break;
    }
}

void Player::drawSystemMenuView(Draw *canvas)
{
    canvas->fillScreen(ColorWhite);
    canvas->color(ColorBlack);
    canvas->icon(Vector(0, 0), &I_icon_menu_128x64px);

    drawMenuType2(canvas, currentMenuIndex, currentSettingsIndex);
}

void Player::drawTitleView(Draw *canvas)
{
    // draw title text
    drawMenuType1(canvas, currentTitleIndex, "Start", "Menu");
}

void Player::drawUserInfoView(Draw *canvas)
{
    static bool loadingStarted = false;
    switch (userInfoStatus)
    {
    case UserInfoWaiting:
        if (!loadingStarted)
        {
            if (!loading)
            {
                loading = std::make_unique<Loading>(canvas);
            }
            loadingStarted = true;
            if (loading)
            {
                loading->setText("Fetching...");
            }
        }
        if (!this->httpRequestIsFinished())
        {
            if (loading)
            {
                loading->animate();
            }
        }
        else
        {
            canvas->text(Vector(0, 10), "Loading user info...", ColorBlack);
            canvas->text(Vector(0, 20), "Please wait...", ColorBlack);
            canvas->text(Vector(0, 30), "It may take up to 15 seconds.", ColorBlack);
            char response[512];
            FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
            if (app && app->load_char("user_info", response, sizeof(response)))
            {
                userInfoStatus = UserInfoSuccess;
                // they're in! let's go
                char *game_stats = get_json_value("game_stats", response);
                if (!game_stats)
                {
                    FURI_LOG_E("Player", "Failed to parse game_stats");
                    userInfoStatus = UserInfoParseError;
                    if (loading)
                    {
                        loading->stop();
                    }
                    loadingStarted = false;
                    return;
                }
                canvas->fillScreen(ColorWhite);
                canvas->text(Vector(0, 10), "User info loaded!", ColorBlack);
                char *username = get_json_value("username", game_stats);
                char *level = get_json_value("level", game_stats);
                char *xp = get_json_value("xp", game_stats);
                char *health = get_json_value("health", game_stats);
                char *strength = get_json_value("strength", game_stats);
                char *max_health = get_json_value("max_health", game_stats);
                if (!username || !level || !xp || !health || !strength || !max_health)
                {
                    FURI_LOG_E("Player", "Failed to parse user info");
                    userInfoStatus = UserInfoParseError;
                    if (username)
                        ::free(username);
                    if (level)
                        ::free(level);
                    if (xp)
                        ::free(xp);
                    if (health)
                        ::free(health);
                    if (strength)
                        ::free(strength);
                    if (max_health)
                        ::free(max_health);
                    ::free(game_stats);
                    if (loading)
                    {
                        loading->stop();
                    }
                    loadingStarted = false;
                    return;
                }

                canvas->fillScreen(ColorWhite);
                canvas->text(Vector(0, 10), "User data found!", ColorBlack);

                // Update player info
                snprintf(player_name, sizeof(player_name), "%s", username);
                name = player_name;
                this->level = atoi(level);
                this->xp = atoi(xp);
                this->health = atoi(health);
                this->strength = atoi(strength);
                this->max_health = atoi(max_health);

                canvas->fillScreen(ColorWhite);
                canvas->text(Vector(0, 10), "Player info updated!", ColorBlack);

                // clean em up gang
                ::free(username);
                ::free(level);
                ::free(xp);
                ::free(health);
                ::free(strength);
                ::free(max_health);
                ::free(game_stats);

                canvas->fillScreen(ColorWhite);
                canvas->text(Vector(0, 10), "Memory freed!", ColorBlack);

                if (currentLobbyMenuIndex == LobbyMenuLocal)
                {
                    currentMainView = GameViewGameLocal; // Switch to local game view
                }
                else if (currentLobbyMenuIndex == LobbyMenuOnline)
                {
                    currentMainView = GameViewGameOnline; // Switch to online game view
                }
                if (loading)
                {
                    loading->stop();
                }
                loadingStarted = false;

                canvas->fillScreen(ColorWhite);
                canvas->text(Vector(0, 10), "User info loaded successfully!", ColorBlack);
                canvas->text(Vector(0, 20), "Please wait...", ColorBlack);
                canvas->text(Vector(0, 30), "Starting game...", ColorBlack);
                canvas->text(Vector(0, 40), "It may take up to 15 seconds.", ColorBlack);

                freeRoamGame->startGame();
                return;
            }
            else
            {
                userInfoStatus = UserInfoRequestError;
            }
        }
        break;
    case UserInfoSuccess:
        canvas->fillScreen(ColorWhite);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(0, 10), "User info loaded successfully!", ColorBlack);
        canvas->text(Vector(0, 20), "Press OK to continue.", ColorBlack);
        break;
    case UserInfoCredentialsMissing:
        canvas->fillScreen(ColorWhite);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(0, 10), "Missing credentials!", ColorBlack);
        canvas->text(Vector(0, 20), "Please update your username", ColorBlack);
        canvas->text(Vector(0, 30), "and password in the settings.", ColorBlack);
        break;
    case UserInfoRequestError:
        canvas->fillScreen(ColorWhite);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(0, 10), "User info request failed!", ColorBlack);
        canvas->text(Vector(0, 20), "Check your network and", ColorBlack);
        canvas->text(Vector(0, 30), "try again later.", ColorBlack);
        break;
    case UserInfoParseError:
        canvas->fillScreen(ColorWhite);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(0, 10), "Failed to parse user info!", ColorBlack);
        canvas->text(Vector(0, 20), "Try again...", ColorBlack);
        break;
    default:
        canvas->fillScreen(ColorWhite);
        canvas->setFont(FontPrimary);
        canvas->text(Vector(0, 10), "Loading user info...", ColorBlack);
        break;
    }
}

void Player::drawWelcomeView(Draw *canvas)
{
    canvas->fillScreen(ColorWhite);

    // rain effect
    drawRainEffect(canvas);

    // Draw welcome text with blinking effect
    // Blink every 15 frames (show for 15, hide for 15)
    canvas->setFontCustom(FONT_SIZE_SMALL);
    if ((welcomeFrame / 15) % 2 == 0)
    {
        canvas->text(Vector(34, 60), "Press OK to start", ColorBlack);
    }
    welcomeFrame++;

    // Reset frame counter to prevent overflow
    if (welcomeFrame >= 30)
    {
        welcomeFrame = 0;
    }

    // Draw a box around the OK button
    canvas->fillRect(Vector(40, 25), Vector(56, 16), ColorBlack);
    canvas->color(ColorWhite);
    canvas->text(Vector(56, 35), "Welcome");
    canvas->color(ColorBlack);
}

Vector Player::findSafeSpawnPosition(const char *levelName)
{
    Vector defaultPos = start_position;

    if (strcmp(levelName, "Tutorial") == 0)
    {
        defaultPos = Vector(6, 6); // Center of tutorial room
    }
    else if (strcmp(levelName, "First") == 0)
    {
        // Try several safe positions in the First level
        Vector candidates[] = {
            Vector(12, 12), // Upper left room
            Vector(8, 15),  // Left side of middle room
            Vector(5, 12),  // Lower in middle room
            Vector(3, 12),  // Even lower
            Vector(20, 12), // Right side of middle room
        };

        for (int i = 0; i < 5; i++)
        {
            if (isPositionSafe(candidates[i]))
            {
                return candidates[i];
            }
        }
        defaultPos = Vector(12, 12); // Fallback
    }
    else if (strcmp(levelName, "Second") == 0)
    {
        // Try several safe positions in the Second level
        Vector candidates[] = {
            Vector(12, 10), // Upper left room
            Vector(8, 8),   // Safe spot in starting room
            Vector(15, 10), // Another spot in starting room
            Vector(10, 12), // Lower in starting room
            Vector(35, 25), // Central hub
        };

        for (int i = 0; i < 5; i++)
        {
            if (isPositionSafe(candidates[i]))
            {
                return candidates[i];
            }
        }
        defaultPos = Vector(12, 10); // Fallback
    }

    return defaultPos;
}

void Player::handleMenu(Draw *draw, Game *game)
{
    if (!draw || !game)
    {
        return;
    }

    if (currentMenuIndex != MenuIndexSettings)
    {
        switch (game->input)
        {
        case InputKeyUp:
            if (currentMenuIndex > MenuIndexProfile)
            {
                currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex - 1);
            }
            shouldDebounce = true;
            break;
        case InputKeyDown:
            if (currentMenuIndex < MenuIndexAbout)
            {
                currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex + 1);
            }
            shouldDebounce = true;
            break;
        default:
            break;
        };
    }
    else
    {
        switch (currentSettingsIndex)
        {
        case MenuSettingsMain:
            // back to title, up to profile, down to settings, left to sound
            switch (game->input)
            {
            case InputKeyUp:
                if (currentMenuIndex > MenuIndexProfile)
                {
                    currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex - 1);
                }
                shouldDebounce = true;
                break;
            case InputKeyDown:
                if (currentMenuIndex < MenuIndexAbout)
                {
                    currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex + 1);
                }
                shouldDebounce = true;
                break;
            case InputKeyLeft:
                currentSettingsIndex = MenuSettingsSound; // Switch to sound settings
                shouldDebounce = true;
                break;
            default:
                break;
            };
            break;
        case MenuSettingsSound:
            // sound on/off (using OK button), down to vibration, right to MainSettingsMain
            switch (game->input)
            {
            case InputKeyOk:
            {
                // Toggle sound on/off
                soundToggle = soundToggle == ToggleOn ? ToggleOff : ToggleOn;
                shouldDebounce = true;
                // let's just make the game check if state has changed and save it
            }
            break;
            case InputKeyRight:
                currentSettingsIndex = MenuSettingsMain; // Switch back to main settings
                shouldDebounce = true;
                break;
            case InputKeyDown:
                currentSettingsIndex = MenuSettingsVibration; // Switch to vibration settings
                shouldDebounce = true;
                break;
            default:
                break;
            };
            break;
        case MenuSettingsVibration:
            // vibration on/off (using OK button), up to sound, right to MainSettingsMain, down to leave game
            switch (game->input)
            {
            case InputKeyOk:
            {
                // Toggle vibration on/off
                vibrationToggle = vibrationToggle == ToggleOn ? ToggleOff : ToggleOn;
                shouldDebounce = true;
                // let's just make the game check if state has changed and save it
            }
            break;
            case InputKeyRight:
                currentSettingsIndex = MenuSettingsMain; // Switch back to main settings
                shouldDebounce = true;
                break;
            case InputKeyUp:
                currentSettingsIndex = MenuSettingsSound; // Switch to sound settings
                shouldDebounce = true;
                break;
            case InputKeyDown:
                currentSettingsIndex = MenuSettingsLeave; // Switch to leave game settings
                shouldDebounce = true;
                break;
            default:
                break;
            };
            break;
        case MenuSettingsLeave:
            // leave game (using OK button), up to vibration, right to MainSettingsMain
            switch (game->input)
            {
            case InputKeyOk:
                // Leave game
                leaveGame = ToggleOn;
                shouldDebounce = true;
                break;
            case InputKeyRight:
                currentSettingsIndex = MenuSettingsMain; // Switch back to main settings
                shouldDebounce = true;
                break;
            case InputKeyUp:
                currentSettingsIndex = MenuSettingsVibration; // Switch to vibration settings
                shouldDebounce = true;
                break;
            default:
                break;
            };
            break;
        default:
            break;
        };
    }

    if (game->input == InputKeyOk)
    {
        switch (currentSettingsIndex)
        {
        case MenuSettingsSound:
            // Toggle sound on/off
            soundToggle = soundToggle == ToggleOn ? ToggleOff : ToggleOn;
            shouldDebounce = true;
            // let's just make the game check if state has changed and save it
            break;
        case MenuSettingsVibration:
            // Toggle vibration on/off
            vibrationToggle = vibrationToggle == ToggleOn ? ToggleOff : ToggleOn;
            shouldDebounce = true;
            // let's just make the game check if state has changed and save it
            break;
        case MenuSettingsLeave:
            leaveGame = ToggleOn;
            shouldDebounce = true;
            break;
        default:
            break;
        }
    }

    draw->fillScreen(ColorWhite);
    draw->color(ColorBlack);
    draw->icon(Vector(0, 0), &I_icon_menu_128x64px);

    drawMenuType2(draw, currentMenuIndex, currentSettingsIndex);
}

bool Player::isPositionSafe(Vector pos)
{
    if (currentDynamicMap == nullptr)
        return true;

    // Check if position is within bounds
    if (pos.x < 0 || pos.y < 0 ||
        pos.x >= currentDynamicMap->getWidth() ||
        pos.y >= currentDynamicMap->getHeight())
    {
        return false;
    }

    // Check if the tile at this position is safe (not a wall)
    TileType tile = currentDynamicMap->getTile((uint8_t)pos.x, (uint8_t)pos.y);
    return (tile != TILE_WALL);
}

void Player::processInput()
{
    if (!freeRoamGame)
    {
        return;
    }

    InputKey currentInput = lastInput;

    if (currentInput == InputKeyMAX)
    {
        return; // No input to process
    }

    switch (currentMainView)
    {
    case GameViewWelcome:
        if (currentInput == InputKeyOk)
        {
            // Check if we should attempt login or skip to title
            if (loginStatus != LoginSuccess)
            {
                // Try to login first
                currentMainView = GameViewLogin;
                loginStatus = LoginWaiting;
                userRequest(RequestTypeLogin);
            }
            else
            {
                // Already logged in, go to title
                currentMainView = GameViewTitle;
            }
            freeRoamGame->shouldDebounce = true;
        }
        else if (currentInput == InputKeyBack)
        {
            // Allow exit from welcome screen
            if (freeRoamGame)
            {
                freeRoamGame->endGame(); // This will set shouldReturnToMenu
            }
            freeRoamGame->shouldDebounce = true;
        }
        break;

    case GameViewTitle:
        // Handle title view navigation
        switch (currentInput)
        {
        case InputKeyUp:
            currentTitleIndex = TitleIndexStart;
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyDown:
            currentTitleIndex = TitleIndexMenu;
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyOk:
            switch (currentTitleIndex)
            {
            case TitleIndexStart:
                // Start button pressed - go to lobby menu
                currentMainView = GameViewLobbyMenu;
                freeRoamGame->shouldDebounce = true;
                break;
            case TitleIndexMenu:
                // Menu button pressed - go to system menu
                currentMainView = GameViewSystemMenu;
                freeRoamGame->shouldDebounce = true;
                break;
            default:
                break;
            }
            break;
        case InputKeyBack:
            freeRoamGame->endGame();
            freeRoamGame->shouldDebounce = true;
            break;
        default:
            break;
        }
        break;

    case GameViewLobbyMenu:
        // Handle lobby menu navigation with proper selection
        switch (currentInput)
        {
        case InputKeyUp:
            currentLobbyMenuIndex = LobbyMenuLocal; // Switch to local menu
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyDown:
            currentLobbyMenuIndex = LobbyMenuOnline; // Switch to online menu
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyOk:
            // 1. Switch to GameViewUserInfo
            // 2. Make a userRequest(RequestTypeUserInfo) call
            // 3. Set userInfoStatus = UserInfoWaiting
            // The user info view will then load player stats and transition to the selected game mode
            currentMainView = GameViewUserInfo;
            userInfoStatus = UserInfoWaiting;
            userRequest(RequestTypeUserInfo);
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyBack:
            currentMainView = GameViewTitle;
            freeRoamGame->shouldDebounce = true;
            break;
        default:
            break;
        }
        break;

    case GameViewSystemMenu:
        // Handle system menu with full original navigation logic
        if (currentMenuIndex != MenuIndexSettings)
        {
            switch (currentInput)
            {
            case InputKeyBack:
                currentMainView = GameViewTitle;
                freeRoamGame->shouldDebounce = true;
                break;
            case InputKeyUp:
                if (currentMenuIndex > MenuIndexProfile)
                {
                    currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex - 1);
                }
                freeRoamGame->shouldDebounce = true;
                break;
            case InputKeyDown:
                if (currentMenuIndex < MenuIndexAbout)
                {
                    currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex + 1);
                }
                freeRoamGame->shouldDebounce = true;
                break;
            case InputKeyOk:
                // Enter the selected menu item
                if (currentMenuIndex == MenuIndexSettings)
                {
                    // Entering settings - this doesn't change the main menu, just shows settings details
                    currentSettingsIndex = MenuSettingsMain;
                }
                freeRoamGame->shouldDebounce = true;
                break;
            default:
                break;
            }
        }
        else // currentMenuIndex == MenuIndexSettings
        {
            switch (currentSettingsIndex)
            {
            case MenuSettingsMain:
                switch (currentInput)
                {
                case InputKeyBack:
                    currentMainView = GameViewTitle;
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyUp:
                    if (currentMenuIndex > MenuIndexProfile)
                    {
                        currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex - 1);
                    }
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyDown:
                    if (currentMenuIndex < MenuIndexAbout)
                    {
                        currentMenuIndex = static_cast<MenuIndex>(currentMenuIndex + 1);
                    }
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyLeft:
                    currentSettingsIndex = MenuSettingsSound;
                    freeRoamGame->shouldDebounce = true;
                    break;
                default:
                    break;
                }
                break;
            case MenuSettingsSound:
                switch (currentInput)
                {
                case InputKeyOk:
                    soundToggle = soundToggle == ToggleOn ? ToggleOff : ToggleOn;
                    // Update the game's sound settings
                    if (freeRoamGame)
                    {
                        freeRoamGame->updateSoundToggle();
                    }
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyRight:
                    currentSettingsIndex = MenuSettingsMain;
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyDown:
                    currentSettingsIndex = MenuSettingsVibration;
                    freeRoamGame->shouldDebounce = true;
                    break;
                default:
                    break;
                }
                break;
            case MenuSettingsVibration:
                switch (currentInput)
                {
                case InputKeyOk:
                    vibrationToggle = vibrationToggle == ToggleOn ? ToggleOff : ToggleOn;
                    // Update the game's vibration settings
                    if (freeRoamGame)
                    {
                        freeRoamGame->updateVibrationToggle();
                    }
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyRight:
                    currentSettingsIndex = MenuSettingsMain;
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyUp:
                    currentSettingsIndex = MenuSettingsSound;
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyDown:
                    currentSettingsIndex = MenuSettingsLeave;
                    freeRoamGame->shouldDebounce = true;
                    break;
                default:
                    break;
                }
                break;
            case MenuSettingsLeave:
                switch (currentInput)
                {
                case InputKeyOk:
                    leaveGame = ToggleOn;
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyRight:
                    currentSettingsIndex = MenuSettingsMain;
                    freeRoamGame->shouldDebounce = true;
                    break;
                case InputKeyUp:
                    currentSettingsIndex = MenuSettingsVibration;
                    freeRoamGame->shouldDebounce = true;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        break;

    case GameViewLogin:
        switch (currentInput)
        {
        case InputKeyBack:
            currentMainView = GameViewWelcome;
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyOk:
            if (loginStatus == LoginSuccess)
            {
                currentMainView = GameViewTitle;
                freeRoamGame->shouldDebounce = true;
            }
            break;
        default:
            break;
        }
        break;

    case GameViewRegistration:
        switch (currentInput)
        {
        case InputKeyBack:
            currentMainView = GameViewWelcome;
            freeRoamGame->shouldDebounce = true;
            break;
        case InputKeyOk:
            if (registrationStatus == RegistrationSuccess)
            {
                currentMainView = GameViewTitle;
                freeRoamGame->shouldDebounce = true;
            }
            break;
        default:
            break;
        }
        break;

    case GameViewUserInfo:
        switch (currentInput)
        {
        case InputKeyBack:
            currentMainView = GameViewTitle;
            freeRoamGame->shouldDebounce = true;
            break;
        default:
            break;
        }
        break;

    case GameViewGameLocal:
    case GameViewGameOnline:
        // In game views, we need to handle input differently
        // The game engine itself will handle input through its update() method
        // We don't intercept input here to avoid conflicts with the in-game menu system
        // The original handleMenu() and debounceInput() methods in the Player::render()
        // and Player::update() methods will handle the in-game system menu correctly
        break;

    default:
        break;
    }
}

void Player::switchLevels(Game *game)
{
    if (currentDynamicMap == nullptr || strcmp(currentDynamicMap->getName(), game->current_level->name) != 0)
    {
        currentDynamicMap.reset(); // reset so we can delete the old map if it exists
        Vector posi = start_position;

        if (strcmp(game->current_level->name, "Tutorial") == 0)
        {
            currentDynamicMap = mapsTutorial();
        }
        else if (strcmp(game->current_level->name, "First") == 0)
        {
            currentDynamicMap = mapsFirst();
        }
        else if (strcmp(game->current_level->name, "Second") == 0)
        {
            currentDynamicMap = mapsSecond();
        }

        if (currentDynamicMap != nullptr)
        {
            // Find a safe spawn position for the new level
            posi = findSafeSpawnPosition(game->current_level->name);

            // Always set position when switching levels to avoid being stuck
            position_set(posi);
            hasBeenPositioned = true;

            // update 3D sprite position immediately after setting player position
            if (has3DSprite())
            {
                update3DSpritePosition();

                // Also ensure the sprite rotation and scale are set correctly
                set3DSpriteRotation(atan2f(direction.y, direction.x) + M_PI_2); // Face forward with orientation correction
                set3DSpriteScale(1.0f);                                         // Normal scale
            }

            justSwitchedLevels = true; // Indicate that we just switched levels
            levelSwitchCounter = 0;    // Reset counter for level switch delay
        }
    }
}

void Player::update(Game *game)
{
    debounceInput(game);

    if (game->input == InputKeyBack)
    {
        gameState = gameState == GameStateMenu ? GameStatePlaying : GameStateMenu;
        shouldDebounce = true;
    }

    if (gameState == GameStateMenu)
    {
        return; // Don't update player position in menu
    }

    const float rotSpeed = 0.2f; // Rotation speed in radians

    switch (game->input)
    {
    case InputKeyUp:
    {
        // Calculate new position
        Vector new_pos = Vector(
            position.x + direction.x * rotSpeed,
            position.y + direction.y * rotSpeed);

        // Check collision with dynamic map
        if (currentDynamicMap == nullptr || !collisionMapCheck(new_pos))
        {
            // Move forward in the direction the player is facing
            this->position_set(new_pos);

            // Update 3D sprite position and rotation to match camera direction
            if (has3DSprite())
            {
                update3DSpritePosition();
                // Make sprite face forward (add π/2 to correct orientation)
                float rotation_angle = atan2f(direction.y, direction.x) + M_PI_2;
                set3DSpriteRotation(rotation_angle);
            }
        }
        game->input = InputKeyMAX;
        justStarted = false;
        justSwitchedLevels = false;
        is_visible = true;
    }
    break;
    case InputKeyDown:
    {
        // Calculate new position
        Vector new_pos = Vector(
            position.x - direction.x * rotSpeed,
            position.y - direction.y * rotSpeed);

        // Check collision with dynamic map
        if (currentDynamicMap == nullptr || !collisionMapCheck(new_pos))
        {
            // Move backward (opposite to the direction)
            this->position_set(new_pos);

            // Update 3D sprite position and rotation to match camera direction
            if (has3DSprite())
            {
                update3DSpritePosition();
                // Make sprite face forward (add π/2 to correct orientation)
                float rotation_angle = atan2f(direction.y, direction.x) + M_PI_2;
                set3DSpriteRotation(rotation_angle);
            }
        }
        game->input = InputKeyMAX;
        justStarted = false;
        justSwitchedLevels = false;
        is_visible = true;
    }
    break;
    case InputKeyLeft:
    {
        float old_dir_x = direction.x;
        float old_plane_x = plane.x;

        direction.x = direction.x * cos(-rotSpeed) - direction.y * sin(-rotSpeed);
        direction.y = old_dir_x * sin(-rotSpeed) + direction.y * cos(-rotSpeed);
        plane.x = plane.x * cos(-rotSpeed) - plane.y * sin(-rotSpeed);
        plane.y = old_plane_x * sin(-rotSpeed) + plane.y * cos(-rotSpeed);

        // Update sprite rotation to match new camera direction
        if (has3DSprite())
        {
            float rotation_angle = atan2f(direction.y, direction.x) + M_PI_2;
            set3DSpriteRotation(rotation_angle);
        }

        game->input = InputKeyMAX;
        justStarted = false;
        justSwitchedLevels = false;
        is_visible = true;
    }
    break;
    case InputKeyRight:
    {
        float old_dir_x = direction.x;
        float old_plane_x = plane.x;

        direction.x = direction.x * cos(rotSpeed) - direction.y * sin(rotSpeed);
        direction.y = old_dir_x * sin(rotSpeed) + direction.y * cos(rotSpeed);
        plane.x = plane.x * cos(rotSpeed) - plane.y * sin(rotSpeed);
        plane.y = old_plane_x * sin(rotSpeed) + plane.y * cos(rotSpeed);

        // Update sprite rotation to match new camera direction
        if (has3DSprite())
        {
            float rotation_angle = atan2f(direction.y, direction.x) + M_PI_2;
            set3DSpriteRotation(rotation_angle);
        }

        game->input = InputKeyMAX;
        justStarted = false;
        justSwitchedLevels = false;
        is_visible = true;
    }
    break;
    default:
        break;
    }

    // if at teleport, then switch levels
    if (currentDynamicMap != nullptr && currentDynamicMap->getTile(position.x, position.y) == TILE_TELEPORT)
    {
        // Switch to the next level or map
        if (game->current_level != nullptr)
        {
            if (strcmp(game->current_level->name, "Tutorial") == 0)
            {
                game->level_switch("First"); // Switch to First level
            }
            else if (strcmp(game->current_level->name, "First") == 0)
            {
                game->level_switch("Second"); // Switch to Second level
            }
            else if (strcmp(game->current_level->name, "Second") == 0)
            {
                game->level_switch("Tutorial"); // Go back to Tutorial or main menu
            }
        }
    }
}

void Player::render(Draw *canvas, Game *game)
{
    if (!canvas || !game || !game->current_level)
    {
        return;
    }

    static uint8_t _state = GameStatePlaying;
    if (justSwitchedLevels && !justStarted)
    {
        // show message after switching levels
        game->draw->fillScreen(ColorWhite);
        game->draw->color(ColorBlack);
        game->draw->icon(Vector(0, 0), &I_icon_menu_128x64px);
        game->draw->setFont(FontPrimary);
        game->draw->text(Vector(5, 15), "New Level");
        game->draw->setFontCustom(FONT_SIZE_SMALL);
        game->draw->text(Vector(5, 30), game->current_level->name);
        game->draw->text(Vector(5, 58), "Tip: BACK opens the menu.");
        is_visible = false; // hide player entity during level switch
        if (levelSwitchCounter < 50)
        {
            levelSwitchCounter++;
        }
        else
        {
            justSwitchedLevels = false;
            levelSwitchCounter = 0; // reset counter
            is_visible = true;      // show player entity again
        }
        return;
    }

    this->switchLevels(game);

    if (gameState == GameStatePlaying)
    {
        if (_state != GameStatePlaying)
        {
            // make entities active again
            for (int i = 0; i < game->current_level->getEntityCount(); i++)
            {
                Entity *entity = game->current_level->getEntity(i);
                if (entity && !entity->is_active && !entity->is_player)
                {
                    entity->is_active = true; // activate all entities
                }
            }
            this->is_visible = true; // show player entity in game
            _state = GameStatePlaying;
        }
        if (currentDynamicMap != nullptr)
        {
            float camera_height = 1.6f;

            // Check if the game is using 3rd person perspective
            if (game->getPerspective() == CAMERA_THIRD_PERSON)
            {
                // Calculate 3rd person camera position for map rendering
                // Normalize direction vector to ensure consistent behavior
                float dir_length = sqrtf(direction.x * direction.x + direction.y * direction.y);
                Vector normalized_dir = Vector(direction.x / dir_length, direction.y / dir_length);

                Vector camera_pos = Vector(
                    position.x - 1.5f, // Fixed offset instead of direction-based
                    position.y - 1.5f);

                if (has3DSprite())
                {
                    // Use Entity's methods instead of direct Sprite3D access
                    update3DSpritePosition();

                    // Make sprite face the same direction as the camera (forward)
                    // Add π/2 offset to correct sprite orientation (was facing left, now faces forward)
                    float camera_direction_angle = atan2f(normalized_dir.y, normalized_dir.x) + M_PI_2;
                    set3DSpriteRotation(camera_direction_angle);
                }

                // Render map from 3rd person camera position
                currentDynamicMap->render(camera_height, canvas, camera_pos, normalized_dir, plane);
            }
            else
            {
                // Default 1st person rendering
                currentDynamicMap->render(camera_height, canvas, position, direction, plane);
            }
        }
    }
    else if (gameState == GameStateMenu)
    {
        if (_state != GameStateMenu)
        {
            // make entities inactive
            for (int i = 0; i < game->current_level->getEntityCount(); i++)
            {
                Entity *entity = game->current_level->getEntity(i);
                if (entity && entity->is_active && !entity->is_player)
                {
                    entity->is_active = false; // deactivate all entities
                }
            }
            this->is_visible = false; // hide player entity in menu
            _state = GameStateMenu;
        }
        handleMenu(canvas, game);
    }
}

void Player::userRequest(RequestType requestType)
{
    if (!freeRoamGame)
    {
        FURI_LOG_E("Player", "userRequest: FreeRoamGame instance is null");
        return;
    }

    // Get app context to access HTTP functionality
    FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
    if (!app)
    {
        FURI_LOG_E("Player", "userRequest: App context is null");
        return;
    }

    // Allocate memory for credentials
    char *username = (char *)malloc(64);
    char *password = (char *)malloc(64);
    if (!username || !password)
    {
        FURI_LOG_E("Player", "userRequest: Failed to allocate memory for credentials");
        if (username)
            free(username);
        if (password)
            free(password);
        return;
    }

    // Load credentials from storage
    bool credentialsLoaded = true;
    if (!app->load_char("user_name", username, 64, "flipper_http"))
    {
        FURI_LOG_E("Player", "Failed to load user_name");
        credentialsLoaded = false;
    }
    if (!app->load_char("user_pass", password, 64, "flipper_http"))
    {
        FURI_LOG_E("Player", "Failed to load user_pass");
        credentialsLoaded = false;
    }

    if (!credentialsLoaded)
    {
        switch (requestType)
        {
        case RequestTypeLogin:
            loginStatus = LoginCredentialsMissing;
            break;
        case RequestTypeRegistration:
            registrationStatus = RegistrationCredentialsMissing;
            break;
        case RequestTypeUserInfo:
            userInfoStatus = UserInfoCredentialsMissing;
            break;
        }
        free(username);
        free(password);
        return;
    }

    // Create JSON payload for login/registration
    char *payload = (char *)malloc(256);
    if (!payload)
    {
        FURI_LOG_E("Player", "userRequest: Failed to allocate memory for payload");
        free(username);
        free(password);
        return;
    }
    snprintf(payload, 256, "{\"username\":\"%s\",\"password\":\"%s\"}", username, password);

    switch (requestType)
    {
    case RequestTypeLogin:
        if (!app->httpRequestAsync("login.txt",
                                   "https://www.jblanked.com/flipper/api/user/login/",
                                   POST, "{\"Content-Type\":\"application/json\"}", payload))
        {
            loginStatus = LoginRequestError;
        }
        break;
    case RequestTypeRegistration:
        if (!app->httpRequestAsync("register.txt",
                                   "https://www.jblanked.com/flipper/api/user/register/",
                                   POST, "{\"Content-Type\":\"application/json\"}", payload))
        {
            registrationStatus = RegistrationRequestError;
        }
        break;
    case RequestTypeUserInfo:
    {
        char *url = (char *)malloc(128);
        if (!url)
        {
            FURI_LOG_E("Player", "userRequest: Failed to allocate memory for url");
            userInfoStatus = UserInfoRequestError;
            free(username);
            free(password);
            free(payload);
            return;
        }
        snprintf(url, 128, "https://www.jblanked.com/flipper/api/user/game-stats/%s/", username);
        if (!app->httpRequestAsync("user_info.txt", url, GET, "{\"Content-Type\":\"application/json\"}"))
        {
            userInfoStatus = UserInfoRequestError;
        }
        free(url);
    }
    break;
    default:
        FURI_LOG_E("Player", "Unknown request type: %d", requestType);
        loginStatus = LoginRequestError;
        registrationStatus = RegistrationRequestError;
        userInfoStatus = UserInfoRequestError;
        free(username);
        free(password);
        free(payload);
        return;
    }

    free(username);
    free(password);
    free(payload);
}

bool Player::httpRequestIsFinished()
{
    if (!freeRoamGame)
    {
        return true; // Default to finished if no game context
    }

    // Get app context to check HTTP state
    FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
    if (!app)
    {
        return true; // Default to finished if no app context
    }

    // Check if HTTP request is finished (state is IDLE)
    return app->getHttpState() == IDLE;
}

HTTPState Player::getHttpState()
{
    if (!freeRoamGame)
    {
        return INACTIVE;
    }

    // Get app context to check HTTP state
    FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
    if (!app)
    {
        return INACTIVE;
    }

    return app->getHttpState();
}

bool Player::setHttpState(HTTPState state)
{
    if (!freeRoamGame)
    {
        return false;
    }

    // Get app context to set HTTP state
    FreeRoamApp *app = static_cast<FreeRoamApp *>(freeRoamGame->appContext);
    if (!app)
    {
        return false;
    }

    return app->setHttpState(state);
}
