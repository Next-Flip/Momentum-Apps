#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

typedef enum {
    SOURCE_INTERNAL,
    SOURCE_EXTERNAL
} IrSource;

typedef struct {
    IrSource current_source;
    bool jamming_active;
    uint16_t mark_timing;
    uint8_t setting_index;
} JammerState;

const char* IR_SOURCE_NAMES[] = {"INTERNAL", "EXTERNAL"};

static void render_callback(Canvas* canvas, void* ctx) {
    JammerState* state = ctx;
    canvas_clear(canvas);
    
    canvas_set_font(canvas, FontSecondary);
    int title_width = canvas_string_width(canvas, "IR Jammer");
    int x_center = (128 - title_width) / 2;
    canvas_draw_str(canvas, x_center, 10, "IR Jammer");
    
    char buffer[32];
    
    canvas_set_color(canvas, (state->setting_index == 0) ? ColorBlack : ColorWhite);
    canvas_draw_box(canvas, 2, 14, 124, 9);
    canvas_set_color(canvas, (state->setting_index == 0) ? ColorWhite : ColorBlack);
    snprintf(buffer, sizeof(buffer), "STATUS: %s", state->jamming_active ? "ACTIVE" : "PAUSED");
    canvas_draw_str(canvas, 4, 22, buffer);
    
    canvas_set_color(canvas, (state->setting_index == 1) ? ColorBlack : ColorWhite);
    canvas_draw_box(canvas, 2, 24, 124, 9);
    canvas_set_color(canvas, (state->setting_index == 1) ? ColorWhite : ColorBlack);
    snprintf(buffer, sizeof(buffer), "TARGET IR: %s", IR_SOURCE_NAMES[state->current_source]);
    canvas_draw_str(canvas, 4, 32, buffer);
    
    canvas_set_color(canvas, (state->setting_index == 2) ? ColorBlack : ColorWhite);
    canvas_draw_box(canvas, 2, 34, 124, 9);
    canvas_set_color(canvas, (state->setting_index == 2) ? ColorWhite : ColorBlack);
    snprintf(buffer, sizeof(buffer), "TIMING: %d ms", state->mark_timing);
    canvas_draw_str(canvas, 4, 42, buffer);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t ir_jammer_app(void* p) {
    UNUSED(p);

    JammerState* state = malloc(sizeof(JammerState));
    state->current_source = SOURCE_EXTERNAL;
    state->jamming_active = false;
    state->mark_timing = 10;
    state->setting_index = 0;

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, state);
    view_port_input_callback_set(view_port, input_callback, event_queue);
    
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;
    bool running = true;

    const uint16_t sweep_freqs[] = {30000, 33000, 36000, 38000, 40000, 42000, 56000};
    const uint8_t total_freqs = sizeof(sweep_freqs) / sizeof(sweep_freqs);
    uint8_t sweep_idx = 3;

    const GpioPin* pin_ext = &gpio_ext_pa7;
    const GpioPin* pin_int = &gpio_infrared_tx;
    
    furi_hal_gpio_init(pin_ext, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_int, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    while(running) {
        if(furi_message_queue_get(event_queue, &event, 1) == FuriStatusOk) {
            if(event.type == InputTypeShort || event.type == InputTypeRepeat) {
                if(event.key == InputKeyBack) {
                    running = false;
                }
                else if(event.key == InputKeyUp) {
                    if(state->setting_index > 0) state->setting_index--;
                    else state->setting_index = 2;
                }
                else if(event.key == InputKeyDown) {
                    state->setting_index = (state->setting_index + 1) % 3;
                }
                else if(event.key == InputKeyRight || event.key == InputKeyLeft) {
                    int dir = (event.key == InputKeyRight) ? 1 : -1;
                    
                    if(state->setting_index == 0) {
                        state->jamming_active = !state->jamming_active;
                        if(state->jamming_active) {
                            if(state->current_source == SOURCE_EXTERNAL) {
                                furi_hal_power_enable_otg();
                            }
                        } else {
                            furi_hal_gpio_write(pin_ext, false);
                            furi_hal_gpio_write(pin_int, false);
                            furi_hal_power_disable_otg();
                        }
                    }
                    else if(state->setting_index == 1) {
                        if(state->jamming_active) {
                            furi_hal_gpio_write(pin_ext, false);
                            furi_hal_gpio_write(pin_int, false);
                            furi_hal_power_disable_otg();
                            state->jamming_active = false;
                        }
                        state->current_source = (IrSource)((state->current_source + dir + 2) % 2);
                    }
                    else if(state->setting_index == 2) {
                        int32_t val = state->mark_timing + dir;
                        if(val < 1) val = 1;
                        if(val > 100) val = 100;
                        state->mark_timing = (uint16_t)val;
                    }
                }
            }
        }
        
        if(state->jamming_active) {
            const GpioPin* active_pin = (state->current_source == SOURCE_EXTERNAL) ? pin_ext : pin_int;
            
            uint32_t current_freq = sweep_freqs[sweep_idx];
            uint32_t half_period_us = 500000 / current_freq;
            uint32_t total_period_us = half_period_us * 2;
            uint32_t cycles = (state->mark_timing * 1000) / total_period_us; 
            
            for(uint32_t i = 0; i < cycles; i++) {
                furi_hal_gpio_write(active_pin, true);
                furi_delay_us(half_period_us);
                furi_hal_gpio_write(active_pin, false);
                furi_delay_us(half_period_us);
            }
            
            furi_hal_gpio_write(active_pin, false);
            furi_delay_ms(5);
            
            sweep_idx = (sweep_idx + 1) % total_freqs;
        } else {
            furi_delay_ms(10);
        }

        view_port_update(view_port);
    }

    furi_hal_gpio_write(pin_ext, false);
    furi_hal_gpio_write(pin_int, false);
    furi_hal_power_disable_otg();

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_GUI);
    free(state);

    return 0;
}
