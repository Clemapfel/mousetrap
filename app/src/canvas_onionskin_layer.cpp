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
    }

    void Canvas::OnionskinLayer::on_layer_resolution_changed() 
    {
        if (not _area.get_is_realized())
            return;
    }

    void Canvas::OnionskinLayer::on_layer_image_updated()
    {
        if (not _area.get_is_realized())
            return;
    }

    void Canvas::OnionskinLayer::on_layer_properties_changed()
    {
        if (not _area.get_is_realized())
            return;
    }

    void Canvas::OnionskinLayer::on_onionskin_visibility_toggled()
    {
        for (auto* shape : _frame_shapes)
            shape->set_visible(active_state->get_onionskin_visible());

        _area.queue_render();
    }

    void Canvas::OnionskinLayer::on_onionskin_layer_count_changed()
    {
        if (not _area.get_is_realized())
            return;

        size_t n_layers = active_state->get_n_onionskin_layers();
        float opacity = state::settings_file->get_value_as<float>("canvas", "onionskin_max_opacity");
        opacity = glm::clamp<float>(opacity, 0, 1);

        const float opacity_step = opacity / n_layers;
        int current = active_state->get_current_frame_index();
        for (int i = current - n_layers; i < current - 1; ++i)
        {

        }


    }
}
