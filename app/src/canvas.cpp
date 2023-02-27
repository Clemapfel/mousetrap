#include <app/canvas.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    Canvas::Canvas()
    {
        _background_visible_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->set_background_visible(button->get_active());
        }, this);

        _scale_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_scale(scale->get_value());
        }, this);

        _x_offset_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_offset(scale->get_value(), instance->_offset.y);
        }, this);

        _y_offset_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_offset(instance->_offset.x, scale->get_value());
        }, this);

        _grid_visible_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->set_grid_visible(button->get_active());
        }, this);

        _h_ruler_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->_symmetry_ruler_layer->set_horizontal_symmetry_pixel_position(scale->get_value());
        }, this);

        _h_ruler_active_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->_symmetry_ruler_layer->set_horizontal_symmetry_active(button->get_active());
        }, this);

        _v_ruler_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->_symmetry_ruler_layer->set_vertical_symmetry_pixel_position(scale->get_value());
        }, this);

        _v_ruler_active_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->_symmetry_ruler_layer->set_vertical_symmetry_active(button->get_active());
        }, this);

        _brush_outline_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->set_brush_outline_visible(button->get_active());
        }, this);

        /*
        _line_visible_button.connect_signal_toggled([](CheckButton* button, Canvas* instance) {
            //instance->set_line_visible(button->get_active());
        }, this);
         */

        _line_start_x_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto& start = instance->_origin;
            auto& end = instance->_destination;
            start.x = scale->get_value();
            instance->_wireframe_layer->set_a(start);
            instance->_wireframe_layer->set_b(end);
        }, this);

        _line_start_y_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto& start = instance->_origin;
            auto& end = instance->_destination;
            start.y = scale->get_value();
            instance->_wireframe_layer->set_a(start);
            instance->_wireframe_layer->set_b(end);
        }, this);

        _line_end_x_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto& start = instance->_origin;
            auto& end = instance->_destination;
            end.x = scale->get_value();
            instance->_wireframe_layer->set_a(start);
            instance->_wireframe_layer->set_b(end);
        }, this);

        _line_end_y_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto& start = instance->_origin;
            auto& end = instance->_destination;
            end.y = scale->get_value();
            instance->_wireframe_layer->set_a(start);
            instance->_wireframe_layer->set_b(end);
        }, this);

        _draw_button.connect_signal_clicked([](Button* button, Canvas* instance) {
            instance->draw(instance->_wireframe_layer->draw());
        }, this);

        _draw_button.set_child(&_draw_button_label);

        _scale_button.set_value(50);
        _x_offset_button.set_value(_offset.x);
        _y_offset_button.set_value(_offset.y);
        _grid_visible_button.set_active(_grid_visible);
        _background_visible_button.set_active(_background_visible);
        _h_ruler_button.set_value(0.5 * active_state->get_layer_resolution().x);
        _v_ruler_button.set_value(0.5 * active_state->get_layer_resolution().y);
        _h_ruler_active_button.set_active(false);
        _v_ruler_active_button.set_active(false);

        _line_visible_button.set_visible(true);
        _line_start_x_pos_button.set_value(25);//1);
        _line_start_y_pos_button.set_value(25);//1);
        _line_end_x_pos_button.set_value(26);//active_state->get_layer_resolution().x);
        _line_end_y_pos_button.set_value(26);//active_state->get_layer_resolution().y);

        _draw_button.set_margin_horizontal(state::margin_unit);

        _background_visible_box.push_back(&_draw_button);
        _background_visible_box.push_back(&_background_visible_label);
        _background_visible_box.push_back(&_background_visible_button);

        _transform_box.push_back(&_scale_label);
        _transform_box.push_back(&_scale_button);
        _transform_box.push_back(&_offset_label);
        _transform_box.push_back(&_x_offset_button);
        _transform_box.push_back(&_y_offset_button);

        _grid_box.push_back(&_grid_visible_label);
        _grid_box.push_back(&_grid_visible_button);

        _ruler_box.push_back(&_ruler_label);
        _ruler_box.push_back(&_h_ruler_active_button);
        _ruler_box.push_back(&_h_ruler_button);
        _ruler_box.push_back(&_v_ruler_active_button);
        _ruler_box.push_back(&_v_ruler_button);

        _brush_box.push_back(&_brush_label);
        _brush_box.push_back(&_brush_outline_button);

        _line_box.push_back(&_line_label);
        _line_box.push_back(&_line_visible_button);
        _line_box.push_back(&_line_start_x_pos_button);
        _line_box.push_back(&_line_start_y_pos_button);
        _line_box.push_back(&_line_end_x_pos_button);
        _line_box.push_back(&_line_end_y_pos_button);
        _line_start_y_pos_button.set_margin_end(2 * state::margin_unit);

        _debug_box.push_back(&_background_visible_box);
        _debug_box.push_back(&_transform_box);
        _debug_box.push_back(&_grid_box);
        _debug_box.push_back(&_ruler_box);
        _debug_box.push_back(&_brush_box);
        _debug_box.push_back(&_line_box);

        auto* sep = new SeparatorLine();
        sep->set_expand(true);

        _layer_overlay.set_child(sep);
        _layer_overlay.add_overlay(*_transparency_tiling_layer);
        _layer_overlay.add_overlay(*_layer_layer);
        _layer_overlay.add_overlay(*_onionskin_layer);
        _layer_overlay.add_overlay(*_brush_shape_layer);
        _layer_overlay.add_overlay(*_grid_layer);
        _layer_overlay.add_overlay(*_selection_layer);
        _layer_overlay.add_overlay(*_symmetry_ruler_layer);
        _layer_overlay.add_overlay(*_wireframe_layer);
        _layer_overlay.add_overlay(*_user_input_layer);

        _transparency_tiling_layer->operator Widget *()->set_expand(true);
        _layer_layer->operator Widget *()->set_expand(true);
        _onionskin_layer->operator Widget*()->set_expand(true);
        _grid_layer->operator Widget*()->set_expand(true);
        _wireframe_layer->operator Widget*()->set_expand(true);
        _user_input_layer->operator Widget*()->set_expand(true);

        _main.set_homogeneous(false);
        _debug_box.set_vexpand(false);

        _control_bar.operator Widget*()-> set_vexpand(false);

        _main.push_back(&_layer_overlay);
        _main.push_back(_control_bar);
        _main.push_back(&_debug_box);
        _main.set_expand(true);

        using namespace state::actions;
        canvas_toggle_grid_visible.set_stateful_function([](bool){
            auto next = not state::canvas->_grid_visible;
            state::canvas->set_grid_visible(next);
            return next;
        });

        canvas_toggle_brush_outline_visible.set_stateful_function([](bool) {
           auto next = not state::canvas->_brush_outline_visible;
           state::canvas->set_brush_outline_visible(next);
           return next;
        });

        canvas_toggle_horizontal_symmetry_active.set_stateful_function([](bool) {
            auto next = not state::canvas->_horizontal_symmetry_active;
            state::canvas->set_horizontal_symmetry_active(next);
            return next;
        });

        canvas_toggle_vertical_symmetry_active.set_stateful_function([](bool) {
            auto next = not state::canvas->_vertical_symmetry_active;
            state::canvas->set_vertical_symmetry_active(next);
            return next;
        });

        canvas_toggle_background_visible.set_stateful_function([](bool){
           auto next = not state::canvas->_background_visible;
           state::canvas->set_background_visible(next);
           return next;
        });

        canvas_reset_transform.set_function([](){
           state::canvas->set_scale(1);
           state::canvas->set_offset(0, 0);
        });

        for (auto* action : {
            &canvas_toggle_grid_visible,
            &canvas_open_grid_color_picker,
            &canvas_toggle_brush_outline_visible,
            &canvas_toggle_horizontal_symmetry_active,
            &canvas_toggle_vertical_symmetry_active,
            &canvas_open_symmetry_color_picker,
            &canvas_reset_transform,
            &canvas_toggle_background_visible
        })
            state::add_shortcut_action(*action);

        set_scale(_scale);
        set_offset(_offset.x, _offset.y);
        set_grid_visible(_grid_visible);
        set_brush_outline_visible(_brush_outline_visible);
        set_background_visible(_background_visible);
        set_horizontal_symmetry_active(_horizontal_symmetry_active);
        set_vertical_symmetry_active(_vertical_symmetry_active);
        set_cursor_position(_cursor_position);
        set_cursor_in_bounds(_cursor_in_bounds);
    }

    Canvas::operator Widget*()
    {
        return &_main;
    }

    void Canvas::set_scale(float scale)
    {
        _scale = scale;
        _transparency_tiling_layer->set_scale(_scale);
        _layer_layer->set_scale(_scale);
        _onionskin_layer->set_scale(_scale);
        _grid_layer->set_scale(_scale);
        _symmetry_ruler_layer->set_scale(_scale);
        _brush_shape_layer->set_scale(_scale);
        _wireframe_layer->set_scale(_scale);
        _selection_layer->set_scale(_scale);
        _user_input_layer->set_scale(_scale);
        _control_bar.set_scale(_scale);
    }

    void Canvas::set_offset(float x, float y)
    {
        _offset = {x, y};
        _transparency_tiling_layer->set_offset(_offset);
        _layer_layer->set_offset(_offset);
        _onionskin_layer->set_offset(_offset);
        _grid_layer->set_offset(_offset);
        _symmetry_ruler_layer->set_offset(_offset);
        _brush_shape_layer->set_offset(_offset);
        _wireframe_layer->set_offset(_offset);
        _selection_layer->set_offset(_offset);
    }

    void Canvas::set_grid_visible(bool b)
    {
        _grid_visible = b;
        _grid_layer->set_visible(b);
        _control_bar.set_grid_visible(b);
        state::actions::canvas_toggle_grid_visible.set_state(b);
    }

    void Canvas::set_background_visible(bool b)
    {
        _background_visible = b;
        _transparency_tiling_layer->set_background_visible(b);
        _control_bar.set_background_visible(b);
        state::actions::canvas_toggle_background_visible.set_state(b);
    }

    void Canvas::set_horizontal_symmetry_active(bool b)
    {
        _horizontal_symmetry_active = b;
        _symmetry_ruler_layer->set_horizontal_symmetry_active(b);
        _control_bar.set_horizontal_symmetry_active(b);
        state::actions::canvas_toggle_horizontal_symmetry_active.set_state(b);
    }

    void Canvas::set_vertical_symmetry_active(bool b)
    {
        _vertical_symmetry_active = b;
        _symmetry_ruler_layer->set_vertical_symmetry_active(b);
        _control_bar.set_vertical_symmetry_active(b);
        state::actions::canvas_toggle_vertical_symmetry_active.set_state(b);
    }

    void Canvas::set_brush_outline_visible(bool b)
    {
        _brush_outline_visible = b;
        _brush_shape_layer->set_brush_outline_visible(_brush_outline_visible);
        state::actions::canvas_toggle_brush_outline_visible.set_state(b);
    }

    void Canvas::on_brush_selection_changed()
    {
        _brush_shape_layer->on_brush_selection_changed();
    }

    void Canvas::on_active_tool_changed()
    {}

    void Canvas::on_color_selection_changed()
    {
        _brush_shape_layer->on_color_selection_changed();
    }

    void Canvas::on_selection_changed()
    {
        _selection_layer->on_selection_changed();
    }

    void Canvas::on_onionskin_visibility_toggled()
    {
        _onionskin_layer->on_onionskin_visibility_toggled();
    }

    void Canvas::on_onionskin_layer_count_changed()
    {
        _onionskin_layer->on_onionskin_layer_count_changed();
    }

    void Canvas::on_layer_image_updated()
    {
        _layer_layer->on_layer_image_updated();
        _onionskin_layer->on_layer_image_updated();
    }

    void Canvas::on_layer_count_changed()
    {
        _layer_layer->on_layer_count_changed();
        _onionskin_layer->on_layer_count_changed();
    }

    void Canvas::on_layer_properties_changed()
    {
        _layer_layer->on_layer_properties_changed();
        _onionskin_layer->on_layer_properties_changed();
    }

    void Canvas::on_layer_resolution_changed()
    {
        _h_ruler_button.set_upper_limit(active_state->get_layer_resolution().x - 1);
        _v_ruler_button.set_upper_limit(active_state->get_layer_resolution().y - 1);

        _transparency_tiling_layer->on_layer_resolution_changed();
        _layer_layer->on_layer_resolution_changed();
        _onionskin_layer->on_layer_resolution_changed();
        _grid_layer->on_layer_resolution_changed();
        _symmetry_ruler_layer->on_layer_resolution_changed();
        _brush_shape_layer->on_layer_resolution_changed();
    }

    void Canvas::on_layer_frame_selection_changed()
    {
        _layer_layer->on_layer_frame_selection_changed();
        _onionskin_layer->on_layer_frame_selection_changed();
    }

    void Canvas::on_color_offset_changed()
    {
        _layer_layer->on_color_offset_changed();
    }

    void Canvas::on_image_flip_changed()
    {
        _layer_layer->on_image_flip_changed();
    }

    void Canvas::draw(const ProjectState::DrawData& data)
    {
        active_state->draw_to_cell({active_state->get_current_layer_index(), active_state->get_current_frame_index()}, data);
    }

    void Canvas::set_cursor_position(Vector2i xy)
    {
        _cursor_position = xy;
        _control_bar.set_cursor_position(_cursor_position);
        _brush_shape_layer->set_cursor_position(_cursor_position);
        _wireframe_layer->set_cursor_position(_cursor_position);
        _symmetry_ruler_layer->set_cursor_position(_cursor_position);
    }

    void Canvas::set_cursor_in_bounds(bool b)
    {
        _cursor_in_bounds = b;
        _control_bar.set_cursor_in_bounds(b);
        _brush_shape_layer->set_cursor_in_bounds(b);
    }
}