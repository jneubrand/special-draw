#pragma once
#include <pebble.h>
#include <@smallstoneapps/linked-list/linked-list.h>

struct _GSpecialSessionModifier;

typedef void (* GSpecialSessionModifierCallback)(
        struct _GSpecialSessionModifier * modifier, GBitmap * session_bitmap);
typedef void (* GSpecialSessionModifierDrawCallback)(
        GContext * ctx, struct _GSpecialSessionModifier * modifier,
        GBitmap * session_bitmap);
typedef void (* GSpecialSessionModifierDestroyCallback)(
        struct _GSpecialSessionModifier * modifier);

typedef struct _GSpecialSessionModifier {
    bool overrides_draw;
    union {
        GSpecialSessionModifierCallback modifier_run;
            // ^^ valid if overrides_draw is false
        GSpecialSessionModifierDrawCallback modifier_draw;
            // ^^ valid if overrides_draw is true
    } action;
    void * context;
    GSpecialSessionModifierDestroyCallback destroy;
} GSpecialSessionModifier;

typedef struct {
    GBitmap * old_fbuf;
    GBitmap * new_fbuf;
    uint8_t * initial_data;
    GContext * ctx;
    GSpecialSessionModifier * draw_modifier;
    LinkedRoot * modifier_root;
    GBitmapFormat old_format;
    uint16_t old_row_size;
    GCompOp compositing_mode;
} GSpecialSession;

GSpecialSession * graphics_context_begin_special_draw(GContext * ctx);

void graphics_context_end_special_draw(GSpecialSession * session);

void graphics_context_special_session_add_modifier(
        GSpecialSession * session, GSpecialSessionModifier * modifier);

void graphics_context_special_session_set_compositing_mode(
        GSpecialSession * session, GCompOp op);
