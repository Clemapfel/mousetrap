// 
// Copyright 2022 Clemens Cords
// Created on 12/16/22 by clem (mail@clemens-cords.com)
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::LineToolLayer::LineToolLayer(Canvas* owner)
        : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _start_point = {1, 1};
        _end_point = {25, 37};
        _anchored = true;
    }

    Canvas::LineToolLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::LineToolLayer::signal_click()
    {
        if (not _anchored)
        {
            _start_point = *_owner->_current_pixel_position;
            _anchored = true;
        }
        else
        {

        }
    }

    void Canvas::LineToolLayer::on_realize(Widget* widget, LineToolLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_start_shape = new Shape();
        instance->_end_shape = new Shape();
        instance->_line_shape = new Shape();

        instance->update();

        area->clear_render_tasks();
        area->add_render_task(instance->_start_shape);
        area->add_render_task(instance->_end_shape);
        area->add_render_task(instance->_line_shape);

        area->queue_render();
    }

    void Canvas::LineToolLayer::on_resize(GLArea* area, int w, int h, LineToolLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->update();
        area->queue_render();
    }

    void Canvas::LineToolLayer::update()
    {
        if (_anchored)
            _end_point = *_owner->_current_pixel_position;

        Vector2f layer_size = {
                state::layer_resolution.x / _canvas_size.x,
                state::layer_resolution.y / _canvas_size.y
        };

        layer_size *= *_owner->_transform_scale;
        Vector2f pixel_size = {layer_size.x / state::layer_resolution.x, layer_size.y / state::layer_resolution.y};

        for (auto* shape : {_start_shape, _end_shape})
            shape->as_wireframe({
                    Vector2f{-0.5 * pixel_size.x, -0.5 * pixel_size.y},
                    Vector2f{0.5 * pixel_size.x, -0.5 * pixel_size.y},
                    Vector2f{0.5 * pixel_size.x, 0.5 * pixel_size.y},
                    Vector2f{-0.5 * pixel_size.x, 0.5 * pixel_size.y},
            });

        float w = state::layer_resolution.x / _canvas_size.x;
        float h = state::layer_resolution.y / _canvas_size.y;

        Vector2f layer_top_left = {0.5 - w / 2, 0.5 - h / 2};
        layer_top_left = to_gl_position(layer_top_left);
        layer_top_left = _owner->_transform->apply_to(layer_top_left);
        layer_top_left = from_gl_position(layer_top_left);

        float x_offset = 0.5;
        float y_offset = 0.5;

        _start_shape->set_centroid({
            layer_top_left.x + (_start_point.x + x_offset) * pixel_size.x,
            layer_top_left.y + (_start_point.y + y_offset) * pixel_size.y
        });

        _end_shape->set_centroid({
            layer_top_left.x + (_end_point.x + x_offset) * pixel_size.x,
            layer_top_left.y + (_end_point.y + y_offset) * pixel_size.y
        });

        _line_shape->as_line(_start_shape->get_centroid(), _end_shape->get_centroid());
        _area.queue_render();

        // TODO
        std::cout << _end_point.x << " " << _end_point.y << std::endl;
        // TODO
    }
}