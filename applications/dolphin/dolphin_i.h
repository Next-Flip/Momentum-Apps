#pragma once

#include "dolphin.h"
#include "dolphin_state.h"
#include "dolphin_views.h"

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/canvas.h>
#include <menu/menu.h>

#include <assets_icons.h>
#include <stdint.h>

typedef enum {
    DolphinEventTypeDeed,
    DolphinEventTypeSave,
} DolphinEventType;

typedef struct {
    DolphinEventType type;
    union {
        DolphinDeed deed;
    };
} DolphinEvent;

struct Dolphin {
    // Internal message queue
    osMessageQueueId_t event_queue;
    // State
    DolphinState* state;
    // Menu
    ValueMutex* menu_vm;
    // GUI
    ViewDispatcher* idle_view_dispatcher;
    View* idle_view_first_start;
    View* idle_view_main;
    View* idle_view_up;
    View* idle_view_down;
    View* view_hw_mismatch;
};

Dolphin* dolphin_alloc();

/* Save Dolphin state (write to permanent memory)
 * Thread safe
 */
void dolphin_save(Dolphin* dolphin);
