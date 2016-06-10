#include "special-draw-opacity.h"

#ifndef PBL_APLITE
static void prv_set_bitmap_opacity(GBitmap * bitmap, GOpacity opacity) {
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

static void prv_run_modifier(GSpecialSessionModifier * modifier,
        GBitmap * session_bitmap) {
    prv_set_bitmap_opacity(session_bitmap, *((GOpacity *) modifier->context));
}

static void prv_destroy_modifier(GSpecialSessionModifier * modifier) {
    free(modifier->context);
    free(modifier);
}

GSpecialSessionModifier * graphics_special_draw_create_opacity_modifier(
        GOpacity opacity) {
    GSpecialSessionModifier * mod = malloc(sizeof(GSpecialSessionModifier));
    mod->overrides_draw = false;
    mod->action.modifier_run = prv_run_modifier;
    mod->destroy = prv_destroy_modifier;
    mod->context = malloc(sizeof(GOpacity));
    *((GOpacity *) mod->context) = opacity;
    return mod;
}
#endif
