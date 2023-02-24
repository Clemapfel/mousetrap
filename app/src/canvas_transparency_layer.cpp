//
// Created by clem on 2/11/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::TransparencyTilingLayer::TransparencyTilingLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
    }

    Canvas::TransparencyTilingLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::TransparencyTilingLayer::on_area_realize(Widget* widget, TransparencyTilingLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (instance->_shader == nullptr)
        {
            instance->_shader = new Shader();
            instance->_shader->create_from_file(get_resource_path() + "/shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        delete instance->_shape;
        instance->_shape = new Shape();
        instance->reformat();

        area->clear_render_tasks();

        auto task = RenderTask(instance->_shape, instance->_shader);
        task.register_vec2("_canvas_size", instance->_canvas_size);
        area->add_render_task(task);

        area->queue_render();
    }

    void Canvas::TransparencyTilingLayer::on_area_resize(GLArea*, int w, int h, TransparencyTilingLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::TransparencyTilingLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::TransparencyTilingLayer::set_offset(Vector2f offset)
    {
        _offset = {offset.x / _canvas_size->x, offset.y / _canvas_size->y};
        reformat();
        _area.queue_render();
    }

    void Canvas::TransparencyTilingLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        float width = active_state->get_layer_resolution().x / _canvas_size->x;
        float height = active_state->get_layer_resolution().y / _canvas_size->y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        auto align_to_pixelgrid = [&](Vector2f in)
        {
            in.x -= std::fmod(in.x, 1.f / _canvas_size->x);
            in.y -= std::fmod(in.y, 1.f / _canvas_size->y);
            return in;
        };

        _shape->as_rectangle(
            align_to_pixelgrid(center + Vector2f{-0.5 * width, -0.5 * height}),
            align_to_pixelgrid(center + Vector2f{+0.5 * width, -0.5 * height}),
            align_to_pixelgrid(center + Vector2f{+0.5 * width, +0.5 * height}),
            align_to_pixelgrid(center + Vector2f{-0.5 * width, +0.5 * height})
        );
        _shape->set_visible(_visible);
    }

    void Canvas::TransparencyTilingLayer::on_layer_resolution_changed()
    {
        reformat();
        _area.queue_render();
    }

    void Canvas::TransparencyTilingLayer::set_background_visible(bool b)
    {
        _visible = b;

        if (_shape != nullptr)
            _shape->set_visible(_visible);

        _area.queue_render();
    }
}