#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::Canvas()
    {
        _scale_button.set_value(_scale);
        _x_offset_button.set_value(_offset.x);
        _y_offset_button.set_value(_offset.y);

        _scale_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_scale(scale->get_value());
        }, this);

        _x_offset_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_offset(scale->get_value(), instance->_offset.y);
        }, this);

        _y_offset_button.connect_signal_value_changed([](SpinButton* scale, Canvas* instance){
            instance->set_offset(instance->_offset.x, scale->get_value());
        }, this);

        _debug_box.push_back(&_scale_label);
        _debug_box.push_back(&_scale_button);
        _debug_box.push_back(&_offset_label);
        _debug_box.push_back(&_x_offset_button);
        _debug_box.push_back(&_y_offset_button);
        _offset_label.set_margin_start(state::margin_unit * 2);

        _layer_overlay.set_child(*_transparency_tiling_layer);
        _layer_overlay.add_overlay(*_layer_layer);
        _layer_overlay.add_overlay(*_onionskin_layer);

        _transparency_tiling_layer->operator Widget *()->set_expand(true);
        _layer_layer->operator Widget *()->set_expand(true);
        _onionskin_layer->operator Widget*()->set_expand(true);

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
    }

    void Canvas::set_offset(float x, float y)
    {
        _offset = {x, y};
        _transparency_tiling_layer->set_offset(_offset);
        _layer_layer->set_offset(_offset);
        _onionskin_layer->set_offset(_offset);
    }

    void Canvas::on_brush_selection_changed()
    {}

    void Canvas::on_active_tool_changed()
    {}

    void Canvas::on_color_selection_changed()
    {}

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
        _transparency_tiling_layer->on_layer_resolution_changed();
        _layer_layer->on_layer_resolution_changed();
        _onionskin_layer->on_layer_resolution_changed();
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

}