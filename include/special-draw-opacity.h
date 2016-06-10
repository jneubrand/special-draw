#pragma once
#include <pebble.h>
#include <@smallstoneapps/linked-list/linked-list.h>
#include "special-draw.h"

typedef enum {
    GOpacity0   = 0b000,
    GOpacity0_5 = 0b001,
    GOpacity1   = 0b010,
    GOpacity1_5 = 0b011,
    GOpacity2   = 0b100,
    GOpacity2_5 = 0b101,
    GOpacity3   = 0b110
} GOpacity;

GSpecialSessionModifier * graphics_special_draw_create_opacity_modifier(
    GOpacity opacity);
