#pragma once

typedef enum {
    GOpacity0   = 0b000,
    GOpacity0_5 = 0b001,
    GOpacity1   = 0b010,
    GOpacity1_5 = 0b011,
    GOpacity2   = 0b100,
    GOpacity2_5 = 0b101,
    GOpacity3   = 0b110
} GOpacity;

typedef struct {
    int32_t angle;
    uint16_t old_row_size;
    GBitmapFormat old_format;
    GOpacity opacity;
    GBitmap * old_fbuf;
    GBitmap * new_fbuf;
    uint8_t * initial_data;
    GContext * ctx;
} GSpecialSession;

GSpecialSession * graphics_context_begin_special_draw(GContext * ctx);

void graphics_context_end_special_draw(GSpecialSession * session);

void graphics_context_special_session_set_rotation(
        GSpecialSession * session, int32_t angle);

void graphics_context_special_session_set_opacity(
        GSpecialSession * session, GOpacity opacity);
