#include <app/canvas.hpp>

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
            instance->_symmetry_ruler_layer->set_horizontal_symmetry_enabled(button->get_active());
        }, this);

        _v_ruler_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->_symmetry_ruler_layer->set_vertical_symmetry_pixel_position(scale->get_value());
        }, this);

        _v_ruler_active_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->_symmetry_ruler_layer->set_vertical_symmetry_enabled(button->get_active());
        }, this);

        _brush_outline_button.connect_signal_toggled([](CheckButton* button, Canvas* instance){
            instance->set_brush_outline_visible(button->get_active());
        }, this);

        _brush_x_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_brush_position({scale->get_value(), instance->_brush_position.y});
        }, this);

        _brush_y_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_brush_position({instance->_brush_position.x, scale->get_value()});
        }, this);

        _line_visible_button.connect_signal_toggled([](CheckButton* button, Canvas* instance) {
            instance->set_line_visible(button->get_active());
        }, this);

        _line_start_x_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto start = instance->_line_origin;
            auto end = instance->_line_destination;
            start.x = scale->get_value();
            instance->set_line(start, end);
        }, this);

        _line_start_y_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto start = instance->_line_origin;
            auto end = instance->_line_destination;
            start.y = scale->get_value();
            instance->set_line(start, end);
        }, this);

        _line_end_x_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto start = instance->_line_origin;
            auto end = instance->_line_destination;
            end.x = scale->get_value();
            instance->set_line(start, end);
        }, this);

        _line_end_y_pos_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            auto start = instance->_line_origin;
            auto end = instance->_line_destination;
            end.y = scale->get_value();
            instance->set_line(start, end);
        }, this);

        _draw_button.connect_signal_clicked([](Button* button, Canvas* instance) {
            instance->draw(instance->_line_tool_layer->draw());
        }, this);

        _draw_button.set_child(&_draw_button_label);

        _scale_button.set_value(50);//_scale);
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

        _brush_x_pos_button.set_value(0);
        _brush_y_pos_button.set_value(0);

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
        _brush_box.push_back(&_brush_x_pos_button);
        _brush_box.push_back(&_brush_y_pos_button);

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

        //_layer_overlay.set_child(*_transparency_tiling_layer);
        //_layer_overlay.add_overlay(*_layer_layer);
        //_layer_overlay.add_overlay(*_onionskin_layer);
        //_layer_overlay.add_overlay(*_brush_shape_layer);
        //_layer_overlay.add_overlay(*_grid_layer);
        //_layer_overlay.add_overlay(*_symmetry_ruler_layer);
        _layer_overlay.add_overlay(*_line_tool_layer);
        //_layer_overlay.add_overlay(*_user_input_layer);

        _transparency_tiling_layer->operator Widget *()->set_expand(true);
        _layer_layer->operator Widget *()->set_expand(true);
        _onionskin_layer->operator Widget*()->set_expand(true);
        _grid_layer->operator Widget*()->set_expand(true);

        _main.set_homogeneous(false);
        _debug_box.set_vexpand(false);

        _main.push_back(&_layer_overlay);
        _main.push_back(&_debug_box);
        _main.set_expand(true);
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
        _line_tool_layer->set_scale(_scale);
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
        _line_tool_layer->set_offset(_offset);
    }

    void Canvas::set_grid_visible(bool b)
    {
        _grid_visible = b;
        _grid_layer->set_visible(_grid_visible);
    }

    void Canvas::set_background_visible(bool b)
    {
        _background_visible = b;
        _transparency_tiling_layer->set_background_visible(_background_visible);
    }

    void Canvas::set_brush_outline_visible(bool b)
    {
        _brush_outline_visible = b;
        _brush_shape_layer->set_brush_outline_visible(_brush_outline_visible);
    }

    void Canvas::set_brush_position(Vector2i pos)
    {
        _brush_position = pos;
        _brush_shape_layer->set_brush_position(_brush_position);
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
    {}

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

    void Canvas::set_line(Vector2i origin, Vector2i destination)
    {
        _line_origin = origin;
        _line_destination = destination;
        _line_tool_layer->set_line_position(_line_origin, _line_destination);
    }

    void Canvas::set_line_visible(bool b)
    {
        _line_visible = b;
        _line_tool_layer->set_line_visible(_line_visible);
    }

    void Canvas::draw(const ProjectState::DrawData& data)
    {
        active_state->draw_to_cell({active_state->get_current_layer_index(), active_state->get_current_frame_index()}, data);
    }
}