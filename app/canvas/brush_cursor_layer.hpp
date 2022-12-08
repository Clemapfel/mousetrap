// 
// Copyright 2022 Clemens Cords
// Created on 11/4/22 by clem (mail@clemens-cords.com)
//

#include <app/bubble_log_area.hpp>

namespace mousetrap
{
    Canvas::BrushCursorLayer::BrushCursorLayer(Canvas* owner)
            : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _area.add_tick_callback([](FrameClock clock, BrushCursorLayer* instance) -> bool {
            *instance->_cursor_outline_time_s += clock.get_time_since_last_frame().as_seconds();
            instance->_area.queue_render();
            return true;
        }, this);

        _brush_opacity = state::brush_opacity;
        _brush_color = state::primary_color;
        _current_brush = state::current_brush;

        auto* recompile_shader_action = new Action("canvas.recompile_brush_outline_shader");
        recompile_shader_action->set_do_function([](BrushCursorLayer* instance){
            ((BubbleLogArea*) state::bubble_log)->send_message("recompiling...");
            instance->_cursor_outline_shader->create_from_file(get_resource_path() + "shaders/brush_outline.frag", ShaderType::FRAGMENT);
        }, this);
        recompile_shader_action->add_shortcut("<Control>space");
        state::app->add_action(*recompile_shader_action);

        auto* shortcut_controller = new ShortcutController(state::app);
        shortcut_controller->add_action(recompile_shader_action->get_id());
        state::main_window->add_controller(shortcut_controller);
    }

    Canvas::BrushCursorLayer::operator Widget*()
    {
        return &_area;
    }

    Canvas::BrushCursorLayer::~BrushCursorLayer()
    {
        delete _cursor_shape;
        delete _cursor_outline_shape_top;
        delete _cursor_outline_shape_right;
        delete _cursor_outline_shape_bottom;
        delete _cursor_outline_shape_left;
    }

    void Canvas::BrushCursorLayer::reschedule_render_tasks()
    {
        _area.clear_render_tasks();
        _area.add_render_task(_cursor_shape);

        {
            auto task = RenderTask(_cursor_outline_shape_top, _cursor_outline_shader);
            task.register_int("_direction", _cursor_outline_shader_top_flag);
            task.register_float("_time_s", _cursor_outline_time_s);
            _area.add_render_task(task);
        }

        {
            auto task = RenderTask(_cursor_outline_shape_right, _cursor_outline_shader);
            task.register_int("_direction", _cursor_outline_shader_right_flag);
            task.register_float("_time_s", _cursor_outline_time_s);
            _area.add_render_task(task);
        }

        {
            auto task = RenderTask(_cursor_outline_shape_bottom, _cursor_outline_shader);
            task.register_int("_direction", _cursor_outline_shader_bottom_flag);
            task.register_float("_time_s", _cursor_outline_time_s);
            _area.add_render_task(task);
        }

        {
            auto task = RenderTask(_cursor_outline_shape_left, _cursor_outline_shader);
            task.register_int("_direction", _cursor_outline_shader_left_flag);
            task.register_float("_time_s", _cursor_outline_time_s);
            _area.add_render_task(task);
        }
    }

    void Canvas::BrushCursorLayer::on_realize(Widget* widget, BrushCursorLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_cursor_outline_shader = new Shader();
        instance->_cursor_outline_shader->create_from_file(get_resource_path() + "shaders/brush_outline.frag", ShaderType::FRAGMENT);

        instance->_cursor_shape = new Shape();
        instance->_cursor_outline_shape_top = new Shape();
        instance->_cursor_outline_shape_right = new Shape();
        instance->_cursor_outline_shape_bottom = new Shape();
        instance->_cursor_outline_shape_left = new Shape();

        instance->reformat();
        instance->reschedule_render_tasks();

        area->queue_render();
    }

    void Canvas::BrushCursorLayer::on_resize(GLArea* area, int w, int h, BrushCursorLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->reformat();
        area->queue_render();
    }

    void Canvas::BrushCursorLayer::reformat()
    {
        float width = state::layer_resolution.x / _canvas_size.x;
        float height = state::layer_resolution.y / _canvas_size.y;

        float pixel_w = width / state::layer_resolution.x * *_owner->_transform_scale;
        float pixel_h = height / state::layer_resolution.y * *_owner->_transform_scale;

        if (_cursor_shape == nullptr)
            return;

        if (_current_brush != nullptr)
        {
            pixel_w *= _current_brush->get_texture()->get_size().x;
            pixel_h *= _current_brush->get_texture()->get_size().y;
        }

        _cursor_shape->as_rectangle(
            {0, 0},
            {pixel_w, 0},
            {pixel_w, pixel_h},
            {0, pixel_h}
        );
        _cursor_shape->set_texture(state::current_brush->get_texture());

        auto texture_size = state::current_brush->get_texture()->get_size();
        auto adjusted_pixel_size = Vector2f(pixel_w / texture_size.x, pixel_h / texture_size.y);

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
            }

            return out;
        };

        auto vertices = state::current_brush->get_outline_vertices();

        _cursor_outline_shape_top->as_lines(convert_vertex_coordinates(vertices.top));
        _cursor_outline_shape_right->as_lines(convert_vertex_coordinates(vertices.right));
        _cursor_outline_shape_bottom->as_lines(convert_vertex_coordinates(vertices.bottom));
        _cursor_outline_shape_left->as_lines(convert_vertex_coordinates(vertices.left));

        _brush_color = state::primary_color;
        _brush_opacity = state::brush_opacity;

        auto cursor_color = HSVA(
            _brush_color.h,
            _brush_color.s,
            _brush_color.v,
            _brush_opacity
        );
        auto outline_color = HSVA(0, 0, cursor_color.v > 0.5 ? 0 : 1, _brush_opacity);

        if (state::active_tool == ERASER)
        {
            cursor_color = HSVA(0, 0, 1, 0.25);
            outline_color = HSVA(0, 0, 0, 1);
        }

        _cursor_shape->set_color(cursor_color);
        _cursor_shape->set_visible(_cursor_in_bounds);

        reschedule_render_tasks();
    }

    void Canvas::BrushCursorLayer::update()
    {
        if (_current_brush != state::current_brush)
        {
            _current_brush = state::current_brush;
            _cursor_in_bounds = *_owner->_cursor_in_bounds;
            _cursor_position = *_owner->_current_cursor_position;
            _brush_color = state::primary_color;
            _brush_opacity = state::brush_opacity;

            reformat();
        }

        if (_cursor_position != *_owner->_current_cursor_position and _area.get_is_realized())
        {
            _cursor_position = *_owner->_current_cursor_position;

            float widget_w = _area.get_size().x;
            float widget_h = _area.get_size().y;

            Vector2f pos = {_cursor_position.x / widget_w, _cursor_position.y / widget_h};

            // align with texture-space pixel grid
            float w = state::layer_resolution.x / _canvas_size.x;
            float h = state::layer_resolution.y / _canvas_size.y;

            Vector2f layer_top_left = {0.5 - w / 2, 0.5 - h / 2};
            layer_top_left = to_gl_position(layer_top_left);
            layer_top_left = _owner->_transform->apply_to(layer_top_left);
            layer_top_left = from_gl_position(layer_top_left);

            Vector2f layer_size = {
                state::layer_resolution.x / _canvas_size.x,
                state::layer_resolution.y / _canvas_size.y
            };

            layer_size *= *_owner->_transform_scale;

            float x_dist = (pos.x - layer_top_left.x);
            float y_dist = (pos.y - layer_top_left.y);

            Vector2f pixel_size = {layer_size.x / state::layer_resolution.x, layer_size.y / state::layer_resolution.y};

            x_dist /= pixel_size.x;
            y_dist /= pixel_size.y;

            x_dist = floor(x_dist);
            y_dist = floor(y_dist);

            pos.x = layer_top_left.x + x_dist * pixel_size.x;
            pos.y = layer_top_left.y + y_dist * pixel_size.y;

            // align with widget-space pixel grid

            pos *= _area.get_size();
            pos.x = round(pos.x);
            pos.y = round(pos.y);
            pos /= _area.get_size();

            float x_offset = 0.5;
            float y_offset = 0.5;

            auto* brush_texture = state::current_brush->get_texture();

            if (brush_texture and brush_texture->get_size().x % 2 == 0)
                x_offset = 1;

            if (brush_texture and brush_texture->get_size().y % 2 == 0)
                y_offset = 1;

            Vector2f centroid = {
                pos.x + pixel_size.x * x_offset,
                pos.y + pixel_size.y * y_offset
            };

            auto top_offset = _cursor_shape->get_centroid() - _cursor_outline_shape_top->get_centroid();
            auto right_offset = _cursor_shape->get_centroid() - _cursor_outline_shape_right->get_centroid();
            auto bottom_offset = _cursor_shape->get_centroid() - _cursor_outline_shape_bottom->get_centroid();
            auto left_offset = _cursor_shape->get_centroid() - _cursor_outline_shape_left->get_centroid();

            _cursor_shape->set_centroid(centroid);
            _cursor_outline_shape_top->set_centroid(centroid - top_offset);
            _cursor_outline_shape_right->set_centroid(centroid - right_offset);
            _cursor_outline_shape_bottom->set_centroid(centroid - bottom_offset);
            _cursor_outline_shape_left->set_centroid(centroid - left_offset);
        }

        auto should_update_opacity = _brush_opacity != state::brush_opacity;
        auto should_update_color = _brush_color != state::primary_color;

        if ((should_update_opacity or should_update_color) and _area.get_is_realized())
        {
            _brush_color = state::primary_color;
            _brush_opacity = state::brush_opacity;

            auto cursor_color = HSVA(
                _brush_color.h,
                _brush_color.s,
                _brush_color.v,
                _brush_opacity
            );
            auto outline_color = HSVA(0, 0, cursor_color.v > 0.5 ? 0 : 1, _brush_opacity);

            if (state::active_tool == ERASER)
            {
                cursor_color = HSVA(0, 0, 1, 0.25);
                outline_color = HSVA(0, 0, 0, 1);
            }

            _cursor_shape->set_color(cursor_color);
        }

        if (_cursor_in_bounds != *_owner->_cursor_in_bounds and _area.get_is_realized())
        {
            _cursor_in_bounds = *_owner->_cursor_in_bounds;

            _cursor_shape->set_visible(_cursor_in_bounds);
        }

        _area.queue_render();
    }
}