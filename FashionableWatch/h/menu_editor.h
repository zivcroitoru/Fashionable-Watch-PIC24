#ifndef MENU_EDITOR_H
#define MENU_EDITOR_H

#include <stdint.h>
#include <stdbool.h>


typedef enum
{
    FIELD_TYPE_RANGE,
    FIELD_TYPE_TOGGLE
} FieldType;

typedef struct
{
    const char* label;

    FieldType type;

    /* points to the real target value */
    uint8_t* value_ptr;

    /* working copy while editing */
    uint8_t edit_value;

    /* for FIELD_TYPE_RANGE */
    uint8_t min_value;
    uint8_t max_value;
    uint8_t step_div;

    /* draw area */
    uint8_t x1;
    uint8_t y1;
    uint8_t x2;
    uint8_t y2;

    /* text draw position */
    uint8_t text_x;
    uint8_t text_y;
    uint8_t text_scale_x;
    uint8_t text_scale_y;
} MenuEditField;

typedef struct
{
    MenuEditField* fields;
    uint8_t field_count;

    const char* back_label;
    uint8_t back_x1;
    uint8_t back_y1;
    uint8_t back_x2;
    uint8_t back_y2;
    uint8_t back_text_x;
    uint8_t back_text_y;

    const char* title;

    uint8_t initialized;
    int8_t last_cursor;
    uint16_t pot_start_val;
    int16_t value_start_val;

    uint32_t msg_timer;
    const char* confirm_msg;

    bool first_draw;
    int8_t last_drawn_box;
    bool values_dirty;
} MenuEditorState;

/* setup/reset */
void menu_editor_init(MenuEditorState* state);
void menu_editor_reset(MenuEditorState* state);

/* logic */
bool menu_editor_update_from_pot(MenuEditorState* state, uint16_t pot_value, int8_t cursor);
void menu_editor_on_select(MenuEditorState* state, int8_t cursor, bool* request_back);

/* draw */
void menu_editor_draw(MenuEditorState* state, int8_t cursor, volatile bool* force_redraw);

#endif