// 
// Copyright 2022 Clemens Cords
// Created on 10/31/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::GridLayer::GridLayer(Canvas* owner)
            : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        update();
    }

    Canvas::GridLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::GridLayer::update()
    {
        if (_grid_visible != _owner->_grid_visible and _area.get_is_realized())
        {
            _grid_visible = _owner->_grid_visible;

            for (auto* shape : _h_lines)
                shape->set_visible(_grid_visible);

            for (auto* shape : _v_lines)
                shape->set_visible(_grid_visible);
        }

        if (_grid_color != _owner->_grid_color and _area.get_is_realized())
        {
            _grid_color = _owner->_grid_color;

            for (auto* shape : _h_lines)
                shape->set_color(_grid_color);

            for (auto* shape : _v_lines)
                shape->set_color(_grid_color);
        }

        if (_canvas_size != *_owner->_canvas_size)
        {
            _canvas_size = *_owner->_canvas_size;
            reformat();
        }

        _area.queue_render();
    }

    Canvas::GridLayer::~GridLayer()
    {
        for (auto* shape : _h_lines)
            delete shape;

        for (auto* shape : _v_lines)
            delete shape;
    }

    void Canvas::GridLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        float w = state::layer_resolution.x / _canvas_size.x;
        float h = state::layer_resolution.y / _canvas_size.y;

        float x = 0.5 - w / 2;
        float y = 0.5 - h / 2;

        for (size_t i = 0; i < state::layer_resolution.x + 1; ++i)
        {
            auto* line = _h_lines.at(i);
            line->as_line(
                {x + ((float(i) / state::layer_resolution.x) * w), y},
                {x + ((float(i) / state::layer_resolution.x) * w), y + h}
            );
            line->set_color(_grid_color);
        }

        for (size_t i = 0; i < state::layer_resolution.y + 1; ++i)
        {
            auto* line = _v_lines.at(i);
            line->as_line(
                    {x, y + ((float(i) / state::layer_resolution.y) * h)},
                    {x + w, y + ((float(i) / state::layer_resolution.y) * h)}
            );
            line->set_color(_grid_color);
        }

        _area.queue_render();
    }

    void Canvas::GridLayer::on_realize(Widget* widget, GridLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        for (size_t i = 0; i < state::layer_resolution.x + 1; ++i)
            instance->_h_lines.emplace_back(new Shape());

        for (size_t i = 0; i < state::layer_resolution.y + 1; ++i)
            instance->_v_lines.emplace_back(new Shape());

        instance->reformat();
        instance->_area.clear_render_tasks();

        for (auto* shape : instance->_h_lines)
        {
            auto task = RenderTask(shape, nullptr, instance->_owner->_transform);
            instance->_area.add_render_task(task);
        }

        for (auto* shape : instance->_v_lines)
        {
            auto task = RenderTask(shape, nullptr, instance->_owner->_transform);
            instance->_area.add_render_task(task);
        }

        area->queue_render();
    }

    void Canvas::GridLayer::on_resize(GLArea* area, int w, int h, GridLayer* instance)
    {
        instance->reformat();
        area->queue_render();
    }
}