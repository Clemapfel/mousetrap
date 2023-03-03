#include <app/canvas.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    Canvas::Canvas()
    {
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

        /*
        _x_offset_scrollbar_adjustment.connect_signal_value_changed([](Adjustment* adjustment, Canvas* instance){
            instance->set_offset(adjustment->get_value(), instance->_offset.y);
        }, this);

        _y_offset_scrollbar_adjustment.connect_signal_value_changed([](Adjustment* adjustment, Canvas* instance){
            instance->set_offset(instance->_offset.x, adjustment->get_value());
        }, this);
         */

        _line_visible_button.set_visible(true);
        _line_start_x_pos_button.set_value(25);//1);
        _line_start_y_pos_button.set_value(25);//1);
        _line_end_x_pos_button.set_value(26);//active_state->get_layer_resolution().x);
        _line_end_y_pos_button.set_value(26);//active_state->get_layer_resolution().y);

        _line_box.push_back(&_line_label);
        _line_box.push_back(&_line_visible_button);
        _line_box.push_back(&_line_start_x_pos_button);
        _line_box.push_back(&_line_start_y_pos_button);
        _line_box.push_back(&_line_end_x_pos_button);
        _line_box.push_back(&_line_end_y_pos_button);
        _line_start_y_pos_button.set_margin_end(2 * state::margin_unit);

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
        //_layer_overlay.add_overlay(*_wireframe_layer);
        _layer_overlay.add_overlay(*_user_input_layer);

        _transparency_tiling_layer->operator Widget *()->set_expand(true);
        _layer_layer->operator Widget *()->set_expand(true);
        _onionskin_layer->operator Widget*()->set_expand(true);
        _grid_layer->operator Widget*()->set_expand(true);
        _wireframe_layer->operator Widget*()->set_expand(true);
        _user_input_layer->operator Widget*()->set_expand(true);

        float corner_size = _y_offset_scrollbar->get_preferred_size().natural_size.x;
        _y_offset_scrollbar->set_vexpand(true);
        _x_offset_scrollbar->set_hexpand(true);
        _y_offset_scrollbar->set_margin_bottom(corner_size);

        _reset_view_button.set_expand(false);
        _reset_view_button.set_size_request({corner_size, corner_size});

        _y_scrollbar_and_reset_button_box.push_back(_y_offset_scrollbar);
        _y_scrollbar_and_reset_button_box.set_homogeneous(false);

        _x_scrollbar_and_canvas_box.push_back(&_layer_overlay);
        _x_scrollbar_and_canvas_box.push_back(_x_offset_scrollbar);
        _x_scrollbar_and_canvas_box.set_homogeneous(false);

        _main.set_homogeneous(false);
        _main.push_back(&_x_scrollbar_and_canvas_box);
        _main.push_back(&_y_scrollbar_and_reset_button_box);

        _control_bar.operator Widget*()-> set_vexpand(false);
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

        update_adjustment_bounds();

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
        if (_scale == scale)
            return;

        if (scale < 1)
            scale = 1;

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

    void Canvas::set_canvas_size(Vector2f size)
    {
        _canvas_size = size;
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

    void Canvas::set_horizontal_symmetry_pixel_position(size_t px)
    {
        _horizontal_symmetry_pixel_position = px;
        _symmetry_ruler_layer->set_horizontal_symmetry_pixel_position(px);
        _control_bar.set_horizontal_symmetry_pixel_position(px);
    }

    void Canvas::set_vertical_symmetry_pixel_position(size_t px)
    {
        _vertical_symmetry_pixel_position = px;
        _symmetry_ruler_layer->set_vertical_symmetry_pixel_position(px);
        _control_bar.set_vertical_symmetry_pixel_position(px);
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
        _transparency_tiling_layer->on_layer_resolution_changed();
        _layer_layer->on_layer_resolution_changed();
        _onionskin_layer->on_layer_resolution_changed();
        _grid_layer->on_layer_resolution_changed();
        _symmetry_ruler_layer->on_layer_resolution_changed();
        _brush_shape_layer->on_layer_resolution_changed();
        _control_bar.on_layer_resolution_changed();
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
        _symmetry_ruler_layer->set_cursor_position(_cursor_position);
    }

    void Canvas::set_cursor_in_bounds(bool b)
    {
        _cursor_in_bounds = b;
        _control_bar.set_cursor_in_bounds(b);
        _brush_shape_layer->set_cursor_in_bounds(b);
    }

    void Canvas::set_widget_cursor_position(Vector2f pos)
    {
        _widget_cursor_position = pos;
        _wireframe_layer->set_widget_cursor_position(_widget_cursor_position);
    }

    void Canvas::update_adjustment_bounds()
    {
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

        auto x_adjustment = _x_offset_scrollbar->get_adjustment();
        x_adjustment.set_signal_value_changed_blocked(true);
        x_adjustment.set_lower(0 - width);
        x_adjustment.set_upper(1 + width);
        x_adjustment.set_page_size(width);
        x_adjustment.set_page_increment(1);
        x_adjustment.set_step_increment((1 + 2 * width) / pixel_w);
        x_adjustment.set_value(top_left.x);
        x_adjustment.set_signal_value_changed_blocked(false);

    }
}