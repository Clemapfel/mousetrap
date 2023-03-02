//
// Created by clem on 2/11/23.
//

#include <app/canvas.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    Canvas::LayerLayer::LayerLayer(Canvas* owner)
    : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
    }

    Canvas::LayerLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::LayerLayer::on_area_realize(Widget* widget, LayerLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_post_fx_shader = new Shader();
        instance->_post_fx_shader->create_from_file(get_resource_path() + "shaders/project_post_fx.frag", ShaderType::FRAGMENT);

        instance->on_layer_count_changed();
        instance->on_layer_properties_changed();
        instance->queue_render_tasks();

        area->queue_render();
    }

    void Canvas::LayerLayer::on_area_resize(GLArea*, int w, int h, LayerLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::LayerLayer::queue_render_tasks()
    {
        if (not _area.get_is_realized())
            return;

        _area.make_current();
        _area.clear_render_tasks();

        auto color_offset_scope = active_state->get_color_offset_apply_scope();
        auto flip_scope = active_state->get_image_flip_apply_scope();

        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
        {
            bool should_apply_color_offset = false;
            bool should_apply_flip = false;

            if (color_offset_scope == CURRENT_FRAME or color_offset_scope == EVERYWHERE)
                should_apply_color_offset = true;
            else if (layer_i == active_state->get_current_layer_index())
                should_apply_color_offset = true;

            if (flip_scope == CURRENT_FRAME or flip_scope == EVERYWHERE)
                should_apply_flip = true;
            else if (layer_i == active_state->get_current_layer_index())
                should_apply_flip = true;

            auto task = RenderTask(_layer_shapes.at(layer_i), _post_fx_shader, nullptr, active_state->get_layer(layer_i)->get_blend_mode());

            task.register_int("_apply_color_offset", should_apply_color_offset ? yes : no);
            task.register_int("_apply_flip", should_apply_flip ? yes : no);

            if (should_apply_color_offset)
            {
                task.register_float("_h_offset", _h_offset);
                task.register_float("_s_offset", _s_offset);
                task.register_float("_v_offset", _v_offset);
                task.register_float("_r_offset", _r_offset);
                task.register_float("_g_offset", _g_offset);
                task.register_float("_b_offset", _b_offset);
                task.register_float("_a_offset", _a_offset);
            }

            if (should_apply_flip)
            {
                task.register_int("_flip_horizontally", _flip_horizontally);
                task.register_int("_flip_vertically", _flip_vertically);
            }

            _area.add_render_task(task);
        }

        _area.queue_render();
    }

    void Canvas::LayerLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::LayerLayer::set_offset(Vector2f offset)
    {
        _offset = {offset.x, offset.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::LayerLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        float width = active_state->get_layer_resolution().x / _canvas_size->x;
        float height = active_state->get_layer_resolution().y / _canvas_size->y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        for (auto* shape : _layer_shapes)
            shape->as_rectangle(
            center + Vector2f{-0.5 * width, -0.5 * height},
            center + Vector2f{+0.5 * width, -0.5 * height},
            center + Vector2f{+0.5 * width, +0.5 * height},
            center + Vector2f{-0.5 * width, +0.5 * height});
    }

    void Canvas::LayerLayer::on_layer_image_updated()
    {
        if (not _area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            _layer_shapes.at(i)->set_texture(active_state->get_cell_texture(i, active_state->get_current_frame_index()));

        _area.queue_render();
    }

    void Canvas::LayerLayer::on_layer_count_changed()
    {
        if (not _area.get_is_realized())
            return;

        _area.make_current();

        for (auto* shape : _layer_shapes)
            delete shape;

        _layer_shapes.clear();
        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            auto* shape = _layer_shapes.emplace_back(new Shape());
            shape->set_texture(active_state->get_cell_texture(i, active_state->get_current_frame_index()));
            shape->set_visible(active_state->get_layer(i)->get_is_visible());
            shape->set_color(RGBA(1, 1, 1, active_state->get_layer(i)->get_opacity()));
        }

        reformat();
        queue_render_tasks();
    }

    void Canvas::LayerLayer::on_layer_resolution_changed()
    {
        if (not _area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            _layer_shapes.at(i)->set_texture(active_state->get_cell_texture(i, active_state->get_current_frame_index()));

        reformat();
        _area.queue_render();
    }

    void Canvas::LayerLayer::on_layer_properties_changed()
    {
        if (not _area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            _layer_shapes.at(i)->set_visible(active_state->get_layer(i)->get_is_visible());
            _layer_shapes.at(i)->set_color(RGBA(1, 1, 1, active_state->get_layer(i)->get_opacity()));
        }

        queue_render_tasks();
    }

    void Canvas::LayerLayer::on_layer_frame_selection_changed()
    {
        on_layer_image_updated();
    }

    void Canvas::LayerLayer::on_color_offset_changed()
    {
        const auto& offset = active_state->get_color_offset();

        *_h_offset = offset.at(0);
        *_s_offset = offset.at(1);
        *_v_offset = offset.at(2);
        *_r_offset = offset.at(3);
        *_g_offset = offset.at(4);
        *_b_offset = offset.at(5);
        *_a_offset = offset.at(6);

        if (active_state->get_color_offset_apply_scope() != _color_offset_apply_scope)
        {
            _color_offset_apply_scope = active_state->get_color_offset_apply_scope();
            queue_render_tasks();
        }

        _area.queue_render();
    }

    void Canvas::LayerLayer::on_image_flip_changed()
    {
        const auto& state = active_state->get_image_flip();
        *_flip_horizontally = state.flip_horizontally;
        *_flip_vertically = state.flip_vertically;

        _image_flip_apply_scope = active_state->get_image_flip_apply_scope();
        queue_render_tasks();

        _area.queue_render();
    }
}
