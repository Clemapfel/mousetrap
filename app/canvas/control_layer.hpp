// 
// Copyright 2022 Clemens Cords
// Created on 12/2/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::ControlLayer::ControlLayer(Canvas* canvas)
        : CanvasLayer(canvas)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _motion_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_controller.connect_signal_motion(on_motion, this);
        _motion_controller.connect_signal_motion_leave(on_motion_leave, this);
        _area.add_controller(&_motion_controller);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _click_controller.connect_signal_click_released(on_click_released, this);
        _area.add_controller(&_click_controller);
    }

    Canvas::ControlLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::ControlLayer::update()
    {
        // noop
    }

    void Canvas::ControlLayer::update_pixel_position()
    {
        float widget_w = _owner->_canvas_size->x;
        float widget_h = _owner->_canvas_size->y;
        float x = _owner->_current_cursor_position->x;
        float y = _owner->_current_cursor_position->y;

        Vector2f pos = {x / widget_w, y / widget_h};

        // align with texture-space pixel grid
        float w = state::layer_resolution.x / widget_w;
        float h = state::layer_resolution.y / widget_h;

        Vector2f layer_top_left = {0.5 - w / 2, 0.5 - h / 2};
        layer_top_left = to_gl_position(layer_top_left);
        layer_top_left = _owner->_transform->apply_to(layer_top_left);
        layer_top_left = from_gl_position(layer_top_left);

        Vector2f layer_size = {
                state::layer_resolution.x / widget_w,
                state::layer_resolution.y / widget_h
        };

        layer_size *= *_owner->_transform_scale;

        float x_dist = (pos.x - layer_top_left.x);
        float y_dist = (pos.y - layer_top_left.y);

        Vector2f pixel_size = {layer_size.x / state::layer_resolution.x, layer_size.y / state::layer_resolution.y};

        x_dist /= pixel_size.x;
        y_dist /= pixel_size.y;

        x_dist = floor(x_dist);
        y_dist = floor(y_dist);

        *_owner->_previous_pixel_position =  *_owner->_current_pixel_position;
        *_owner->_current_pixel_position = {x_dist, y_dist};

        _owner->_brush_cursor_layer.update();
    }

    void Canvas::ControlLayer::on_realize(Widget*, ControlLayer* instance)
    {}

    void Canvas::ControlLayer::on_resize(GLArea*, int w, int h, ControlLayer* instance)
    {
        *instance->_owner->_canvas_size = {w, h};
        instance->update_pixel_position();
    }

    void Canvas::ControlLayer::on_click_pressed(ClickEventController*, size_t n, double x, double y, ControlLayer* instance)
    {
        *instance->_owner->_click_pressed = true;
        *instance->_owner->_previous_cursor_position = *instance->_owner->_current_cursor_position;
        *instance->_owner->_current_cursor_position = {x, y};
        instance->update_pixel_position();

        if (state::active_tool == BRUSH)
            instance->_owner->resolve_brush_click_pressed();
        else if (state::active_tool == ERASER)
            instance->_owner->resolve_eraser_click_pressed();
    }

    void Canvas::ControlLayer::on_click_released(ClickEventController*, size_t n, double x, double y, ControlLayer* instance)
    {
        *instance->_owner->_click_pressed = false;
        *instance->_owner->_previous_cursor_position = *instance->_owner->_current_cursor_position;
        *instance->_owner->_current_cursor_position = {x, y};
        instance->update_pixel_position();

        if (state::active_tool == BRUSH)
            instance->_owner->resolve_brush_click_released();
        else if (state::active_tool == ERASER)
            instance->_owner->resolve_eraser_click_released();
    }

    void Canvas::ControlLayer::on_motion_enter(MotionEventController*, double x, double y, ControlLayer* instance)
    {
        *instance->_owner->_cursor_in_bounds = true;
        *instance->_owner->_previous_cursor_position = *instance->_owner->_current_cursor_position;
        *instance->_owner->_current_cursor_position = {x, y};
        instance->update_pixel_position();
    }

    void Canvas::ControlLayer::on_motion_leave(MotionEventController*, ControlLayer* instance)
    {
        *instance->_owner->_cursor_in_bounds = false;
        *instance->_owner->_previous_cursor_position = *instance->_owner->_current_cursor_position;
        instance->_owner->_brush_cursor_layer.update();
    }

    void Canvas::ControlLayer::on_motion(MotionEventController*, double x, double y, ControlLayer* instance)
    {
        *instance->_owner->_previous_cursor_position = *instance->_owner->_current_cursor_position;
        *instance->_owner->_current_cursor_position = {x, y};
        instance->update_pixel_position();

        if (state::active_tool == BRUSH)
            instance->_owner->resolve_brush_motion();
        else if (state::active_tool == ERASER)
            instance->_owner->resolve_eraser_motion();
    }
}