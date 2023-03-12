// 
// Copyright 2022 Clemens Cords
// Created on 12/10/22 by clem (mail@clemens-cords.com)
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::SelectionLayer::SelectionLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _area.add_tick_callback([](FrameClock clock, SelectionLayer* instance) -> bool {

            if (state::settings_file->get_value_as<bool>("canvas", "selection_outline_animated"))
                *instance->_outline_time_s += clock.get_time_since_last_frame().as_seconds();

            instance->_area.queue_render();
            return true;
        }, this);

        set_scale(_owner->_scale);
        set_offset(_owner->_offset);
    }

    Canvas::SelectionLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::SelectionLayer::reschedule_render_tasks()
    {
        _area.clear_render_tasks();

        auto outline_task = RenderTask(_outline_outline, nullptr, nullptr, BlendMode::REVERSE_SUBTRACT);
        _area.add_render_task(outline_task);

        auto tasks = std::vector<RenderTask>();

        {
            auto& task = tasks.emplace_back(_outline_left_to_right, _outline_shader);
            task.register_int("_direction", _outline_shader_top_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_top_to_bottom, _outline_shader);
            task.register_int("_direction", _outline_shader_right_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_right_to_left, _outline_shader);
            task.register_int("_direction", _outline_shader_bottom_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_bottom_to_top, _outline_shader);
            task.register_int("_direction", _outline_shader_left_flag);
        }

        for (auto& task : tasks)
        {
            task.register_float("_time_s", _outline_time_s);
            task.register_vec2("_canvas_size", _canvas_size);
            task.register_int("_animation_paused", _animation_paused);
            _area.add_render_task(task);
        }

        _area.queue_render();
    }

    void Canvas::SelectionLayer::on_realize(Widget* widget, SelectionLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_outline_shader = new Shader();
        instance->_outline_shader->create_from_file(get_resource_path() + "shaders/dotted_outline.frag", ShaderType::FRAGMENT);

        instance->_outline_left_to_right = new Shape();
        instance->_outline_top_to_bottom = new Shape();
        instance->_outline_right_to_left = new Shape();
        instance->_outline_bottom_to_top = new Shape();

        instance->_outline_outline = new Shape();

        instance->reformat();
        instance->reschedule_render_tasks();
    }

    void Canvas::SelectionLayer::on_resize(GLArea* area, int w, int h, SelectionLayer* instance)
    {
        *(instance->_canvas_size) = {w, h};
        instance->reformat();
        area->queue_render();
    }

    void Canvas::SelectionLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::SelectionLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x, offset.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::SelectionLayer::on_selection_changed()
    {
        reformat();
        _area.queue_render();
    }

    void Canvas::SelectionLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float canvas_width = layer_resolution.x / _canvas_size->x;
        float canvas_height = layer_resolution.y / _canvas_size->y;

        canvas_width *= _scale;
        canvas_height *= _scale;

        Vector2f canvas_center = {0.5, 0.5};
        canvas_center += _offset;

        Vector2f top_left = canvas_center - Vector2f{0.5 * canvas_width, 0.5 * canvas_height};
        float pixel_w = canvas_width / layer_resolution.x;
        float pixel_h = canvas_height / layer_resolution.y;

        float x_eps = 1.f / _canvas_size->x;
        float y_eps = 1.f / _canvas_size->y;

        const auto& outline_vertices = active_state->get_selection().get_outline_vertices();
        std::vector<std::pair<Vector2f, Vector2f>> outline_outline;

        auto convert_vertices = [&](Shape* shape, const std::vector<std::pair<Vector2f, Vector2f>>& vertices){

            std::vector<std::pair<Vector2f, Vector2f>> converted;
            converted.reserve(vertices.size());

            for (const auto& pair : vertices)
            {
                auto to_push = std::pair<Vector2f, Vector2f>();
                to_push.first = {
                    top_left.x + pair.first.x * pixel_w,
                    top_left.y + pair.first.y * pixel_h
                };
                to_push.second = {
                    top_left.x + pair.second.x * pixel_w,
                    top_left.y + pair.second.y * pixel_h
                };
                converted.push_back(to_push);

                if (to_push.first.y == to_push.second.y)
                {
                    outline_outline.push_back({
                        to_push.first + Vector2f(0, -y_eps),
                        to_push.second + Vector2f(0, -y_eps)
                    });

                    outline_outline.push_back({
                        to_push.first + Vector2f(0, +y_eps),
                        to_push.second + Vector2f(0, +y_eps)
                    });
                }
                else
                {
                    outline_outline.push_back({
                        to_push.first + Vector2f(-x_eps, 0),
                        to_push.second + Vector2f(-x_eps, 0)
                    });

                    outline_outline.push_back({
                        to_push.first + Vector2f(+x_eps, 0),
                        to_push.second + Vector2f(+x_eps, 0)
                    });
                }
            }

            shape->as_lines(converted);
        };

        convert_vertices(_outline_left_to_right, outline_vertices.left_to_right);
        convert_vertices(_outline_top_to_bottom, outline_vertices.top_to_bottom);
        convert_vertices(_outline_right_to_left, outline_vertices.right_to_left);
        convert_vertices(_outline_bottom_to_top, outline_vertices.bottom_to_top);
        _outline_outline->as_lines(outline_outline);
        _outline_outline->set_color(RGBA(0, 0, 0, 0.5));
        reschedule_render_tasks();
    }

    void Canvas::SelectionLayer::set_animation_paused(bool b)
    {
        *_animation_paused = b ? 1 : 0;
        _area.queue_render();
    }

    void Canvas::SelectionLayer::set_color(HSVA color)
    {
        _color = color;

        if (not _area.get_is_realized())
            return;

        _outline_left_to_right->set_color(color);
        _outline_top_to_bottom->set_color(color);
        _outline_right_to_left->set_color(color);
        _outline_bottom_to_top->set_color(color);
    }
}