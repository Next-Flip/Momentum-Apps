#pragma once
#include "mousejacker_ducky.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEYBOARD_LAYOUT_QTY 13

typedef struct {
    const char* name;            // Name of the keyboard layout (e.g., "US QWERTY")
    MJDuckyKey* keys;            // Pointer to an array of MJDuckyKey values
    size_t num_keys;             // Number of keys in the array (length of the array)
} KeyboardLayout;

extern KeyboardLayout keyboard_layouts[];
extern int8_t keyboard_layouts_idx;

#ifdef __cplusplus
}
#endif