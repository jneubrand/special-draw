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
3. Change the rotation and opacity values.
4. End the special-draw procedure.

It should be noted that special-draw is a relatively cpu-heavy operation.
For this reason, it's generally better to

### Example

    graphics_draw_rect(ctx, GRect(32, 54, 80, 60));
    // Begin the session.
    GSpecialSession * session = graphics_context_begin_special_draw(ctx);
    // Set the opacity (using 2/3)
    graphics_context_special_session_set_opacity(session, GOpacity2);
    graphics_draw_rect(ctx, GRect(32, 54, 80, 60));
    graphics_context_end_special_draw(session);

### API

#### Base API

`GSpecialSession * graphics_context_begin_special_draw(GContext * ctx)`

>Begins a special-draw session. You must keep the returned pointer to the
    `GSpecialSession` (it is used to end the session)

`void graphics_context_end_special_draw(GSpecialSession * session)`

>Ends this `GSpecialSession` and destroys the associated data structure.
    You must call this exactly once for every special-draw session opened.

#### Effects

`void graphics_context_special_session_set_rotation(
        GSpecialSession * session, int32_t angle)`

Default rotation: 0

>Sets the rotation used for the `GSpecialSession`. Expects a `TRIGANGLE`.
    May be called any time between beginning and ending the session.

`void graphics_context_special_session_set_opacity(
        GSpecialSession * session, GOpacity opacity)`

Default opacity: `GOpacity3`

>Sets the rotation used for the `GSpecialSession`. May be called any time between beginning and ending the session.

#### `enum`s

    typedef enum {
        GOpacity0,
        GOpacity0_5,
        GOpacity1,
        GOpacity1_5,
        GOpacity2,
        GOpacity2_5,
        GOpacity3
    } GOpacity;

0: completely invisible (somewhat pointless)
3: fully visible (default)

Intermediate stages lead to (checkerboard) dithered transparency. This may lead to disappearing diagonal lines.

Caveats
-------

- Nesting `GSpecialSession`s is not supported.
    (reason unclear, research needed.)
- Mixing a dithered opacity setting such as `GOpacity1_5` with rotation that is
    not a multiple of 90 degrees will lead to weird interference
    - Nesting `GSpecialSession`s, once supported, will resolve this.
- Settings apply to the entire `GSpecialSession`, not just
    the part drawn after they are applied.
- Arbitrary center points for rotation are currently not supported.

Ideally, most of these issues could be solved by moving from the
    current design to a system in which arbitrary effects can be
    defined and added, and are applied in order after drawing is
    completed.

Thanks
------

...to @mhungerford for creating
    [pebble_offscreen_rendering_text_demo](https://github.com/mhungerford/pebble_offscreen_rendering_text_demo),
    which displays the concept behind offscreen rendering on Pebble.

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
