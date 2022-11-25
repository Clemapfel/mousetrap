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

        _overlay.set_child(&_area);

        auto* button = new Button();
        button->connect_signal_clicked([](Button*, BrushCursorLayer* instance){

            std::cout << "recompiling..." << std::endl;
            if (instance->_brush_texture_shader != nullptr)
                instance->_brush_texture_shader->create_from_file(get_resource_path() + "shaders/brush_texture_outline.frag", ShaderType::FRAGMENT);
        }, this);
        button->set_align(GTK_ALIGN_END);
        _overlay.add_overlay(button);
    }

    Canvas::BrushCursorLayer::operator Widget*()
    {
        return &_overlay;
    }

    Canvas::BrushCursorLayer::~BrushCursorLayer()
    {
        delete _cursor_shape;
        delete _brush_texture_shader;
        delete _canvas_size;
        delete _cursor_position;
    }

    void Canvas::BrushCursorLayer::on_realize(Widget* widget, BrushCursorLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_cursor_shape = new Shape();
        instance->_cursor_outline_shape = new Shape();
        instance->_brush_texture_shader = new Shader();
        instance->_brush_texture_shader->create_from_file(get_resource_path() + "shaders/brush_texture_outline.frag", ShaderType::FRAGMENT);
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
        _cursor_shape->set_color(HSVA(
            state::primary_color.h,
            state::primary_color.s,
            state::primary_color.v,
            state::brush_opacity
        ));

        auto vertices = state::current_brush->get_outline_vertices();
        auto texture_size = state::current_brush->get_texture()->get_size();

        std::vector<Vector2f> outline_vertices;
        auto adjusted_pixel_size = Vector2f(pixel_w / texture_size.x, pixel_h / texture_size.y);
        for (size_t i = 0; i < vertices.size(); ++i)
            outline_vertices.push_back({
                vertices.at(i).x * adjusted_pixel_size.x,
                vertices.at(i).y * adjusted_pixel_size.y
            });

        _cursor_outline_shape->as_point_cloud(outline_vertices);
        _cursor_outline_shape->set_color(RGBA(0, 1, 0, 1));

        _area.clear_render_tasks();
        _area.add_render_task(_cursor_shape, _brush_texture_shader);
        _area.add_render_task(_cursor_outline_shape);
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

        instance->_owner->set_current_pixel_position(x_dist, y_dist);

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
            instance->_cursor_shape->set_centroid(centroid);
            instance->_cursor_outline_shape->set_centroid(centroid);
        }

        instance->_area.queue_render();
    }

    void Canvas::BrushCursorLayer::on_motion_enter(MotionEventController*, double, double, BrushCursorLayer* instance)
    {
        instance->_cursor_shape->set_visible(true);
        instance->_area.queue_render();
    }

    void Canvas::BrushCursorLayer::on_motion_leave(MotionEventController*, BrushCursorLayer* instance)
    {
        instance->_cursor_shape->set_visible(false);
        instance->_area.queue_render();
    }
}