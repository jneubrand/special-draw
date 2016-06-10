#include "special-draw.h"

#if defined(PBL_RECT) && !defined(SPECIAL_DRAW_SCREEN_SIZE)
#define SPECIAL_DRAW_SCREEN_SIZE (GSize(144, 168))
#elif defined(PBL_ROUND) && !defined(SPECIAL_DRAW_SCREEN_SIZE)
#define SPECIAL_DRAW_SCREEN_SIZE (GSize(180, 180))
#endif

GSpecialSession * graphics_context_begin_special_draw(GContext * ctx) {
    GSpecialSession * session = malloc(sizeof(GSpecialSession));
    // Defaults
    session->modifier_root = linked_list_create_root();
    // Initialize
    session->ctx = ctx;
    session->old_fbuf = graphics_capture_frame_buffer(session->ctx);
    session->initial_data = gbitmap_get_data(session->old_fbuf);
    session->old_format = gbitmap_get_format(session->old_fbuf);
    session->old_row_size = gbitmap_get_bytes_per_row(session->old_fbuf);
    graphics_release_frame_buffer(session->ctx, session->old_fbuf);
    session->new_fbuf = gbitmap_create_blank(SPECIAL_DRAW_SCREEN_SIZE,
                                             GBitmapFormat8Bit);
    gbitmap_set_data(session->old_fbuf,
                     gbitmap_get_data(session->new_fbuf),
                     gbitmap_get_format(session->new_fbuf),
                     gbitmap_get_bytes_per_row(session->new_fbuf), false);
    return session;
}

static bool prv_apply_modifier(void * _modifier,
        void * _session) {
    GSpecialSessionModifier * modifier = _modifier;
    GSpecialSession * session = _session;
    modifier->action.modifier_run(modifier, session->new_fbuf);
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "applied linked list item %p",
            modifier);
    return true;
}

static bool prv_destroy_modifier(void * _modifier,
        void * _session) {
    GSpecialSessionModifier * modifier = _modifier;
    modifier->destroy(modifier);
    APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "destroyed linked list item %p",
            modifier);
    return true;
}

void graphics_context_end_special_draw(GSpecialSession * session) {
    gbitmap_set_data(session->old_fbuf, session->initial_data,
                     session->old_format, session->old_row_size, false);
    graphics_context_set_compositing_mode(session->ctx, GCompOpSet);
    linked_list_foreach(session->modifier_root, prv_apply_modifier, session);
    if (session->draw_modifier) {
        session->draw_modifier->action.modifier_draw(
            session->ctx, session->draw_modifier, session->new_fbuf);
        session->draw_modifier->destroy(session->draw_modifier);
    } else {
        graphics_draw_bitmap_in_rect(session->ctx, session->new_fbuf,
            (GRect) {GPointZero, SPECIAL_DRAW_SCREEN_SIZE});
    }
    linked_list_foreach(session->modifier_root, prv_destroy_modifier, session);
    linked_list_clear(session->modifier_root);
    free(session->modifier_root);
    gbitmap_destroy(session->new_fbuf);
    free(session);
}

void graphics_context_special_session_add_modifier(GSpecialSession * session,
        GSpecialSessionModifier * modifier) {
    if (modifier->overrides_draw) {
        if (session->draw_modifier) {
            APP_LOG(APP_LOG_LEVEL_WARNING, "Replacing previous draw modifier"
                "%p with new modifier %p. Destroying previous draw modifier.",
                session->draw_modifier, modifier);
            session->draw_modifier->destroy(session->draw_modifier);
        }
        session->draw_modifier = modifier;
    } else {
        linked_list_append(session->modifier_root, modifier);
    }
}
