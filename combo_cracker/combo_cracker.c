#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <input/input.h>
#include "combo_cracker_icons.h"

#define TAG "ComboLockCracker"

#define BACKLIGHT_ON 1

#define MAX_VALUES 10

// thank you derek jamison! ;)
typedef enum {
    ComboViewSubmenu,
    ComboViewCracker,
    ComboViewResults,
    ComboViewTutorial,
    ComboViewAbout,
} ComboView;

typedef enum {
    ComboEventIdRedrawScreen = 0,
    ComboEventIdCalculateCombo = 1,
} ComboEventId;

typedef enum {
    ComboSubmenuIndexCracker,
    ComboSubmenuIndexTutorial,
    ComboSubmenuIndexAbout,
} ComboSubmenuIndex;

typedef struct {
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
    Submenu* submenu;
    View* view_cracker;
    Widget* widget_results;
    Widget* widget_tutorial;
    Widget* widget_about;

    FuriTimer* timer;
} ComboLockCrackerApp;

typedef struct {
    int first_lock;
    int second_lock;
    float resistance;
    int selected;
    char result[256];
} ComboLockCrackerModel;

/**
 * @brief      helper to convert float to string
 * @param      num   the float number to convert
 * @return     string representation of the float
 */
static char* float_to_char(float num) {
    static char buffer[8];
    snprintf(buffer, sizeof(buffer), "%.1f", (double)num);
    return buffer;
}

/**
 * @brief      calculate the combination based on inputs
 * @param      model   the model containing input values
 */
static void calculate_combo(ComboLockCrackerModel* model) {
    float sticky_number = model->resistance;
    int sticky_as_int = (int)sticky_number;

    int first_digit;
    if((sticky_number - sticky_as_int) == 0.0f) {
        // first digit easy, resistance + 5... crazy world we live in
        first_digit = sticky_as_int + 5;
    } else {
        first_digit = (int)(sticky_number + 5) + 1; // ceiling that biih... prob. a better way
    }

    first_digit = first_digit % 40;
    int remainder = first_digit % 4;

    int a = model->first_lock;
    int b = model->second_lock;

    // third digit isn't too bad
    int third_position_values[MAX_VALUES];
    int third_count = 0;

    for(int i = 0; i < 3; i++) {
        if(a % 4 == remainder) third_position_values[third_count++] = a;
        if(b % 4 == remainder) third_position_values[third_count++] = b;
        a = (a + 10) % 40;
        b = (b + 10) % 40;
    }

    int row_1 = (remainder + 2) % 40;
    int row_2 = (row_1 + 4) % 40;

    int second_position_values[MAX_VALUES];
    int second_count = 0;
    second_position_values[second_count++] = row_1;
    second_position_values[second_count++] = row_2;

    for(int i = 0; i < 4; i++) {
        row_1 = (row_1 + 8) % 40;
        row_2 = (row_2 + 8) % 40;
        second_position_values[second_count++] = row_1;
        second_position_values[second_count++] = row_2;
    }

    for(int i = 0; i < second_count - 1; i++) {
        for(int j = i + 1; j < second_count; j++) {
            if(second_position_values[i] > second_position_values[j]) {
                int temp = second_position_values[i];
                second_position_values[i] = second_position_values[j];
                second_position_values[j] = temp;
            }
        }
    }

    int pos = 0;
    int written = snprintf(
        model->result + pos,
        sizeof(model->result) - pos,
        "First Pin: %d\nSecond Pin(s): ",
        first_digit);
    if(written < 0 || written >= (int)(sizeof(model->result) - pos)) return;
    pos += written;

    for(int i = 0; i < second_count; i++) {
        written = snprintf(
            model->result + pos, sizeof(model->result) - pos, "%d", second_position_values[i]);
        if(written < 0 || written >= (int)(sizeof(model->result) - pos)) return;
        pos += written;

        if(i < second_count - 1) {
            const char* sep = (i == 3) ? ",\n -> " : ", ";
            written = snprintf(model->result + pos, sizeof(model->result) - pos, "%s", sep);
            if(written < 0 || written >= (int)(sizeof(model->result) - pos)) return;
            pos += written;
        }
    }

    written = snprintf(model->result + pos, sizeof(model->result) - pos, "\nThird Pin(s): ");
    if(written < 0 || written >= (int)(sizeof(model->result) - pos)) return;
    pos += written;

    for(int i = 0; i < third_count; i++) {
        written = snprintf(
            model->result + pos, sizeof(model->result) - pos, "%d", third_position_values[i]);
        if(written < 0 || written >= (int)(sizeof(model->result) - pos)) return;
        pos += written;

        if(i < third_count - 1) {
            written = snprintf(model->result + pos, sizeof(model->result) - pos, ", ");
            if(written < 0 || written >= (int)(sizeof(model->result) - pos)) return;
            pos += written;
        }
    }
}

/**
 * @brief      callback for exiting the application.
 * @details    this function is called when user press back button.
 * @param      _context  the context - unused
 * @return     next view id
 */
static uint32_t combo_navigation_exit_callback(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}

/**
 * @brief      callback for returning to submenu.
 * @details    this function is called when user press back button.
 * @param      _context  the context - unused
 * @return     next view id
 */
static uint32_t combo_navigation_submenu_callback(void* _context) {
    UNUSED(_context);
    return ComboViewSubmenu;
}

/**
 * @brief      handle submenu item selection.
 * @details    this function is called when user selects an item from the submenu.
 * @param      context  the context - ComboLockCrackerApp object.
 * @param      index    the ComboSubmenuIndex item that was clicked.
 */
static void combo_submenu_callback(void* context, uint32_t index) {
    ComboLockCrackerApp* app = (ComboLockCrackerApp*)context;
    switch(index) {
    case ComboSubmenuIndexCracker:
        view_dispatcher_switch_to_view(app->view_dispatcher, ComboViewCracker);
        break;
    case ComboSubmenuIndexTutorial:
        view_dispatcher_switch_to_view(app->view_dispatcher, ComboViewTutorial);
        break;
    case ComboSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, ComboViewAbout);
        break;
    default:
        break;
    }
}

/**
 * @brief      callback for drawing the cracker screen.
 * @details    this function is called when the screen needs to be redrawn.
 * @param      canvas  the canvas to draw on.
 * @param      model   the model - ComboLockCrackerModel object.
 */
static void combo_view_cracker_draw_callback(Canvas* canvas, void* model) {
    ComboLockCrackerModel* my_model = (ComboLockCrackerModel*)model;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);

    char buf[16];
    int icon_width = 32;
    int icon_x = 128 - icon_width - 2; // moved 3 pixels to the right
    int icon_y = 2;
    int text_x = 2;
    int value_x = 75;
    int indicator_offset = -5;

    canvas_draw_str(canvas, text_x, 12, "First Lock:");
    snprintf(buf, sizeof(buf), "%d", my_model->first_lock);
    canvas_draw_str(
        canvas,
        value_x + (my_model->selected == 0 ? indicator_offset : 0),
        12,
        (my_model->selected == 0 ? ">" : ""));
    canvas_draw_str(canvas, value_x, 12, buf);

    canvas_draw_str(canvas, text_x, 24, "Second Lock:");
    snprintf(buf, sizeof(buf), "%d", my_model->second_lock);
    canvas_draw_str(
        canvas,
        value_x + (my_model->selected == 1 ? indicator_offset : 0),
        24,
        (my_model->selected == 1 ? ">" : ""));
    canvas_draw_str(canvas, value_x, 24, buf);

    canvas_draw_str(canvas, text_x, 36, "Resistance:");
    snprintf(buf, sizeof(buf), "%s", float_to_char(my_model->resistance));
    canvas_draw_str(
        canvas,
        value_x + (my_model->selected == 2 ? indicator_offset : 0),
        36,
        (my_model->selected == 2 ? ">" : ""));
    canvas_draw_str(canvas, value_x, 36, buf);

    canvas_draw_str(canvas, text_x, 62, "OK to calculate");
    canvas_draw_icon(canvas, icon_x, icon_y, &I_lock32x32);
}

/**
 * @brief      callback for cracker screen input.
 * @details    this function is called when the user presses or holds a button while on the cracker screen.
 * @param      event    the event - InputEvent object.
 * @param      context  the context - ComboLockCrackerApp object.
 * @return     true if the event was handled, false otherwise.
 */
static bool combo_view_cracker_input_callback(InputEvent* event, void* context) {
    ComboLockCrackerApp* app = (ComboLockCrackerApp*)context;

    bool redraw = true;
    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            with_view_model(
                app->view_cracker,
                ComboLockCrackerModel * model,
                { model->selected = (model->selected > 0) ? model->selected - 1 : 2; },
                redraw);
            break;
        case InputKeyDown:
            with_view_model(
                app->view_cracker,
                ComboLockCrackerModel * model,
                { model->selected = (model->selected < 2) ? model->selected + 1 : 0; },
                redraw);
            break;
        case InputKeyLeft:
            with_view_model(
                app->view_cracker,
                ComboLockCrackerModel * model,
                {
                    if(model->selected == 0 && model->first_lock > 0) model->first_lock--;
                    if(model->selected == 1 && model->second_lock > 0) model->second_lock--;
                    if(model->selected == 2 && model->resistance > 0) model->resistance -= 0.5f;
                },
                redraw);
            break;
        case InputKeyRight:
            with_view_model(
                app->view_cracker,
                ComboLockCrackerModel * model,
                {
                    if(model->selected == 0 && model->first_lock < 39) model->first_lock++;
                    if(model->selected == 1 && model->second_lock < 39) model->second_lock++;
                    if(model->selected == 2 && model->resistance < 39.5f)
                        model->resistance += 0.5f;
                },
                redraw);
            break;
        case InputKeyOk:
            view_dispatcher_send_custom_event(app->view_dispatcher, ComboEventIdCalculateCombo);
            return true;
        default:
            break;
        }
    } else if (event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyLeft:
            with_view_model(
                app->view_cracker,
                ComboLockCrackerModel * model,
                {
                    if(model->selected == 0 && model->first_lock > 0) model->first_lock--;
                    if(model->selected == 1 && model->second_lock > 0) model->second_lock--;
                    if(model->selected == 2 && model->resistance > 0) model->resistance -= 0.5f;
                },
                redraw);
            break;
        case InputKeyRight:
            with_view_model(
                app->view_cracker,
                ComboLockCrackerModel * model,
                {
                    if(model->selected == 0 && model->first_lock < 39) model->first_lock++;
                    if(model->selected == 1 && model->second_lock < 39) model->second_lock++;
                    if(model->selected == 2 && model->resistance < 39.5f)
                        model->resistance += 0.5f;
                },
                redraw);
            break;
        default:
            break;
        }
    }

    return false;
}

/**
 * @brief      callback for custom events.
 * @details    this function is called when a custom event is sent to the view dispatcher.
 * @param      event    the event id - ComboEventId value.
 * @param      context  the context - ComboLockCrackerApp object.
 */
static bool combo_view_cracker_custom_event_callback(uint32_t event, void* context) {
    ComboLockCrackerApp* app = (ComboLockCrackerApp*)context;

    switch(event) {
    case ComboEventIdRedrawScreen: {
        bool redraw = true;
        with_view_model(
            app->view_cracker, ComboLockCrackerModel * _model, { UNUSED(_model); }, redraw);
        return true;
    }
    case ComboEventIdCalculateCombo: {
        bool redraw = false;
        with_view_model(
            app->view_cracker,
            ComboLockCrackerModel * model,
            {
                calculate_combo(model);
                widget_reset(app->widget_results);
                widget_add_text_scroll_element(app->widget_results, 0, 0, 128, 64, model->result);
            },
            redraw);

        view_dispatcher_switch_to_view(app->view_dispatcher, ComboViewResults);
        return true;
    }
    default:
        return false;
    }
}

static ComboLockCrackerApp* combo_app_alloc() {
    ComboLockCrackerApp* app = (ComboLockCrackerApp*)malloc(sizeof(ComboLockCrackerApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Crack Lock", ComboSubmenuIndexCracker, combo_submenu_callback, app);
    submenu_add_item(app->submenu, "Tutorial", ComboSubmenuIndexTutorial, combo_submenu_callback, app);
    submenu_add_item(app->submenu, "About", ComboSubmenuIndexAbout, combo_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), combo_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, ComboViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, ComboViewSubmenu);

    app->view_cracker = view_alloc();
    view_set_draw_callback(app->view_cracker, combo_view_cracker_draw_callback);
    view_set_input_callback(app->view_cracker, combo_view_cracker_input_callback);
    view_set_previous_callback(app->view_cracker, combo_navigation_submenu_callback);
    view_set_context(app->view_cracker, app);
    view_set_custom_callback(app->view_cracker, combo_view_cracker_custom_event_callback);
    view_allocate_model(app->view_cracker, ViewModelTypeLockFree, sizeof(ComboLockCrackerModel));

    ComboLockCrackerModel* model = view_get_model(app->view_cracker);
    model->first_lock = 0;
    model->second_lock = 0;
    model->resistance = 0.0f;
    model->selected = 0;
    model->result[0] = '\0';

    view_dispatcher_add_view(app->view_dispatcher, ComboViewCracker, app->view_cracker);

    app->widget_results = widget_alloc();
    view_set_previous_callback(
        widget_get_view(app->widget_results), combo_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, ComboViewResults, widget_get_view(app->widget_results));

    app->widget_tutorial = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_tutorial,
        0,
        0,
        128,
        64,
        "How to use:\n"
        "---\n"
        "First lock value:\n"
        "Set the lock's position to 0, then pull up firmly on the shackle and slowly rotate the dial counter-clockwise until it catches between two numbers. If the two locations where the lock gets caught between are whole numbers, reduce the tension on the shackle slightly so you can move out of the groove and find the next one. Once you find a groove between two half numbers, enter the number in the middle of that groove as the first lock position.\n\n"
        "Second lock value:\n"
        "Use the same process to find the next groove(s) after the first position. Remember that the locations where the lock gets caught between should be half numbers, so the middle location will be a whole number.\n\n"
        "Resistance value:\n"
        "Now apply about half as much tension on the shackle and rotate the dial clockwise until you feel resistance. You can repeat this step several times to make sure you have the correct position. Then enter this value as the resistance position. This can be a whole or half number.\n\n"
        "This is a brief summary of the technique developed by Samy Kamkar. For full details and his original write-up, see:\n"
        "https://samy.pl/master/master.html\n");
    view_set_previous_callback(
        widget_get_view(app->widget_tutorial), combo_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, ComboViewTutorial, widget_get_view(app->widget_tutorial));

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "Combo Lock Cracker\n"
        "---\n"
        "Based on Samy Kamkar's Master Lock research.\n"
        "Crack Combo Locks in 8 tries\n"
        "https://samy.pl/master/\n"
        "README at https://github.com/CharlesTheGreat77/ComboCracker-FZ\n");
    view_set_previous_callback(
        widget_get_view(app->widget_about), combo_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, ComboViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    notification_message(app->notifications, &sequence_display_backlight_enforce_on);

    return app;
}

/**
 * @brief      free the combo application.
 * @details    this function frees the application resources.
 * @param      app  the application object.
 */
static void combo_app_free(ComboLockCrackerApp* app) {
#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, ComboViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, ComboViewTutorial);
    widget_free(app->widget_tutorial);
    view_dispatcher_remove_view(app->view_dispatcher, ComboViewResults);
    widget_free(app->widget_results);
    view_dispatcher_remove_view(app->view_dispatcher, ComboViewCracker);
    view_free(app->view_cracker);
    view_dispatcher_remove_view(app->view_dispatcher, ComboViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t combo_cracker_app(void* _p) {
    UNUSED(_p);

    ComboLockCrackerApp* app = combo_app_alloc();
    view_dispatcher_run(app->view_dispatcher);

    combo_app_free(app);
    return 0;
}
