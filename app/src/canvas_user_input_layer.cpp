
//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::UserInputLayer::UserInputLayer(Canvas* canvas)
        : _owner(canvas)
    {
        _proxy.set_opacity(1 / 255.f);
        _proxy.connect_signal_resize(on_area_resize, this);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _click_controller.connect_signal_click_released(on_click_released, this);

        _motion_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_controller.connect_signal_motion(on_motion, this);
        _motion_controller.connect_signal_motion_leave(on_motion_leave, this);

        _key_controller.connect_signal_key_pressed(on_key_pressed, this);
        _key_controller.connect_signal_key_released(on_key_released, this);

        _scroll_controller.connect_signal_scroll_begin(on_scroll_begin, this);
        _scroll_controller.connect_signal_scroll(on_scroll, this);
        _scroll_controller.connect_signal_scroll_end(on_scroll_end, this);

        _drag_controller.connect_signal_drag_begin(on_drag_begin, this);
        _drag_controller.connect_signal_drag_update(on_drag_update, this);
        _drag_controller.connect_signal_drag_end(on_drag_end, this);

        _proxy.add_controller(&_click_controller);
        _proxy.add_controller(&_motion_controller);
        _proxy.add_controller(&_key_controller);
        _proxy.add_controller(&_scroll_controller);
        _proxy.add_controller(&_drag_controller);

        _proxy.set_focusable(true);
        _proxy.set_cursor(GtkCursorType::CELL);
    }

    Canvas::UserInputLayer::operator Widget*()
    {
        return &_proxy;
    }

    void Canvas::UserInputLayer::on_area_resize(GLArea*, int x, int y, UserInputLayer* instance)
    {
        instance->_canvas_size = {x, y};
        instance->update_cursor_pos();
    }

    void Canvas::UserInputLayer::set_scale(float scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;
        update_cursor_pos();
    }

    void Canvas::UserInputLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x / _canvas_size.x, offset.y / _canvas_size.y};
        update_cursor_pos();
    }

    void Canvas::UserInputLayer::update_cursor_pos()
    {
        _owner->set_widget_cursor_position(_widget_space_pos);

        auto x = _widget_space_pos.x;
        auto y = _widget_space_pos.y;

        auto layer_resolution = active_state->get_layer_resolution();

        float width = layer_resolution.x / _canvas_size.x;
        float height = layer_resolution.y / _canvas_size.y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * width, 0.5 * height};
        float pixel_w = width / layer_resolution.x;
        float pixel_h = height / layer_resolution.y;

        Vector2f cursor_pos = Vector2f(x / _canvas_size.x, y / _canvas_size.y);
        _owner->set_cursor_position(Vector2i(
            ((cursor_pos.x - top_left.x) / pixel_w),
            ((cursor_pos.y - top_left.y) / pixel_h)
        ));
    }

    void Canvas::UserInputLayer::on_click_pressed(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance)
    {
        instance->_widget_space_pos = {x, y};
        instance->update_cursor_pos();
    }

    void Canvas::UserInputLayer::on_click_released(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance)
    {
        instance->_widget_space_pos = {x, y};
        instance->update_cursor_pos();
    }

    void Canvas::UserInputLayer::on_motion_enter(MotionEventController*, double x, double y, UserInputLayer* instance)
    {
        instance->_owner->set_cursor_in_bounds(true);
        instance->_widget_space_pos = {x, y};
        instance->update_cursor_pos();
    }

    void Canvas::UserInputLayer::on_motion_leave(MotionEventController*, UserInputLayer* instance)
    {
        instance->_owner->set_cursor_in_bounds(false);
    }

    void Canvas::UserInputLayer::on_motion(MotionEventController*, double x, double y, UserInputLayer* instance)
    {
        instance->_widget_space_pos = {x, y};
        instance->update_cursor_pos();
    }

    bool Canvas::UserInputLayer::on_key_pressed(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, UserInputLayer* instance)
    {
        return true;
    }

    bool Canvas::UserInputLayer::on_key_released(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, UserInputLayer* instance)
    {
        return true;
    }

    void Canvas::UserInputLayer::on_scroll_begin(ScrollEventController*, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_scroll(ScrollEventController*, double x, double y, UserInputLayer* instance)
    {

        if (instance->_scroll_scale_active)
        {
            bool inverted = state::settings_file->get_value_as<bool>("canvas", "scroll_inverted");
            float sensitivity = state::settings_file->get_value_as<float>("canvas", "scroll_sensitivity");

            auto scale = instance->_owner->_scale;
            scale += (inverted ? -1.f : 1) * y * sensitivity;
            instance->_owner->set_scale(scale);
        }
    }

    void Canvas::UserInputLayer::on_scroll_end(ScrollEventController*, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_drag_begin(DragEventController*, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_drag_update(DragEventController*, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_drag_end(DragEventController*, double x, double y, UserInputLayer* instance)
    {

    }
}
