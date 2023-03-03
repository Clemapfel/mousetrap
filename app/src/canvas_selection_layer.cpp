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
            auto& task = tasks.emplace_back(_outline_top, _outline_shader);
            task.register_int("_direction", _outline_shader_top_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_right, _outline_shader);
            task.register_int("_direction", _outline_shader_right_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_bottom, _outline_shader);
            task.register_int("_direction", _outline_shader_bottom_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_left, _outline_shader);
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

        instance->_outline_top = new Shape();
        instance->_outline_right = new Shape();
        instance->_outline_bottom = new Shape();
        instance->_outline_left = new Shape();

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

        std::vector<std::pair<Vector2f, Vector2f>> outline_outline;
        auto convert_vertex_coordinates = [&](const std::vector<std::pair<Vector2i, Vector2i>>& in) {

            std::vector<std::pair<Vector2f, Vector2f>> out;

            for (size_t i = 0; i < in.size(); ++i)
            {
                auto v = in.at(i);
                auto to_push = std::pair<Vector2f, Vector2f>{
                {
                    top_left.x + v.first.x * pixel_w,
                    top_left.y + v.first.y * pixel_h
                    },
                    {
                        top_left.x + v.second.x * pixel_w,
                        top_left.y + v.second.y * pixel_h
                    }
                };

                out.push_back(to_push);

                auto a = to_push.first;
                auto b = to_push.second;

                if (a.x == b.x)
                {
                    Vector2f top = a.y < b.y ? a : b;
                    Vector2f bottom = a.y < b.y ? b : a;

                    outline_outline.emplace_back(
                        Vector2f{a.x - x_eps, top.y},
                        Vector2f{a.x - x_eps, bottom.y}
                    );

                    outline_outline.emplace_back(
                        Vector2f{a.x + x_eps, top.y},
                        Vector2f{a.x + x_eps, bottom.y}
                    );
                }
                else if (a.y == b.y)
                {
                    Vector2f left = a.x < b.x ? a : b;
                    Vector2f right = a.x < b.x ? b : a;

                    outline_outline.emplace_back(
                        Vector2f{left.x, a.y - y_eps},
                        Vector2f{right.x, a.y - y_eps}
                    );

                    outline_outline.emplace_back(
                        Vector2f{left.x, a.y + y_eps},
                        Vector2f{right.x, a.y + y_eps}
                    );
                }
            }

            return out;
        };

        auto selection = active_state->get_selection();
        auto vertices = generate_outline_vertices(selection);

        _outline_top->as_lines(convert_vertex_coordinates(vertices.top));
        _outline_right->as_lines(convert_vertex_coordinates(vertices.right));
        _outline_bottom->as_lines(convert_vertex_coordinates(vertices.bottom));
        _outline_left->as_lines(convert_vertex_coordinates(vertices.left));

        for (auto* shape : {
            _outline_top,
            _outline_right,
            _outline_bottom,
            _outline_left
        })
            shape->set_color(_color);

        _outline_outline->as_lines(outline_outline);
        _outline_outline->set_color(RGBA(0, 0, 0, 0.5));

        _outline_top_initial_position = _outline_top->get_top_left();
        _outline_right_initial_position = _outline_right->get_top_left();
        _outline_bottom_initial_position = _outline_bottom->get_top_left();
        _outline_left_initial_position = _outline_left->get_top_left();
        _outline_outline_initial_position = _outline_outline->get_top_left();

        reschedule_render_tasks();
        _area.queue_render();
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

        _outline_top->set_color(color);
        _outline_right->set_color(color);
        _outline_bottom->set_color(color);
        _outline_left->set_color(color);
    }
}