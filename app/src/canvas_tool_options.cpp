//
// Created by clem on 3/10/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::ToolOptions::ToolOptions(Canvas* owner)
        : _owner(owner),
          _gradient_options(owner),
          _selection_options(owner)
    {
        _gradient_revealer.set_child(_gradient_options);
        _selection_revealer.set_child(_selection_options);

        _main.set_child(&_gradient_revealer);
        _main.add_overlay(&_selection_revealer);

        _main_revealer.set_child(&_main);
    }

    void Canvas::ToolOptions::on_active_tool_changed()
    {
        auto current_tool = active_state->get_active_tool();

        bool gradient = current_tool == ToolID::GRADIENT;
        _gradient_revealer.set_revealed(gradient);

        bool selection =
            current_tool == ToolID::MARQUEE_NEIGHBORHODD_SELECT or
            current_tool == ToolID::MARQUEE_RECTANGLE or
            current_tool == ToolID::MARQUEE_CIRCLE or
            current_tool == ToolID::MARQUEE_POLYGON;

        _selection_revealer.set_revealed(selection);

        _main_revealer.set_revealed(gradient or selection);
        _main_revealer.set_transition_type(TransitionType::SWING_TOP_TO_BOTTOM);
    }

    Canvas::ToolOptions::operator Widget*()
    {
        return &_main_revealer;
    }

    void Canvas::ToolOptions::GradientOptions::update_shader_anchors()
    {
        Vector2f size = active_state->get_layer_resolution();

        Vector2f origin = {
            _origin_x_pos_button.get_value() / size.x,
            _origin_y_pos_button.get_value() / size.y
        };

        Vector2f destination = {
            _destination_x_pos_button.get_value() / size.x,
            _destination_y_pos_button.get_value() / size.y
        };

        _owner->_gradient_layer->set_origin_point(origin);
        _owner->_gradient_layer->set_destination_point(destination);

        std::cout << "(" << origin.x << " | " << origin.y << ") -> (" << destination.x << " | " << destination.y << ")" << std::endl;
    }

    Canvas::ToolOptions::GradientOptions::GradientOptions(Canvas* owner)
        : _owner(owner)
    {
        Vector2f size = active_state->get_layer_resolution();
        _origin_x_pos_button.set_value(0.1 * size.x);
        _origin_y_pos_button.set_value(0.1 * size.y);

        _destination_x_pos_button.set_value(0.9 * size.x);
        _destination_y_pos_button.set_value(0.9 * size.y);

        _origin_x_pos_button.connect_signal_value_changed([](SpinButton*, GradientOptions* instance){
            instance->update_shader_anchors();
        }, this);

        _origin_y_pos_button.connect_signal_value_changed([](SpinButton*, GradientOptions* instance){
            instance->update_shader_anchors();
        }, this);

        _destination_x_pos_button.connect_signal_value_changed([](SpinButton*, GradientOptions* instance){
            instance->update_shader_anchors();
        }, this);

        _destination_y_pos_button.connect_signal_value_changed([](SpinButton*, GradientOptions* instance){
            instance->update_shader_anchors();
        }, this);

        update_shader_anchors();

        _dithering_mode_dropdown.push_back(
            &_dithering_none_list_label, 
            &_dithering_none_selected_label,
            [](GradientOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_NONE);
            }, this
        );

        _dithering_mode_dropdown.push_back(
            &_dithering_2x2_list_label,
            &_dithering_2x2_selected_label,
            [](GradientOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_2x2);
            }, this
        );

        _dithering_mode_dropdown.push_back(
            &_dithering_4x4_list_label,
            &_dithering_4x4_selected_label,
            [](GradientOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_4x4);
            }, this
        );

        _dithering_mode_dropdown.push_back(
            &_dithering_8x8_list_label,
            &_dithering_8x8_selected_label,
            [](GradientOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_8x8);
            }, this
        );

        _mode_dropdown.push_back(
            &_linear_mode_list_label,
            &_linear_mode_selected_label,
            [](GradientOptions* instance) {
                instance->_owner->_gradient_layer->set_shape_mode(GradientLayer::LINEAR);
            }, this
        );

        _mode_dropdown.push_back(
            &_circular_mode_list_label,
            &_circular_mode_selected_label,
            [](GradientOptions* instance) {
                instance->_owner->_gradient_layer->set_shape_mode(GradientLayer::CIRCULAR);
            }, this
        );

        _clamp_button.connect_signal_toggled([](CheckButton* button, GradientOptions* instance){
            instance->_owner->_gradient_layer->set_clamp(button->get_active());
        }, this);
        _clamp_button.set_active(true);

        float dither_size = 0;
        for (auto* label : {
            &_dithering_none_selected_label,
            &_dithering_none_list_label,
            &_dithering_2x2_selected_label,
            &_dithering_2x2_list_label,
            &_dithering_4x4_selected_label,
            &_dithering_4x4_list_label,
            &_dithering_8x8_selected_label,
            &_dithering_8x8_list_label
        })
            dither_size = std::max(dither_size, label->get_preferred_size().natural_size.x);

        for (auto* label : {
            &_dithering_none_selected_label,
            &_dithering_none_list_label,
            &_dithering_2x2_selected_label,
            &_dithering_2x2_list_label,
            &_dithering_4x4_selected_label,
            &_dithering_4x4_list_label,
            &_dithering_8x8_selected_label,
            &_dithering_8x8_list_label
        })
            label->set_size_request({dither_size, 0});

        float mode_size = 0;
        for (auto* label : {
            &_circular_mode_list_label,
            &_circular_mode_selected_label,
            &_linear_mode_selected_label,
            &_linear_mode_list_label
        })
            mode_size = std::max(mode_size, label->get_preferred_size().natural_size.x);

        for (auto* label : {
            &_circular_mode_list_label,
            &_circular_mode_selected_label,
            &_linear_mode_selected_label,
            &_linear_mode_list_label
        })
            label->set_size_request({mode_size, 0});

        for (auto* widget : std::vector<Widget*>{
            &_origin_label,
            &_origin_x_pos_button,
            &_origin_y_pos_button,
            &_destination_label,
            &_destination_x_pos_button,
            &_destination_y_pos_button,
            &_dithering_label,
            &_dithering_mode_dropdown,
            &_mode_label,
            &_mode_dropdown,
            &_clamp_label,
            &_clamp_button
        })
            _main.push_back(widget);
    }

    Canvas::ToolOptions::GradientOptions::operator Widget*()
    {
        return &_main;
    }

    Canvas::ToolOptions::SelectionOptions::SelectionOptions(Canvas* owner)
        : _owner(owner)
    {
        _mode_dropdown.push_back(&_replace_list_label, &_replace_selected_label, [](auto){
            active_state->set_selection_mode(SelectionMode::REPLACE);
        }, this);

        _mode_dropdown.push_back(&_add_list_label, &_add_selected_label, [](auto){
            active_state->set_selection_mode(SelectionMode::ADD);
        }, this);

        _mode_dropdown.push_back(&_subtract_list_label, &_subtract_selected_label, [](auto){
            active_state->set_selection_mode(SelectionMode::SUBTRACT);
        }, this);

        _main.push_back(&_mode_label);
        _main.push_back(&_mode_dropdown);
    }

    Canvas::ToolOptions::SelectionOptions::operator Widget*()
    {
        return &_main;
    }
}
