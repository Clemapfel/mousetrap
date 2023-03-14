// 
// Copyright 2022 Clemens Cords
// Created on 10/31/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::TransparencyTilingLayer::TransparencyTilingLayer(Canvas* owner)
            : Canvas::CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
    }

    void Canvas::TransparencyTilingLayer::update()
    {
        reformat();
        _area.queue_render();
    }

    Canvas::TransparencyTilingLayer::operator Widget*()
    {
        return &_area;
    }

    Canvas::TransparencyTilingLayer::~TransparencyTilingLayer()
    {
        delete _shape;
        delete _subtract_top;
        delete _subtract_right;
        delete _subtract_bottom;
        delete _subtract_left;
    }

    void Canvas::TransparencyTilingLayer::on_realize(Widget* widget, TransparencyTilingLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_shader == nullptr)
        {
            _shader = new Shader();
            _shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_shape = new Shape();
        instance->_subtract_top = new Shape();
        instance->_subtract_right = new Shape();
        instance->_subtract_bottom = new Shape();
        instance->_subtract_left = new Shape();

        instance->reformat();
        instance->_area.clear_render_tasks();

        auto task = RenderTask(instance->_shape, _shader, nullptr);
        task.register_vec2("_canvas_size", instance->_owner->_canvas_size);
        area->add_render_task(task);

        for (auto* shape : {
                instance->_subtract_top,
                instance->_subtract_right,
                instance->_subtract_bottom,
                instance->_subtract_left})
        {
            shape->set_color(RGBA(0, 0, 0, 0));
            auto task = RenderTask(shape, nullptr, instance->_owner->_transform, BlendMode::MULTIPLY);
            area->add_render_task(task);
        }

        area->queue_render();
    }

    void Canvas::TransparencyTilingLayer::on_resize(GLArea*, int w, int h, TransparencyTilingLayer* instance)
    {
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::TransparencyTilingLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        float width = active_state->layer_resolution.x / _owner->_canvas_size->x;
        float height = active_state->layer_resolution.y / _owner->_canvas_size->y;

        float eps = 0;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = 10e4;
        float b = 10e4;

        auto align_with_pixel_grid = [&](Vector2f in)
        {
            auto pos = in;
            pos *= *_owner->_canvas_size;
            pos.x = round(pos.x);
            pos.y = round(pos.y);
            pos /= *_owner->_canvas_size;
            return pos;
        };

        _shape->as_rectangle(
            align_with_pixel_grid({0, 0}),
            align_with_pixel_grid({1, 0}),
            align_with_pixel_grid({1, 1}),
            align_with_pixel_grid({0, 1})
        );

        _subtract_top->as_rectangle(
                {x - a, y - b}, {x + w + a, y - b},
                {x - a, y}, {x + w + a, y}
        );

        _subtract_bottom->as_rectangle(
                {x - a, y + h}, {x + w + a, y + h},
                {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        _subtract_left->as_rectangle(
                {x - a, y}, {x, y},
                {x, y + h}, {x - a, y + h}
        );

        _subtract_right->as_rectangle(
                {x + w, y}, {x + w + a, y},
                {x + w + a, y + h}, {x + w, y + h}
        );

        _area.queue_render();
    }
}