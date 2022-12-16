// 
// Copyright 2022 Clemens Cords
// Created on 12/10/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::SelectionLayer::SelectionLayer(Canvas* owner)
        : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _area.add_tick_callback([](FrameClock clock, SelectionLayer* instance) -> bool {

            if (state::settings_file->get_value_as<bool>("canvas", "selection_outline_animated"))
                *instance->_outline_time_s += clock.get_time_since_last_frame().as_seconds();

            instance->_area.queue_render();
            return true;
        }, this);
    }
    
    Canvas::SelectionLayer::operator Widget*()
    {
        return &_area;
    }
    
    void Canvas::SelectionLayer::reschedule_render_tasks()
    {
        _area.clear_render_tasks();

        auto outline_task = RenderTask(_outline_outline_shape, nullptr, nullptr, BlendMode::REVERSE_SUBTRACT);
        _area.add_render_task(outline_task);

        auto tasks = std::vector<RenderTask>();

        {
            auto& task = tasks.emplace_back(_outline_shape_top, _outline_shader);
            task.register_int("_direction", _outline_shader_top_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_shape_right, _outline_shader);
            task.register_int("_direction", _outline_shader_right_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_shape_bottom, _outline_shader);
            task.register_int("_direction", _outline_shader_bottom_flag);
        }

        {
            auto& task = tasks.emplace_back(_outline_shape_left, _outline_shader);
            task.register_int("_direction", _outline_shader_left_flag);
        }

        for (auto& task : tasks)
        {
            task.register_float("_time_s", _outline_time_s);
            task.register_vec2("_canvas_size", _canvas_size);
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

        instance->_outline_shape_top = new Shape();
        instance->_outline_shape_right = new Shape();
        instance->_outline_shape_bottom = new Shape();
        instance->_outline_shape_left = new Shape();

        instance->_outline_outline_shape = new Shape();

        instance->reformat();
        instance->reschedule_render_tasks();
    }

    void Canvas::SelectionLayer::on_resize(GLArea* area, int w, int h, SelectionLayer* instance)
    {
        *(instance->_canvas_size) = {w, h};
        instance->reformat();
        area->queue_render();
    }

    void Canvas::SelectionLayer::reformat()
    {
        float width = state::layer_resolution.x / _canvas_size->x;
        float height = state::layer_resolution.y / _canvas_size->y;

        float pixel_w = width / state::layer_resolution.x * *_owner->_transform_scale;
        float pixel_h = height / state::layer_resolution.y * *_owner->_transform_scale;

        if (not _area.get_is_realized())
            return;

        auto texture_size = state::current_brush->get_texture()->get_size();
        auto adjusted_pixel_size = Vector2f(pixel_w / texture_size.x, pixel_h / texture_size.y);

        std::vector<std::pair<Vector2f, Vector2f>> outline_outline;
        float pixel_offset_x = 1.f / _area.get_size().x;
        float pixel_offset_y = 1.f / _area.get_size().y;

        auto convert_vertex_coordinates = [&](const std::vector<std::pair<Vector2i, Vector2i>>& in) {

            std::vector<std::pair<Vector2f, Vector2f>> out;

            for (size_t i = 0; i < in.size(); ++i)
            {
                auto v = in.at(i);
                auto to_push = std::pair<Vector2f, Vector2f>{
                    {
                        v.first.x * adjusted_pixel_size.x,
                        v.first.y * adjusted_pixel_size.y
                    },
                    {
                        v.second.x * adjusted_pixel_size.x,
                        v.second.y * adjusted_pixel_size.y
                    }
                };

                out.push_back(to_push);

                auto a = to_push.first;
                auto b = to_push.second;

                if (a.x == b.x)
                {
                    Vector2f top = a.y < b.y ? a : b;
                    Vector2f bottom = a.y < b.y ? b : a;

                    outline_outline.push_back({
                          {top.x - pixel_offset_x, top.y},
                          {bottom.x - pixel_offset_x, bottom.y}
                    });

                    outline_outline.push_back({
                          {top.x + pixel_offset_x, top.y},
                          {bottom.x + pixel_offset_x, bottom.y}
                  });
                }
                else if (a.y == b.y)
                {
                    Vector2f left = a.x < b.x ? a : b;
                    Vector2f right = a.x < b.x ? b : a;

                    outline_outline.push_back({
                          {left.x, left.y - pixel_offset_y},
                          {right.x, right.y - pixel_offset_y}
                    });

                    outline_outline.push_back({
                          {left.x, left.y + pixel_offset_y},
                          {right.x, right.y + pixel_offset_y}
                      });
                }
            }

            return out;
        };

        auto vertices = generate_outline_vertices(state::selection);

        _outline_shape_top->as_lines(convert_vertex_coordinates(vertices.top));
        _outline_shape_right->as_lines(convert_vertex_coordinates(vertices.right));
        _outline_shape_bottom->as_lines(convert_vertex_coordinates(vertices.bottom));
        _outline_shape_left->as_lines(convert_vertex_coordinates(vertices.left));

        _outline_outline_shape->as_lines(outline_outline);
        _outline_outline_shape->set_color(RGBA(0, 0, 0, 1));

        _outline_shape_top_initial_position = _outline_shape_top->get_top_left();
        _outline_shape_right_initial_position = _outline_shape_right->get_top_left();
        _outline_shape_bottom_initial_position = _outline_shape_bottom->get_top_left();
        _outline_shape_left_initial_position = _outline_shape_left->get_top_left();
        _outline_outline_shape_initial_position = _outline_outline_shape->get_top_left();

        reschedule_render_tasks();
        _area.queue_render();
    }

    void Canvas::SelectionLayer::update()
    {
        if (not _area.get_is_realized())
            return;

        auto selection_top_left = *(state::selection.begin());

        float layer_w = state::layer_resolution.x / _canvas_size->x;
        float layer_h = state::layer_resolution.y / _canvas_size->y;

        Vector2f layer_top_left = {0.5 - layer_w / 2, 0.5 - layer_h / 2};
        layer_top_left = to_gl_position(layer_top_left);
        layer_top_left = _owner->_transform->apply_to(layer_top_left);
        layer_top_left = from_gl_position(layer_top_left);

        Vector2f pixel_size = {
            layer_w / state::layer_resolution.x,
            layer_h / state::layer_resolution.y
        };

        auto top_left = layer_top_left;
        top_left.x += selection_top_left.x * pixel_size.x;
        top_left.y += selection_top_left.y * pixel_size.y;

        /*
        _outline_shape_top->set_top_left(top_left + _outline_shape_top_initial_position);
        _outline_shape_right->set_top_left(top_left + _outline_shape_right_initial_position);
        _outline_shape_bottom->set_top_left(top_left + _outline_shape_bottom_initial_position);
        _outline_shape_left->set_top_left(top_left + _outline_shape_left_initial_position);
        _outline_outline_shape->set_top_left(top_left + _outline_outline_shape_initial_position);
        */

        _area.queue_render();
    }
}