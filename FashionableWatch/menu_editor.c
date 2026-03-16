#include "h/menu_editor.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdio.h>

#define COLOR_TEXT 0xFFFF
#define COLOR_BG   0x0000

static void draw_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
    oledC_DrawLine(x1, y1, x2, y1, 1, color);
    oledC_DrawLine(x1, y2, x2, y2, 1, color);
    oledC_DrawLine(x1, y1, x1, y2, 1, color);
    oledC_DrawLine(x2, y1, x2, y2, 1, color);
}

static uint8_t wrap_range(int16_t value, uint8_t min_val, uint8_t max_val)
{
    int16_t range = (int16_t)(max_val - min_val + 1);

    while (value < min_val) value += range;
    while (value > max_val) value -= range;

    return (uint8_t)value;
}

static void draw_field_value(MenuEditField* field)
{
    char buf[16];

    oledC_DrawRectangle(field->x1 + 2, field->y1 + 2, field->x2 - 2, field->y2 - 2, COLOR_BG);

    if (field->type == FIELD_TYPE_TOGGLE) {
        sprintf(buf, "%s", field->edit_value ? "ON" : "OFF");
    } else {
        sprintf(buf, "%02u", field->edit_value);
    }

    oledC_DrawString(field->text_x, field->text_y,
                     field->text_scale_x, field->text_scale_y,
                     (uint8_t*)buf, COLOR_TEXT);
}

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

    for (i = 0; i < state->field_count; i++) {
        state->fields[i].edit_value = *(state->fields[i].value_ptr);
    }
}

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
}

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

    if (cursor != state->last_cursor) {
        state->last_cursor = cursor;
        state->pot_start_val = pot_value;
        state->value_start_val = field->edit_value;
        return false;
    }

    if (field->type == FIELD_TYPE_TOGGLE) {
        return false;
    } else {
        offset = ((int16_t)pot_value - (int16_t)state->pot_start_val) / field->step_div;
        new_val = state->value_start_val + offset;
        new_val = wrap_range(new_val, field->min_value, field->max_value);

        if ((uint8_t)new_val != field->edit_value) {
            field->edit_value = (uint8_t)new_val;
            return true;
        }
    }

    return false;
}

void menu_editor_on_select(MenuEditorState* state, int8_t cursor, bool* request_back)
{
    MenuEditField* field;

    if (!state) return;
    if (!state->initialized) {
        menu_editor_init(state);
    }

    *request_back = false;

    if (cursor == state->field_count) {
        *request_back = true;
        return;
    }

    if (cursor < 0 || cursor >= state->field_count) {
        return;
    }

    field = &state->fields[cursor];

    if (field->type == FIELD_TYPE_TOGGLE) {
        field->edit_value = field->edit_value ? 0 : 1;
    }

    *(field->value_ptr) = field->edit_value;

    state->confirm_msg = "updated";
    state->msg_timer = 100;
}

void menu_editor_draw(MenuEditorState* state, int8_t cursor, volatile bool* force_redraw)
{
    uint8_t i;

    if (!state) return;
    if (!state->initialized) {
        menu_editor_init(state);
    }

    if (state->first_draw) {
        oledC_DrawRectangle(0, 24, 95, 95, COLOR_BG);

        for (i = 0; i < state->field_count; i++) {
            draw_field_value(&state->fields[i]);
        }

        oledC_DrawRectangle(state->back_x1, state->back_y1,
                            state->back_x2, state->back_y2, COLOR_BG);
        oledC_DrawString(state->back_text_x, state->back_text_y,
                         1, 1, (uint8_t*)state->back_label, COLOR_TEXT);

        state->first_draw = false;
    }

    for (i = 0; i < state->field_count; i++) {
        draw_field_value(&state->fields[i]);
    }

    oledC_DrawRectangle(5, 65, 95, 75, COLOR_BG);
    if (state->msg_timer > 0) {
        oledC_DrawString(5, 65, 1, 1, (uint8_t*)state->confirm_msg, COLOR_TEXT);
        state->msg_timer--;
        *force_redraw = true;
    }

    if (state->last_drawn_box >= 0) {
        if (state->last_drawn_box < state->field_count) {
            MenuEditField* oldf = &state->fields[state->last_drawn_box];
            draw_box(oldf->x1, oldf->y1, oldf->x2, oldf->y2, COLOR_BG);
        } else {
            draw_box(state->back_x1, state->back_y1, state->back_x2, state->back_y2, COLOR_BG);
            oledC_DrawString(state->back_text_x, state->back_text_y,
                             1, 1, (uint8_t*)state->back_label, COLOR_TEXT);
        }
    }

    if (cursor < state->field_count) {
        MenuEditField* current = &state->fields[cursor];
        draw_box(current->x1, current->y1, current->x2, current->y2, COLOR_TEXT);
    } else {
        draw_box(state->back_x1, state->back_y1, state->back_x2, state->back_y2, COLOR_TEXT);
    }

    state->last_drawn_box = cursor;
}