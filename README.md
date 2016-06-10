special-draw
============

special-draw is a very small pebble library that lets you draw rotated and/or semi-transparent primitives and text.

Installation
------------

```
pebble package install special-draw
```

You must be using a new-style project; install the latest pebble tool and SDK
and run `pebble convert-project` on your app if you aren't.

Usage
-----

Usage is quite simple:

1. Begin the special-draw procedure.
2. Draw whatever primitives/text you'd like to.
3. Add modifiers to achieve the desired effect.
4. End the special-draw procedure.

It should be noted that special-draw is a relatively
cpu-heavy operation and takes about 24kb of memory.
For this reason, it's generally better to use
alternatives such as GPath when possible.

### Example

```c
graphics_draw_rect(ctx, GRect(32, 54, 80, 60));

// Begin the session.
GSpecialSession * session = graphics_context_begin_special_draw(ctx);

graphics_draw_rect(ctx, GRect(42, 64, 80, 60));

// Add modifiers as desired.
graphics_context_special_session_add_modifier(session, graphics_special_draw_create_opacity_modifier(GOpacity2));
graphics_context_special_session_add_modifier(session, graphics_special_draw_create_rotation_modifier(TRIG_MAX_ANGLE / 100));

// End the session.
graphics_context_end_special_draw(session);
```

The above code requires no further cleanup since special-draw instances are
automatically destroyed after use.

### API

#### Base API

`GSpecialSession * graphics_context_begin_special_draw(GContext * ctx)`

>Begins a special-draw session. You must keep the returned pointer to the
    `GSpecialSession` as it is used to end the session.

`void graphics_context_end_special_draw(GSpecialSession * session)`

>Ends this `GSpecialSession` and destroys the associated data structure,
    including all modifiers.
>You must call this exactly once for every special-draw session opened.

`void graphics_context_special_session_set_compositing_mode(GSpecialSession * session, GCompOp compositing_mode)`

>Change the compositing mode used by the default draw
    implementation. This may not be honored by custom draw
    modifiers. Defaults are `GCompOpSet` for color and
    `GCompOpAnd` for B/W screens.

>This is usually not necessary for color screens. See **Caveats**
    below for more information on compositing modes for B/W
    screens.

>Default for B/W: `GCompOpClear`, Default for 6+2-bit color: `GCompOpSet`

#### Modifier API

`void graphics_context_special_session_add_modifier(
        GSpecialSession * session, GSpecialSessionModifier * modifier)`

>Add a modifier to the internal list of modifiers.

>Note that there can only be one `draw`-type modifier, and
>attempting to add more will replace (and destroy) the
>previous one.
>(currently, the only effect that requires a draw-type modifier is rotation.)

### Included Modifiers

#### special-draw-rotation.h

`GSpecialSessionModifier * graphics_special_draw_create_rotation_modifier(
    int32_t angle)`

>Creates a rotation modifier. This is a `draw`-type modifier.

- Mixing a dithered opacity setting such as `GOpacity1_5`
    with a rotation that is not a multiple of 90 degrees
    will lead to weird interference.
- Arbitrary center points for rotation are currently not
    supported. (this is low-hanging fruit—feel free to submit
    a pull request!)

#### special-draw-opacity.h

`GSpecialSessionModifier * graphics_special_draw_create_opacity_modifier(
    GOpacity opacity);`

>Not supported on Aplite.

>Creates an opacity modifier. This is a normal-type modifier.

Choose an opacity from this enum:

```c
    typedef enum {
        GOpacity0,
        GOpacity0_5,
        GOpacity1,
        GOpacity1_5,
        GOpacity2,
        GOpacity2_5,
        GOpacity3
    } GOpacity;
```

0: completely invisible (somewhat pointless) to 3: fully visible

Intermediate stages lead to (checkerboarded) dithered
transparency. This may lead to disappearing or
dashed diagonal lines.

### Making your own modifier

Modifiers get full access to the GBitmap which
primitives were rendered to, and draw-type modifiers
also get access to the GContext (as they are supposed
to draw the GBitmap to the GContext.)

Modifiers are a struct, defined in special-draw.h, with the
following structure:

```c
(GSpecialSessionModifier) {
    bool overrides_draw;
    union {
        GSpecialSessionModifierCallback modifier_run;
        GSpecialSessionModifierDrawCallback modifier_draw;
    } action;
    void * context;
    GSpecialSessionModifierDestroyCallback destroy;
}
```

A pointer to this structure should be passed to
`graphics_context_special_session_add_modifier`.

If `overrides_draw` is true, `modifier_draw` must be set.
Otherwise, `modifier_run` must be set.

This is a bare-bones modifier example:

```c
#include "special-draw-opacity.h"

static void prv_run_modifier(GSpecialSessionModifier * modifier,
        GBitmap * session_bitmap) {
    // Insert bitmap manipulation code here.
}

static void prv_destroy_modifier(GSpecialSessionModifier * modifier) {
    free(modifier->context);
    free(modifier);
}

GSpecialSessionModifier * graphics_special_draw_create_pointless_modifier() {
    GSpecialSessionModifier * mod = malloc(sizeof(GSpecialSessionModifier));
    mod->overrides_draw = false;
    mod->action.modifier_run = prv_run_modifier;
    mod->destroy = prv_destroy_modifier;
    mod->context = malloc(sizeof(uint8_t));
    *((uint8_t *) mod->context) = 42;
    return mod;
}
```

Caveats
-------

- Nesting `GSpecialSession`s is not supported and probably
    won't be due to the high memory requirements.
- Settings apply to the entire `GSpecialSession`, not just
    the part drawn after they are applied.
- Since the B/W framebuffer does not natively support
    transparency, it it **not** possible to draw a mixed
    black-and-white image on B/W screens using this library.
    - The created framebuffer is completely filled with black
    on these platforms, and you may want to use
    `graphics_fill_rect` to fill it with white for certain
    `GCompOp`s.
    - `GCompOpClear` (the default), for example, lets you draw in 
    white and [effectively] maps black -> transparent and
    white -> black.

Thanks
------

...to @mhungerford for creating
    [pebble_offscreen_rendering_text_demo](https://github.com/mhungerford/pebble_offscreen_rendering_text_demo),
which shows the concept behind offscreen rendering on Pebble.

Licensing information
---------------------

This project is under the following license:

>MIT License

>Copyright (c) 2016 Johannes Neubrand

>Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

>The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

**NOTE**: This README.md file is adapted from https://github.com/Katharine/pebble-events/blob/master/README.md, which is under the MIT license:

>MIT License

>Copyright (c) 2016 Katharine Berry

>Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

>The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
