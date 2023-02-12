//
// Created by clem on 2/11/23.
//

#include <app/canvas.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    Canvas::OnionskinLayer::OnionskinLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
    }

    Canvas::OnionskinLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::OnionskinLayer::on_area_realize(Widget* widget, OnionskinLayer* instance) 
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (instance->_onionskin_shader == nullptr)
        {
            instance->_onionskin_shader = new Shader();
            instance->_onionskin_shader->create_from_file(get_resource_path() + "shaders/onionskin.frag", ShaderType::FRAGMENT);
        }
        
        instance->on_layer_count_changed();
        instance->on_onionskin_layer_count_changed();

        area->clear_render_tasks();
        for (auto* shape : instance->_frame_shapes)
            area->add_render_task(shape, instance->_onionskin_shader);
        
        area->queue_render();
    }

    void Canvas::OnionskinLayer::on_area_resize(GLArea* area, int w, int h, OnionskinLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
        area->queue_render();
    }

    void Canvas::OnionskinLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        float width = active_state->get_layer_resolution().x / _canvas_size->x;
        float height = active_state->get_layer_resolution().y / _canvas_size->y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        for (auto* shape : _frame_shapes)
            shape->as_rectangle(
                center + Vector2f{-0.5 * width, -0.5 * height},
                center + Vector2f{+0.5 * width, -0.5 * height},
                center + Vector2f{+0.5 * width, +0.5 * height},
                center + Vector2f{-0.5 * width, +0.5 * height});
    }

    void Canvas::OnionskinLayer::set_offset(Vector2f offset)
    {
        _offset = offset;
        reformat();
        _area.queue_render();
    }

    void Canvas::OnionskinLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::OnionskinLayer::on_layer_count_changed() 
    {
        if (not _area.get_is_realized())
            return;

        for (auto* shape : _frame_shapes)
            delete shape;

        _frame_shapes.clear();

        for (size_t i = 0; i < active_state->get_n_frames(); ++i)
        {
            auto* shape = _frame_shapes.emplace_back(new Shape());
            shape->set_texture(active_state->get_cell_texture(active_state->get_current_frame_index(), i));
            shape->set_visible(active_state->get_onionskin_visible());
        }

        reformat();

        on_onionskin_layer_count_changed();
        _area.queue_render();
    }
    
    void Canvas::OnionskinLayer::on_layer_frame_selection_changed() 
    {
        if (not _area.get_is_realized())
            return;

        on_onionskin_layer_count_changed();
    }

    void Canvas::OnionskinLayer::on_layer_image_updated()
    {
        if (not _area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_frames(); ++i)
            _frame_shapes.at(i)->set_texture(active_state->get_cell_texture(active_state->get_current_layer_index(), i));

        _area.queue_render();
    }

    void Canvas::OnionskinLayer::on_layer_resolution_changed() 
    {
        if (not _area.get_is_realized())
            return;

        on_layer_image_updated();
        reformat();
    }

    void Canvas::OnionskinLayer::on_layer_properties_changed()
    {
        if (not _area.get_is_realized())
            return;

        on_onionskin_layer_count_changed();
        _area.queue_render();
    }

    void Canvas::OnionskinLayer::on_onionskin_visibility_toggled()
    {
        on_onionskin_layer_count_changed();
    }

    void Canvas::OnionskinLayer::on_onionskin_layer_count_changed()
    {
        if (not _area.get_is_realized())
            return;

        size_t n_frames = active_state->get_n_onionskin_layers();
        float max_opacity = state::settings_file->get_value_as<float>("canvas", "onionskin_max_opacity");

        auto hsv = state::settings_file->get_value_as<std::vector<float>>("canvas", "onionskin_left_color");
        auto left_color = HSVA(hsv.at(0), hsv.at(1), hsv.at(2), 1);
        hsv = state::settings_file->get_value_as<std::vector<float>>("canvas", "onionskin_right_color");
        auto right_color = HSVA(hsv.at(0), hsv.at(1), hsv.at(2), 1);

        int current = active_state->get_current_frame_index();
        for (int i = 0; i < active_state->get_n_frames(); ++i)
        {
            HSVA color;
            bool visible = true;

            if (i < current and i >= current - int(n_frames))
            {
                color = left_color;
                visible = true;
            }
            else if (i > current and i <= current + n_frames)
            {
                color = right_color;
                visible = true;
            }
            else
            {
                color = RGBA(1, 1, 1, 1).operator HSVA();
                visible = false;
            }

            color.a = active_state->get_current_layer()->get_opacity();
            color.a *= (1 - (abs(current - i) - 1) / float(n_frames)) * max_opacity;

            if (not active_state->get_onionskin_visible() or not active_state->get_current_layer()->get_is_visible())
                visible = false;

            _frame_shapes.at(i)->set_color(color);
            _frame_shapes.at(i)->set_visible(visible);
        }

        _area.queue_render();
    }
}
