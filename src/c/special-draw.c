#include <pebble.h>
#include "special-draw.h"

#if defined(PBL_RECT) && !defined(SPECIAL_DRAW_SCREEN_SIZE)
#define SPECIAL_DRAW_SCREEN_SIZE (GSize(144, 168))
#elif defined(PBL_ROUND) && !defined(SPECIAL_DRAW_SCREEN_SIZE)
#define SPECIAL_DRAW_SCREEN_SIZE (GSize(180, 180))
#endif

#if defined(PBL_RECT) && !defined(SPECIAL_DRAW_SCREEN_CENTER)
#define SPECIAL_DRAW_SCREEN_CENTER (GPoint(72, 84))
#elif defined(PBL_ROUND) && !defined(SPECIAL_DRAW_SCREEN_CENTER)
#define SPECIAL_DRAW_SCREEN_CENTER (GPoint(90, 90))
#endif

GSpecialSession * graphics_context_begin_special_draw(GContext * ctx) {
    GSpecialSession * session = malloc(sizeof(GSpecialSession));
    // Defaults
    session->angle = 0;
    session->opacity = GOpacity3;
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

static void set_bitmap_opacity(GBitmap * bitmap, GOpacity opacity) {
    GRect bounds = gbitmap_get_bounds(bitmap);
    uint8_t alpha_1 = opacity >> 1;
    uint8_t alpha_2 = (opacity + 1) >> 1;
    if (alpha_1 >= 3) {
        // Save some time if the bitmap isn't actually supposed to be
        // semi-transparent. You may want to remove this if reusing this code.
        return;
    }
    for (int y = bounds.origin.y; y < bounds.origin.y + bounds.size.h; y++) {
        GBitmapDataRowInfo row_info = gbitmap_get_data_row_info(bitmap, y);
        for (int x = row_info.min_x; x < row_info.max_x; x++) {
            GColor * pixel = (GColor *) &row_info.data[x];
            if (pixel->a == 0b11) {
                pixel->a = ((x + y) % 2) ? alpha_1 : alpha_2;
            }
        }
    }
}

void graphics_context_end_special_draw(GSpecialSession * session) {
    gbitmap_set_data(session->old_fbuf, session->initial_data,
                     session->old_format, session->old_row_size, false);
    set_bitmap_opacity(session->new_fbuf, session->opacity);
    graphics_context_set_compositing_mode(session->ctx, GCompOpSet);
    if (session->angle) {
        graphics_draw_rotated_bitmap(session->ctx, session->new_fbuf,
            SPECIAL_DRAW_SCREEN_CENTER, session->angle,
            SPECIAL_DRAW_SCREEN_CENTER);
    } else {
        graphics_draw_bitmap_in_rect(session->ctx, session->new_fbuf,
            (GRect) {GPointZero, SPECIAL_DRAW_SCREEN_SIZE});
    }
    gbitmap_destroy(session->new_fbuf);
    free(session);
}

void graphics_context_special_session_set_rotation(
        GSpecialSession * session, int32_t angle) {
    APP_LOG(APP_LOG_LEVEL_INFO, "session ptr %p", &session);
    session->angle = angle;
}

void graphics_context_special_session_set_opacity(
        GSpecialSession * session, GOpacity opacity) {
    APP_LOG(APP_LOG_LEVEL_INFO, "session ptr %p", &session);
    session->opacity = opacity;
}
