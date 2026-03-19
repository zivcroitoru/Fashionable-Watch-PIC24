#include "h/menu_editor.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define COLOR_TEXT 0xFFFF
#define COLOR_BG   0x0000

// Small area used for the temporary "SET" message
#define SET_X1  2
#define SET_Y1  84
#define SET_X2  28
#define SET_Y2  95
#define SET_TX  2
#define SET_TY  85

// Draw a simple rectangle using 4 lines
static void draw_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
    oledC_DrawLine(x1, y1, x2, y1, 1, color);
    oledC_DrawLine(x1, y2, x2, y2, 1, color);
    oledC_DrawLine(x1, y1, x1, y2, 1, color);
    oledC_DrawLine(x2, y1, x2, y2, 1, color);
}

// Wrap a value inside a min..max range
// Example: if max is 23 and value becomes 24 -> wrap back to 0
static uint8_t wrap_range(int16_t value, uint8_t min_val, uint8_t max_val)
{
    int16_t range = (int16_t)(max_val - min_val + 1);

    while (value < min_val) value += range;
    while (value > max_val) value -= range;

    return (uint8_t)value;
}

// Draw only the inside value of one editable field
// This clears the inside of the field and redraws the text
static void draw_field_value(MenuEditField* field)
{
    char buf[16];

    oledC_DrawRectangle(field->x1 + 2, field->y1 + 2,
                        field->x2 - 2, field->y2 - 2, COLOR_BG);

    if (field->type == FIELD_TYPE_TOGGLE) {
        sprintf(buf, "%s", field->edit_value ? "ON" : "OFF");
    } else {
        sprintf(buf, "%02u", field->edit_value);
    }

    oledC_DrawString(field->text_x, field->text_y,
                     field->text_scale_x, field->text_scale_y,
                     (uint8_t*)buf, COLOR_TEXT);
}

// Initialize editor runtime state and copy current real values into edit values
void menu_editor_init(MenuEditorState* state)
{
    uint8_t i;

    if (!state) return;

    state->initialized = 1;
    state->last_cursor = -1;
    state->pot_start_val = 0;
    state->value_start_val = 0;
    state->msg_timer = 0;
    state->confirm_msg = "";
    state->first_draw = true;
    state->last_drawn_box = -1;
    state->values_dirty = true;   // editor values need to be drawn at least once

    for (i = 0; i < state->field_count; i++) {
        state->fields[i].edit_value = *(state->fields[i].value_ptr);
    }
}

// Reset editor state so the next entry to the page starts clean
void menu_editor_reset(MenuEditorState* state)
{
    if (!state) return;

    state->initialized = 0;
    state->last_cursor = -1;
    state->pot_start_val = 0;
    state->value_start_val = 0;
    state->msg_timer = 0;
    state->confirm_msg = "";
    state->first_draw = true;
    state->last_drawn_box = -1;
    state->values_dirty = true;
}

// Update the currently selected field from the potentiometer
// Important:
// - this now marks only the editor as dirty
// - it should not force the whole screen / top bar to redraw
bool menu_editor_update_from_pot(MenuEditorState* state, uint16_t pot_value, int8_t cursor)
{
    MenuEditField* field;
    int16_t offset;
    int16_t new_val;

    if (!state) return false;

    if (!state->initialized) {
        menu_editor_init(state);
    }

    if (cursor < 0 || cursor >= state->field_count) {
        return false;
    }

    field = &state->fields[cursor];

    // If user moved to a different field:
    // save a new pot baseline so editing starts smoothly from current value
    if (cursor != state->last_cursor) {
        state->last_cursor = cursor;
        state->pot_start_val = pot_value;
        state->value_start_val = field->edit_value;

        // Remove old SET message when switching fields
        state->msg_timer = 0;
        state->confirm_msg = "";

        // Only editor contents need refresh
        state->values_dirty = true;
        return true;
    }

    // Toggle fields are changed only by button press, not by pot movement
    if (field->type == FIELD_TYPE_TOGGLE) {
        return false;
    }

    // Convert pot delta into a field value delta
    offset = ((int16_t)pot_value - (int16_t)state->pot_start_val) / field->step_div;
    new_val = state->value_start_val + offset;
    new_val = wrap_range(new_val, field->min_value, field->max_value);

    // Update only if value actually changed
    if ((uint8_t)new_val != field->edit_value) {
        field->edit_value = (uint8_t)new_val;
        state->values_dirty = true;
        return true;
    }

    return false;
}

// Handle select/confirm button press on current cursor position
void menu_editor_on_select(MenuEditorState* state, int8_t cursor, bool* request_back)
{
    MenuEditField* field;
    bool changed = false;

    if (!state || !request_back) return;

    if (!state->initialized) {
        menu_editor_init(state);
    }

    *request_back = false;

    // Cursor is on BACK button
    if (cursor == state->field_count) {
        *request_back = true;
        return;
    }

    if (cursor < 0 || cursor >= state->field_count) {
        return;
    }

    field = &state->fields[cursor];

    // Toggle field flips immediately on select
    if (field->type == FIELD_TYPE_TOGGLE) {
        field->edit_value = field->edit_value ? 0 : 1;
        changed = true;
    } else {
        // For numeric fields, show SET only if the edited value is different
        if (*(field->value_ptr) != field->edit_value) {
            changed = true;
        }
    }

    if (changed) {
        // Commit edited value to the real variable
        *(field->value_ptr) = field->edit_value;

        // Show temporary confirmation text
        state->confirm_msg = "SET";
        state->msg_timer = 150;

        // Only editor area needs refresh
        state->values_dirty = true;
    }
}

// Draw the editor UI
// force_redraw = full editor redraw request from outside
// values_dirty  = local editor-only redraw request from pot/value change
void menu_editor_draw(MenuEditorState* state, int8_t cursor, volatile bool* force_redraw)
{
    uint8_t i;

    if (!state || !force_redraw) return;

    if (!state->initialized) {
        menu_editor_init(state);
    }

    // First time on this page: draw the editor area once
    if (state->first_draw) {
        oledC_DrawRectangle(0, 24, 95, 95, COLOR_BG);

        if (state->title && state->title[0] != '\0') {
            oledC_DrawString(18, 25, 1, 1, (uint8_t*)state->title, COLOR_TEXT);
        }

        oledC_DrawString(state->back_text_x, state->back_text_y,
                         1, 1, (uint8_t*)state->back_label, COLOR_TEXT);

        for (i = 0; i < state->field_count; i++) {
            draw_field_value(&state->fields[i]);
        }

        state->first_draw = false;
        state->values_dirty = false;
    }

    // Redraw editor values only when needed
    // This can happen from:
    // 1. external full redraw
    // 2. local editor value change from pot/select
    if (*force_redraw || state->values_dirty) {
        for (i = 0; i < state->field_count; i++) {
            draw_field_value(&state->fields[i]);
        }
        state->values_dirty = false;
    }

    // Keep SET visible while timer runs
    if (state->msg_timer > 0) {
        oledC_DrawRectangle(SET_X1, SET_Y1, SET_X2, SET_Y2, COLOR_BG);
        oledC_DrawString(SET_TX, SET_TY, 1, 1, (uint8_t*)state->confirm_msg, COLOR_TEXT);
        state->msg_timer--;
    } else {
        oledC_DrawRectangle(SET_X1, SET_Y1, SET_X2, SET_Y2, COLOR_BG);
    }

    // Redraw selection box only when cursor changes
    if (cursor != state->last_drawn_box) {

        // Erase old box
        if (state->last_drawn_box >= 0) {
            if (state->last_drawn_box < state->field_count) {
                MenuEditField* oldf = &state->fields[state->last_drawn_box];
                draw_box(oldf->x1, oldf->y1, oldf->x2, oldf->y2, COLOR_BG);
            } else {
                draw_box(state->back_x1, state->back_y1,
                         state->back_x2, state->back_y2, COLOR_BG);
            }
        }

        // Draw new box
        if (cursor < state->field_count) {
            MenuEditField* current = &state->fields[cursor];
            draw_box(current->x1, current->y1, current->x2, current->y2, COLOR_TEXT);
        } else {
            draw_box(state->back_x1, state->back_y1,
                     state->back_x2, state->back_y2, COLOR_TEXT);
        }

        state->last_drawn_box = cursor;
    }

    // Consume external full redraw request here
    *force_redraw = false;
}