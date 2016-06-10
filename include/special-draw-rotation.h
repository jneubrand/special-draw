#pragma once
#include <pebble.h>
#include <@smallstoneapps/linked-list/linked-list.h>
#include "special-draw.h"

GSpecialSessionModifier * graphics_special_draw_create_rotation_modifier(
    int32_t angle);
