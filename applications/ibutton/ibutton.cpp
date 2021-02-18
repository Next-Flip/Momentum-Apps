#include "ibutton.h"
#include "ibutton_mode_dallas_read.h"
#include "ibutton_mode_dallas_emulate.h"
#include "ibutton_mode_dallas_write.h"
#include "ibutton_mode_cyfral_read.h"
#include "ibutton_mode_cyfral_emulate.h"

// start app
void AppiButton::run() {
    mode[0] = new AppiButtonModeDallasRead(this);
    mode[1] = new AppiButtonModeDallasEmulate(this);
    mode[2] = new AppiButtonModeDallasWrite(this);
    mode[3] = new AppiButtonModeCyfralRead(this);
    mode[4] = new AppiButtonModeCyfralEmulate(this);

    switch_to_mode(0);

    api_hal_power_insomnia_enter();
    app_ready();

    AppiButtonEvent event;
    while(1) {
        if(get_event(&event, 1024 / 8)) {
            if(event.type == AppiButtonEvent::EventTypeKey) {
                // press events
                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyBack) {
                    view_port_enabled_set(view_port, false);
                    gui_remove_view_port(gui, view_port);
                    api_hal_power_insomnia_exit();

                    return;
                }

                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyLeft) {
                    decrease_mode();
                }

                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyRight) {
                    increase_mode();
                }
            }
        } else {
            event.type = AppiButtonEvent::EventTypeTick;
        }

        acquire_state();
        mode[state.mode_index]->event(&event, &state);
        release_state();

        view_port_update(view_port);
    };
}

// render app
void AppiButton::render(Canvas* canvas) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 12, "iButton");

    mode[state.mode_index]->render(canvas, &state);
}

void AppiButton::render_dallas_list(Canvas* canvas, AppiButtonState* state) {
    const uint8_t buffer_size = 50;
    char buf[buffer_size];
    for(uint8_t i = 0; i < state->dallas_address_count; i++) {
        snprintf(
            buf,
            buffer_size,
            "%s[%u] %x:%x:%x:%x:%x:%x:%x:%x",
            (i == state->dallas_address_index) ? "> " : "",
            i + 1,
            state->dallas_address[i][0],
            state->dallas_address[i][1],
            state->dallas_address[i][2],
            state->dallas_address[i][3],
            state->dallas_address[i][4],
            state->dallas_address[i][5],
            state->dallas_address[i][6],
            state->dallas_address[i][7]);
        canvas_draw_str(canvas, 2, 37 + i * 12, buf);
    }
}

void AppiButton::render_cyfral_list(Canvas* canvas, AppiButtonState* state) {
    const uint8_t buffer_size = 50;
    char buf[buffer_size];
    for(uint8_t i = 0; i < state->cyfral_address_count; i++) {
        snprintf(
            buf,
            buffer_size,
            "%s[%u] %x:%x:%x:%x",
            (i == state->cyfral_address_index) ? "> " : "",
            i + 1,
            state->cyfral_address[i][0],
            state->cyfral_address[i][1],
            state->cyfral_address[i][2],
            state->cyfral_address[i][3]);
        canvas_draw_str(canvas, 2, 37 + i * 12, buf);
    }
}

void AppiButton::blink_red() {
    api_hal_light_set(LightRed, 0xFF);
    delay(10);
    api_hal_light_set(LightRed, 0x00);
}

void AppiButton::blink_green() {
    api_hal_light_set(LightGreen, 0xFF);
    delay(10);
    api_hal_light_set(LightGreen, 0x00);
}

void AppiButton::increase_mode() {
    acquire_state();
    if(state.mode_index < (modes_count - 1)) {
        mode[state.mode_index]->release();
        state.mode_index++;
        mode[state.mode_index]->acquire();
    }
    release_state();
}

void AppiButton::decrease_mode() {
    acquire_state();
    if(state.mode_index > 0) {
        mode[state.mode_index]->release();
        state.mode_index--;
        mode[state.mode_index]->acquire();
    }
    release_state();
}

void AppiButton::increase_dallas_address() {
    if(state.dallas_address_index < (state.dallas_address_count - 1)) {
        state.dallas_address_index++;
    }
}

void AppiButton::decrease_dallas_address() {
    if(state.dallas_address_index > 0) {
        state.dallas_address_index--;
    }
}

void AppiButton::increase_cyfral_address() {
    if(state.cyfral_address_index < (state.cyfral_address_count - 1)) {
        state.cyfral_address_index++;
    }
}

void AppiButton::decrease_cyfral_address() {
    if(state.cyfral_address_index > 0) {
        state.cyfral_address_index--;
    }
}

void AppiButton::switch_to_mode(uint8_t mode_index) {
    mode[state.mode_index]->release();
    state.mode_index = mode_index;
    mode[state.mode_index]->acquire();
}

// app enter function
extern "C" int32_t app_ibutton(void* p) {
    AppiButton* app = new AppiButton();
    app->run();
    return 0;
}