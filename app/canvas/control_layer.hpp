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

        _scroll_controller.connect_signal_scroll_begin(on_scroll_begin, this);
        _scroll_controller.connect_signal_scroll(on_scroll, this);
        _scroll_controller.connect_signal_scroll_end(on_scroll_end, this);
        _area.add_controller(&_scroll_controller);

        _scale_step_up_action.set_do_function([](ControlLayer* instance) {
            instance->_owner->set_transform_scale(*(instance->_owner->_transform_scale) + instance->_owner->_transform_scale_step);
            instance->_owner->update();
        }, this);
        _scale_step_up_action.add_shortcut(state::keybindings_file->get_value("canvas", "scale_step_up"));
        state::app->add_action(_scale_step_up_action);
        _shortcut_controller.add_action(_scale_step_up_action.get_id());

        _scale_step_down_action.set_do_function([](ControlLayer* instance) {
            float scale = *(instance->_owner->_transform_scale) - instance->_owner->_transform_scale_step;
            instance->_owner->set_transform_scale(std::max<float>(scale, 0));
            instance->_owner->update();
        }, this);
        _scale_step_down_action.add_shortcut(state::keybindings_file->get_value("canvas", "scale_step_down"));
        state::app->add_action(_scale_step_down_action);
        _shortcut_controller.add_action(_scale_step_down_action.get_id());
        _area.add_controller(&_shortcut_controller);

        _key_controller.connect_signal_key_pressed(on_key_pressed, this);
        _key_controller.connect_signal_key_released(on_key_released, this);
        _key_controller.connect_signal_modifiers_changed(on_modifiers_changed, this);

        auto trigger = state::keybindings_file->get_value("canvas", "scroll_scale_trigger");
        _scroll_scale_trigger = gtk_shortcut_trigger_parse_string(trigger.c_str());
        state::main_window->add_controller(&_key_controller);
    }

    Canvas::ControlLayer::~ControlLayer() noexcept
    {
        g_object_unref(_scroll_scale_trigger);
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

    bool Canvas::ControlLayer::on_key_pressed(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, ControlLayer* instance)
    {
        auto* event = controller->get_current_event();
        auto hash = gtk_shortcut_trigger_hash(instance->_scroll_scale_trigger);

        if (gtk_shortcut_trigger_trigger(instance->_scroll_scale_trigger, event, true) == GDK_KEY_MATCH_EXACT)
            goto trigger;

        // non modifier trigger presses handled in on_modifier_changed

        return false;

        trigger:
            instance->_scroll_scale_active = true;
            std::cout << "active" << std::endl;
            return false;
    }

    bool Canvas::ControlLayer::on_key_released(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state,
                                               ControlLayer* instance)
    {
        static auto shift_hash = gtk_shortcut_trigger_hash(gtk_shortcut_trigger_parse_string("<Shift>"));
        static auto control_hash = gtk_shortcut_trigger_hash(gtk_shortcut_trigger_parse_string("<Control>"));
        static auto alt_hash = gtk_shortcut_trigger_hash(gtk_shortcut_trigger_parse_string("<Alt>"));

        auto* event = controller->get_current_event();
        auto hash = gtk_shortcut_trigger_hash(instance->_scroll_scale_trigger);

        if (hash == shift_hash and state & GDK_SHIFT_MASK)
            goto trigger;

        if (hash == control_hash and state & GDK_CONTROL_MASK)
            goto trigger;

        if (hash == alt_hash and state & GDK_ALT_MASK)
            goto trigger;

        if (gtk_shortcut_trigger_trigger(instance->_scroll_scale_trigger, event, true) == GDK_KEY_MATCH_EXACT)
            goto trigger;

        return false;

        trigger:
            instance->_scroll_scale_active = false;
            std::cout << "inactive" << std::endl;
            return false;
    }

    bool Canvas::ControlLayer::on_modifiers_changed(KeyEventController*, GdkModifierType keyval, ControlLayer* instance)
    {
        static auto shift_hash = gtk_shortcut_trigger_hash(gtk_shortcut_trigger_parse_string("<Shift>"));
        static auto control_hash = gtk_shortcut_trigger_hash(gtk_shortcut_trigger_parse_string("<Control>"));
        static auto alt_hash = gtk_shortcut_trigger_hash(gtk_shortcut_trigger_parse_string("<Alt>"));

        auto hash = gtk_shortcut_trigger_hash(instance->_scroll_scale_trigger);

        if (hash == shift_hash and keyval == GDK_SHIFT_MASK)
            goto trigger;

        if (hash == control_hash and keyval == GDK_CONTROL_MASK)
            goto trigger;

        if (hash == alt_hash and keyval == GDK_ALT_MASK)
            goto trigger;

        // non modifier trigger presses handled in on_key_pressed

        return false;

        trigger:
            instance->_scroll_scale_active = true;
            std::cout << "active" << std::endl;
            return false;
    }

    void Canvas::ControlLayer::on_scroll_begin(ScrollEventController*, ControlLayer* instance)
    {}

    void Canvas::ControlLayer::on_scroll_end(ScrollEventController*, ControlLayer* instance)
    {}

    void Canvas::ControlLayer::on_scroll(ScrollEventController*, double x, double y, ControlLayer* instance)
    {}
}