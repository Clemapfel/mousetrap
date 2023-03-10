//
// Created by clem on 3/10/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    void Canvas::ToolOptions::update_shader_anchors()
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

    Canvas::ToolOptions::ToolOptions(Canvas* owner)
        : _owner(owner)
    {
        Vector2f size = active_state->get_layer_resolution();
        _origin_x_pos_button.set_value(0.1 * size.x);
        _origin_y_pos_button.set_value(0.1 * size.y);

        _destination_x_pos_button.set_value(0.9 * size.x);
        _destination_y_pos_button.set_value(0.9 * size.y);

        _origin_x_pos_button.connect_signal_value_changed([](SpinButton*, ToolOptions* instance){
            instance->update_shader_anchors();
        }, this);

        _origin_y_pos_button.connect_signal_value_changed([](SpinButton*, ToolOptions* instance){
            instance->update_shader_anchors();
        }, this);

        _destination_x_pos_button.connect_signal_value_changed([](SpinButton*, ToolOptions* instance){
            instance->update_shader_anchors();
        }, this);

        _destination_y_pos_button.connect_signal_value_changed([](SpinButton*, ToolOptions* instance){
            instance->update_shader_anchors();
        }, this);

        update_shader_anchors();

        _dithering_mode_dropdown.push_back(
            &_dithering_none_list_label, 
            &_dithering_none_selected_label,
            [](ToolOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_NONE);
            }, this
        );

        _dithering_mode_dropdown.push_back(
            &_dithering_2x2_list_label,
            &_dithering_2x2_selected_label,
            [](ToolOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_2x2);
            }, this
        );

        _dithering_mode_dropdown.push_back(
            &_dithering_4x4_list_label,
            &_dithering_4x4_selected_label,
            [](ToolOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_4x4);
            }, this
        );

        _dithering_mode_dropdown.push_back(
            &_dithering_8x8_list_label,
            &_dithering_8x8_selected_label,
            [](ToolOptions* instance){
                instance->_owner->_gradient_layer->set_dither_mode(GradientLayer::DITHER_8x8);
            }, this
        );

        _mode_dropdown.push_back(
            &_linear_mode_list_label,
            &_linear_mode_selected_label,
            [](ToolOptions* instance) {
                instance->_owner->_gradient_layer->set_shape_mode(GradientLayer::LINEAR);
            }, this
        );

        _mode_dropdown.push_back(
            &_circular_mode_list_label,
            &_circular_mode_selected_label,
            [](ToolOptions* instance) {
                instance->_owner->_gradient_layer->set_shape_mode(GradientLayer::CIRCULAR);
            }, this
        );

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
            &_mode_dropdown
        })
            _shader_box.push_back(widget);

        _revealer_box.push_back(&_shader_box);
        _revealer.set_child(&_revealer_box);
        _revealer.set_transition_type(TransitionType::SWING_TOP_TO_BOTTOM);

    }

    Canvas::ToolOptions::operator Widget*()
    {
        return &_revealer;
    }

    void Canvas::ToolOptions::on_active_tool_changed()
    {
        // TODO
        _revealer.set_revealed(true);
    }
}
