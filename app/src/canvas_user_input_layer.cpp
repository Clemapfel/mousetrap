
//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::UserInputLayer::UserInputLayer(Canvas* canvas)
        : _owner(canvas)
    {
        _proxy.set_expand(true);
        _proxy.set_opacity(0);
        _proxy.connect_signal_resize([](GLArea*, int w, int h, UserInputLayer* instance){
            instance->_canvas_size = {w, h};
        }, this);

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
    }

    Canvas::UserInputLayer::operator Widget*()
    {
        return &_proxy;
    }

    void Canvas::UserInputLayer::on_click_pressed(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_click_released(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_motion_enter(MotionEventController*, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_motion_leave(MotionEventController*, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_motion(MotionEventController*, double x, double y, UserInputLayer* instance)
    {

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
