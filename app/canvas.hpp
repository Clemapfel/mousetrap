// 
// Copyright 2022 Clemens Cords
// Created on 10/12/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/shape.hpp>
#include <include/gl_area.hpp>
#include <include/overlay.hpp>
#include <include/get_resource_path.hpp>
#include <include/aspect_frame.hpp>

#include <app/app_component.hpp>
#include <app/global_state.hpp>

#include <deque>

namespace mousetrap
{
    class Canvas : public AppComponent
    {
        public:
            Canvas();
            operator Widget*();
            void update() override;

        private:
            struct SubtractOverlay
            {
                Shape* top;
                Shape* right;
                Shape* bottom;
                Shape* left;
            };

            Vector2f* _canvas_size = new Vector2f(state::layer_resolution);
            GLTransform* _transform = new GLTransform();
            float _scale = 1;
            void set_scale(float);

            // Layers

            struct LayerShape
            {
                std::vector<Shape*> frames;
            };

            GLArea _layer_area;
            std::deque<LayerShape*> _layer_shapes;
            std::vector<Shape*> _grid_shapes;
            SubtractOverlay _layer_subtract_overlay;

            static void on_layer_area_realize(Widget*, Canvas* instance);
            static void on_layer_area_resize(GLArea*, int, int, Canvas* instance);

            void schedule_layer_render_tasks();

            // Transparency Tiling

            GLArea _transparency_area;
            Shape* _transparency_shape;
            SubtractOverlay _transparency_subtract_overlay;

            static inline Shader* _transparency_shader = nullptr;

            static void on_transparency_area_realize(Widget*, Canvas* instance);
            static void on_transparency_area_resize(GLArea*, int, int, Canvas* instance);

            // Layout

            Overlay _overlay;
    };
}

///

namespace mousetrap
{
    void Canvas::on_transparency_area_realize(Widget* widget, Canvas* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_shader == nullptr)
        {
            _transparency_shader = new Shader();
            _transparency_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_shape = new Shape();
        instance->_transparency_subtract_overlay = SubtractOverlay{
            new Shape(),
            new Shape(),
            new Shape(),
            new Shape()
        };

        on_transparency_area_resize(area, state::layer_resolution.x, state::layer_resolution.y, instance);
        auto transparency_render_task = RenderTask(instance->_transparency_shape, _transparency_shader, nullptr);
        transparency_render_task.register_vec2("_canvas_size", instance->_canvas_size);

        area->add_render_task(transparency_render_task);

        auto& overlay = instance->_transparency_subtract_overlay;
        for (auto* shape : {overlay.top, overlay.bottom, overlay.left, overlay.right})
        {
            shape->set_color(RGBA(0, 0, 0, 0));
            auto task = RenderTask(shape, nullptr, instance->_transform, BlendMode::MULTIPLY);
            area->add_render_task(task);
        }

        area->queue_render();
    }

    void Canvas::on_transparency_area_resize(GLArea* area, int w_in, int h_in, Canvas* instance)
    {
        area->make_current();

        instance->_canvas_size->x = w_in;
        instance->_canvas_size->y = h_in;

        float width = state::layer_resolution.x / instance->_canvas_size->x;
        float height = state::layer_resolution.y / instance->_canvas_size->y;

        float eps = 0.001;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = 10e4;
        float b = 10e4;

        instance->_transparency_shape->as_rectangle(
                {0, 0}, {1, 1}
        );

        instance->_transparency_subtract_overlay.top->as_rectangle(
                {x - a, y - b}, {x + w + a, y - b},
                {x - a, y}, {x + w + a, y}
        );

        instance->_transparency_subtract_overlay.bottom->as_rectangle(
                {x - a, y + h}, {x + w + a, y + h},
                {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        instance->_transparency_subtract_overlay.left->as_rectangle(
                {x - a, y}, {x, y},
                {x, y + h}, {x - a, y + h}
        );

        instance->_transparency_subtract_overlay.right->as_rectangle(
                {x + w, y}, {x + w + a, y},
                {x + w + a, y + h}, {x + w, y + h}
        );

        area->queue_render();
    }

    void Canvas::on_layer_area_realize(Widget* widget, Canvas* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        for (auto& layer : state::layers)
        {
            auto layer_shape = instance->_layer_shapes.emplace_back(new LayerShape());
            for (auto& frame: layer->frames)
                layer_shape->frames.emplace_back(new Shape());
        }

        instance->_layer_subtract_overlay = SubtractOverlay{
                new Shape(),
                new Shape(),
                new Shape(),
                new Shape()
        };

        on_layer_area_resize(area, state::layer_resolution.x, state::layer_resolution.y, instance);
            // also schedules render tasks
    }

    void Canvas::on_layer_area_resize(GLArea* area, int w_in, int h_in, Canvas* instance)
    {
        instance->_canvas_size->x = w_in;
        instance->_canvas_size->y = h_in;

        float width = state::layer_resolution.x / instance->_canvas_size->x;
        float height = state::layer_resolution.y / instance->_canvas_size->y;

        float eps = 0.001;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = (float) std::numeric_limits<int>::max() / 4; // sic
        float b = (float) std::numeric_limits<int>::max() / 4;

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto& layer = state::layers.at(layer_i);
            auto& shape = instance->_layer_shapes.at(layer_i);

            for (size_t frame_i = 0; frame_i < layer->frames.size(); ++frame_i)
            {
                shape->frames.at(frame_i)->as_rectangle(
                    {0.5 - width / 2, 0.5 - height / 2},
                    {width, height}
                );
                shape->frames.at(frame_i)->set_texture(layer->frames.at(frame_i).texture);
            }
        }

        instance->_transparency_shape->as_rectangle(
                {0.5 - width / 2, 0.5 - height / 2},
                {width, height}
        );

        instance->_layer_subtract_overlay.top->as_rectangle(
                {x - a, y - b}, {x + w + a, y - b},
                {x - a, y}, {x + w + a, y}
        );

        instance->_layer_subtract_overlay.bottom->as_rectangle(
                {x - a, y + h}, {x + w + a, y + h},
                {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        instance->_layer_subtract_overlay.left->as_rectangle(
                {x - a, y}, {x, y},
                {x, y + h}, {x - a, y + h}
        );

        instance->_layer_subtract_overlay.right->as_rectangle(
                {x + w, y}, {x + w + a, y},
                {x + w + a, y + h}, {x + w, y + h}
        );

        for (auto* line : instance->_grid_shapes)
            delete line;

        instance->_grid_shapes.clear();

        x = instance->_layer_shapes.at(0)->frames.at(0)->get_top_left().x;
        y = instance->_layer_shapes.at(0)->frames.at(0)->get_top_left().y;
        w = instance->_layer_shapes.at(0)->frames.at(0)->get_size().x;
        h = instance->_layer_shapes.at(0)->frames.at(0)->get_size().y;

        std::cout << x << " " << y << " " << w << " " << h << std::endl;

        for (size_t i = 0; i < state::layer_resolution.x + 1; ++i)
        {
            auto* line = instance->_grid_shapes.emplace_back(new Shape());
            line->as_line(
                    {x + ((float(i) / state::layer_resolution.x) * w), y},
                    {x + ((float(i) / state::layer_resolution.x) * w), y + h}
            );
        }

        for (size_t i = 0; i < state::layer_resolution.y + 1; ++i)
        {
            auto* line = instance->_grid_shapes.emplace_back(new Shape());
            line->as_line(
                    {x, y + ((float(i) / state::layer_resolution.y) * h)},
                    {x + w, y + ((float(i) / state::layer_resolution.y) * h)}
            );
        }

        for (auto* shape : instance->_grid_shapes)
            shape->set_color(RGBA(1, 0, 1, 1));

        instance->schedule_layer_render_tasks();
        area->queue_render();
    }

    void Canvas::schedule_layer_render_tasks()
    {
        _layer_area.clear_render_tasks();

        size_t frame_i = state::current_frame;
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto task = RenderTask(
                _layer_shapes.at(layer_i)->frames.at(frame_i),
                nullptr,
                _transform,
                state::layers.at(layer_i)->blend_mode
            );
            _layer_area.add_render_task(task);
        }

        for (auto* shape : _grid_shapes)
        {
            auto task = RenderTask(shape, nullptr, _transform);
            _layer_area.add_render_task(task);
        }

        auto& overlay = _layer_subtract_overlay;
        for (auto* shape : {overlay.top, overlay.bottom, overlay.left, overlay.right})
        {
            shape->set_color(RGBA(0, 0, 0, 0));
            auto task = RenderTask(shape, nullptr, _transform, BlendMode::MULTIPLY);
            _layer_area.add_render_task(task);
        }

        _layer_area.queue_render();
    }

    Canvas::Canvas()
    {
        _transparency_area.connect_signal_realize(on_transparency_area_realize, this);
        _transparency_area.connect_signal_resize(on_transparency_area_resize, this);
        _layer_area.connect_signal_realize(on_layer_area_realize, this);
        _layer_area.connect_signal_resize(on_layer_area_resize, this);

        for (auto* area : {&_transparency_area, &_layer_area})
        {
            area->set_expand(true);
            area->set_size_request(state::layer_resolution);
        }

        _overlay.set_child(&_transparency_area);
        _overlay.add_overlay(&_layer_area);

        set_scale(8);
    }

    void Canvas::set_scale(float scale)
    {
        _scale = scale;
        _transform->scale(scale, scale);
    }

    Canvas::operator Widget*()
    {
        return &_overlay;
    }

    void Canvas::update()
    {}
}

