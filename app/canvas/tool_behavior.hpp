// 
// Copyright 2022 Clemens Cords
// Created on 12/3/22 by clem (mail@clemens-cords.com)
//

#include <app/canvas.hpp>

namespace mousetrap
{
    // BRUSH

    void Canvas::resolve_brush_click_pressed()
    {
        if (*_click_pressed)
        {
            undo_safepoint();
            draw_brush(*_current_pixel_position, state::current_brush, state::primary_color);
        }
    }

    void Canvas::resolve_brush_click_released()
    {}

    void Canvas::resolve_brush_motion()
    {
        if (*_click_pressed)
        {
            draw_brush_line(*_previous_pixel_position, *_current_pixel_position, state::current_brush, state::primary_color);
        }
    }

    // ERASER

    void Canvas::resolve_eraser_click_pressed()
    {
        if (*_click_pressed)
        {
            undo_safepoint();
            draw_brush(*_current_pixel_position, state::current_brush, HSVA(0, 0, 0, 0));
        }
    }

    void Canvas::resolve_eraser_click_released()
    {}

    void Canvas::resolve_eraser_motion()
    {
        if (*_click_pressed)
            draw_brush_line(*_previous_pixel_position, *_current_pixel_position, state::current_brush, HSVA(0, 0, 0, 0));
    }
}