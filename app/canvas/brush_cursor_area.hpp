// 
// Copyright 2022 Clemens Cords
// Created on 11/4/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::BrushCursorLayer::BrushCursorLayer(Canvas* owner)
            : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        _motion_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_controller.connect_signal_motion(on_motion, this);
        _motion_controller.connect_signal_motion_leave(on_motion_leave, this);
        _area.add_controller(&_motion_controller);

        _area.add_tick_callback([](FrameClock clock, BrushCursorLayer* instance) -> bool {
            *instance->_timer = clock.get_frame_time().as_seconds();
            instance->_area.queue_render();
            return true;
        }, this);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _click_controller.connect_signal_click_released(on_click_released, this);
        _area.add_controller(&_click_controller);

        _overlay.set_child(&_area);
    }

    Canvas::BrushCursorLayer::operator Widget*()
    {
        return &_overlay;
    }

    Canvas::BrushCursorLayer::~BrushCursorLayer()
    {
        delete _cursor_shape;
        delete _brush_outline_shader;
        delete _cursor_outline_shape_hlines;
        delete _cursor_outline_shape_vlines;
        delete _canvas_size;
        delete _cursor_position;
    }

    void Canvas::BrushCursorLayer::on_realize(Widget* widget, BrushCursorLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_cursor_shape = new Shape();
        instance->_cursor_outline_shape_hlines = new Shape();
        instance->_cursor_outline_shape_vlines = new Shape();
        instance->_brush_outline_shader = new Shader();
        instance->_brush_outline_shader->create_from_file(get_resource_path() + "shaders/brush_outline.frag", ShaderType::FRAGMENT);
        instance->update();

        area->queue_render();
    }

    void Canvas::BrushCursorLayer::update()
    {
        float width = state::layer_resolution.x / _canvas_size->x;
        float height = state::layer_resolution.y / _canvas_size->y;

        float pixel_w = width / state::layer_resolution.x * _owner->_transform_scale;
        float pixel_h = height / state::layer_resolution.y * _owner->_transform_scale;

        if (_cursor_shape == nullptr)
            return;

        if (state::current_brush != nullptr)
        {
            pixel_w *= state::current_brush->get_texture()->get_size().x;
            pixel_h *= state::current_brush->get_texture()->get_size().y;
        }

        _cursor_shape->as_rectangle(
             {0, 0},
             {pixel_w, 0},
             {pixel_w, pixel_h},
             {0, pixel_h}
         );

        _cursor_shape->set_texture(state::current_brush->get_texture());

        auto cursor_color = HSVA(
            state::primary_color.h,
            state::primary_color.s,
            state::primary_color.v,
            state::brush_opacity
        );
        auto outline_color = HSVA(0, 0, cursor_color.v > 0.5 ? 0 : 1, brush_cursor_color_alpha);

        if (state::active_tool == ERASER)
        {
            cursor_color = HSVA(0, 0, 1, 0.25);
            outline_color = HSVA(0, 0, 0, 1);
        }

        _cursor_shape->set_color(cursor_color);

        auto vertices = state::current_brush->get_outline_vertices();
        auto texture_size = state::current_brush->get_texture()->get_size();

        std::vector<std::pair<Vector2f, Vector2f>> outline_vertices_h;
        std::vector<std::pair<Vector2f, Vector2f>> outline_vertices_v;

        auto adjusted_pixel_size = Vector2f(pixel_w / texture_size.x, pixel_h / texture_size.y);
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            auto v = vertices.at(i);
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

            if (v.first.x < v.second.x)
                outline_vertices_h.push_back(to_push);
            else
                outline_vertices_v.push_back(to_push);
        }

        _cursor_outline_shape_hlines->as_lines(outline_vertices_h);
        _cursor_outline_shape_vlines->as_lines(outline_vertices_v);


        _cursor_outline_shape_hlines->set_color(outline_color);
        _cursor_outline_shape_vlines->set_color(outline_color);

        _area.clear_render_tasks();
        _area.add_render_task(_cursor_shape);

        auto outline_task_h = RenderTask(_cursor_outline_shape_hlines);//, _brush_outline_shader);
        //outline_task_h.register_int("_horizontal", new int(1));
        //outline_task_h.register_float("_time_s", _timer);
        _area.add_render_task(outline_task_h);

        auto outline_task_v = RenderTask(_cursor_outline_shape_vlines);//, _brush_outline_shader);
        //outline_task_v.register_int("_horizontal", new int(0));
        //outline_task_v.register_float("_time_s", _timer);
        _area.add_render_task(outline_task_v);

        // align with previous position
        on_motion(&_motion_controller, _cursor_position->x, _cursor_position->y, this);

        _area.queue_render();
    }

    void Canvas::BrushCursorLayer::on_resize(GLArea* area, int w, int h, BrushCursorLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->update();
        area->queue_render();
    }

    void Canvas::BrushCursorLayer::on_motion(MotionEventController*, double x, double y, BrushCursorLayer* instance)
    {
        *instance->_cursor_position = {x, y};

        float widget_w = instance->_area.get_size().x;
        float widget_h = instance->_area.get_size().y;

        Vector2f pos = {x / widget_w, y / widget_h};

        // align with texture-space pixel grid
        float w = state::layer_resolution.x / instance->_canvas_size->x;
        float h = state::layer_resolution.y / instance->_canvas_size->y;

        Vector2f layer_top_left = {0.5 - w / 2, 0.5 - h / 2};
        layer_top_left = to_gl_position(layer_top_left);
        layer_top_left = instance->_owner->_transform->apply_to(layer_top_left);
        layer_top_left = from_gl_position(layer_top_left);

        Vector2f layer_size = {
            state::layer_resolution.x / instance->_canvas_size->x,
            state::layer_resolution.y / instance->_canvas_size->y
        };

        layer_size *= instance->_owner->_transform_scale;

        float x_dist = (pos.x - layer_top_left.x);
        float y_dist = (pos.y - layer_top_left.y);

        Vector2f pixel_size = {layer_size.x / state::layer_resolution.x, layer_size.y / state::layer_resolution.y};

        x_dist /= pixel_size.x;
        y_dist /= pixel_size.y;

        x_dist = floor(x_dist);
        y_dist = floor(y_dist);

        auto previous_pixel_pos = instance->_owner->get_current_pixel_position();
        auto current_pixel_pos = Vector2i{x_dist, y_dist};
        instance->_owner->set_current_pixel_position(current_pixel_pos.x, current_pixel_pos.y);

        pos.x = layer_top_left.x + x_dist * pixel_size.x;
        pos.y = layer_top_left.y + y_dist * pixel_size.y;

        // align with widget-space pixel grid
        pos *= instance->_area.get_size();
        pos.x = round(pos.x);
        pos.y = round(pos.y);
        pos /= instance->_area.get_size();

        if (instance->_cursor_shape != nullptr)
        {
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

            auto h_offset = instance->_cursor_shape->get_centroid() - instance->_cursor_outline_shape_hlines->get_centroid();
            auto v_offset = instance->_cursor_shape->get_centroid() - instance->_cursor_outline_shape_vlines->get_centroid();

            instance->_cursor_shape->set_centroid(centroid);
            instance->_cursor_outline_shape_hlines->set_centroid(centroid - h_offset);
            instance->_cursor_outline_shape_vlines->set_centroid(centroid - v_offset);
        }

        if (state::active_tool == BRUSH and instance->_click_active)
            instance->_owner->draw_brush_line(previous_pixel_pos, current_pixel_pos, state::current_brush, state::primary_color);

        instance->_area.queue_render();
    }

    void Canvas::BrushCursorLayer::on_motion_enter(MotionEventController*, double, double, BrushCursorLayer* instance)
    {
        instance->_cursor_shape->set_visible(true);
        instance->_cursor_outline_shape_hlines->set_visible(true);
        instance->_cursor_outline_shape_vlines->set_visible(true);
        instance->_area.queue_render();
    }

    void Canvas::BrushCursorLayer::on_motion_leave(MotionEventController*, BrushCursorLayer* instance)
    {
        instance->_cursor_shape->set_visible(false);
        instance->_cursor_outline_shape_hlines->set_visible(false);
        instance->_cursor_outline_shape_vlines->set_visible(false);
        instance->_area.queue_render();
    }

    void Canvas::BrushCursorLayer::on_click_pressed(ClickEventController*, size_t n, double x, double y,
                                                    BrushCursorLayer* instance)
    {
        instance->_click_active = true;
        auto pos = instance->_owner->widget_to_texture_coord(Vector2f{x, y}, *instance->_canvas_size);
        instance->_owner->set_current_pixel_position(pos.x, pos.y);

        if (state::active_tool == BRUSH)
            instance->_owner->draw_brush({pos.x, pos.y}, state::current_brush, state::primary_color);
    }

    void Canvas::BrushCursorLayer::on_click_released(ClickEventController*, size_t n, double x, double y, BrushCursorLayer* instance)
    {
        instance->_click_active = false;
    }
}