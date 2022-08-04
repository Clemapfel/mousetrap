// 
// Copyright 2022 Clemens Cords
// Created on 8/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/get_resource_path.hpp>

#include <include/components/global_state.hpp>

namespace mousetrap
{
    class HsvTriangleSelect;
    static inline HsvTriangleSelect* hsv_triangle_select = nullptr;

    struct HsvTriangleSelect : public Widget
    {
        public:
            HsvTriangleSelect(float width);
            ~HsvTriangleSelect();

            GtkWidget* get_native();

        //private:
            struct ShapeArea : public GLArea
            {
                ShapeArea();
                ~ShapeArea();

                void on_realize(GtkGLArea* area);
                void on_resize(GtkGLArea* area, int, int);

                void update();

                Shape* _hsv_triangle;
                Shape* _hsv_triangle_frame;

                Shape* _hue_triangle_right;
                Shape* _hue_triangle_right_frame;
                Shape* _hue_triangle_left;
                Shape* _hue_triangle_left_frame;

                Shape* _hue_bar;
                Shape* _hue_bar_frame;
                Shader* _hue_bar_shader;
                HSVA* _shader_color = new HSVA(1, 1, 1, 1);
                Vector2f* _shader_canvas_size = new Vector2f(1, 1);

                Shape* _hue_bar_cursor_frame_inner;
                Shape* _hue_bar_cursor;
                Shape* _hue_bar_cursor_frame_outer;

                Shape* _cursor;
                Shape* _cursor_frame;

                Vector2f one_px;
            };

            ShapeArea* _shape_area;
    };
}

// ###

namespace mousetrap
{
    GtkWidget* HsvTriangleSelect::get_native()
    {
        return _shape_area->get_native();
    }

    HsvTriangleSelect::ShapeArea::ShapeArea()
    {}

    HsvTriangleSelect::ShapeArea::~ShapeArea()
    {
        delete _hsv_triangle;
        delete _hsv_triangle_frame;

        delete _hue_triangle_left;
        delete _hue_triangle_left_frame;
        delete _hue_triangle_right;
        delete _hue_triangle_right_frame;

        delete _hue_bar;
        delete _hue_bar_frame;
        delete _hue_bar_shader;
        delete _shader_color;
        delete _shader_canvas_size;
        delete _hue_bar_cursor;
        delete _hue_bar_cursor_frame_outer;
        delete _hue_bar_cursor_frame_inner;

        delete _cursor;
        delete _cursor_frame;
    }

    void HsvTriangleSelect::ShapeArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        auto translate_by_angle = [](Vector2f center, float radius, float as_radians) -> Vector2f
        {
            return Vector2f(center.x + cos(as_radians) * radius,  center.y + sin(as_radians) * radius);
        };

        float hue_bar_x_margin = 0.05;
        float hue_bar_y_margin = hue_bar_x_margin;

        float hue_bar_height = 3 * hue_bar_x_margin;

        auto hsv_triangle_radius = 0.5f;
        auto hsv_triangle_center = Vector2f(0.5, 1 - hue_bar_y_margin - hue_bar_height - hsv_triangle_radius);

        float hsv_triangle_frame_radius_factor = 1.1;
        auto hsv_triangle_rotation_offset = degrees(270);

        {
            // creating circle shape and reading vertices is easier than calcing position
            auto temp = Shape();
            temp.as_circle(hsv_triangle_center, hsv_triangle_radius, 3);
            temp.rotate(hsv_triangle_rotation_offset);
            temp.set_centroid(hsv_triangle_center);

            _hsv_triangle = new Shape();
            _hsv_triangle->as_triangle(
                temp.get_vertex_position(1),    // sic, 0 is centroid
                temp.get_vertex_position(2),
                temp.get_vertex_position(3)
            );
        }

        _hsv_triangle_frame = new Shape();
        _hsv_triangle_frame->as_wireframe({
            _hsv_triangle->get_vertex_position(0),
            _hsv_triangle->get_vertex_position(1),
            _hsv_triangle->get_vertex_position(2)
        });
        _hsv_triangle_frame->set_color(RGBA(0, 0, 0, 1));

        Vector2f hue_bar_pos = {hue_bar_x_margin, 1 - hue_bar_y_margin - hue_bar_height};
        Vector2f hue_bar_size = {1 - 2*hue_bar_x_margin, hue_bar_height};

        _hue_bar = new Shape();
        _hue_bar->as_rectangle(hue_bar_pos, hue_bar_size);

        float frame_x_thickness = one_px.x;
        float frame_y_thickness = one_px.y;

        _hue_bar_frame = new Shape();
        _hue_bar_frame->as_frame(hue_bar_pos, hue_bar_size, frame_x_thickness, frame_y_thickness);
        _hue_bar_frame->set_color(RGBA(0, 0, 0, 1));

        _hue_bar_shader = new Shader();
        _hue_bar_shader->create_from_file(get_resource_path() + "shaders/hsv_triangle_hue_bar.frag", ShaderType::FRAGMENT);

        float hue_bar_cursor_height = hue_bar_height + 4 * one_px.y;
        float hue_bar_cursor_frame_width_base = std::max<float>(0.075 * hue_bar_height, one_px.x * 7);
        float cursor_width = std::max<float>(0.05, hue_bar_cursor_frame_width_base * 2 + one_px.x * 7);
        auto hue_bar_cursor_frame_width = Vector2f(hue_bar_cursor_frame_width_base, hue_bar_cursor_frame_width_base * (one_px.y / one_px.x));

        Vector2f hue_bar_cursor_size = {cursor_width, hue_bar_cursor_height};
        _hue_bar_cursor = new Shape();
        _hue_bar_cursor->as_frame({0, 0}, hue_bar_cursor_size, hue_bar_cursor_frame_width.x, hue_bar_cursor_frame_width.y);
        _hue_bar_cursor->set_color(RGBA(1, 1, 1, 1));

        _hue_bar_cursor_frame_inner = new Shape();
        _hue_bar_cursor_frame_inner->as_frame({0, 0}, hue_bar_cursor_size - hue_bar_cursor_frame_width * Vector2f(2), one_px.x, one_px.y);
        _hue_bar_cursor_frame_inner->set_color(RGBA(0, 0, 0, 1));

        _hue_bar_cursor_frame_outer = new Shape();
        _hue_bar_cursor_frame_outer->as_frame({0, 0}, hue_bar_cursor_size, one_px.x, one_px.y);
        _hue_bar_cursor_frame_outer->set_color(RGBA(0, 0, 0, 1));

        for (auto* shape : {_hue_bar_cursor_frame_inner, _hue_bar_cursor_frame_outer, _hue_bar_cursor})
            shape->set_centroid(Vector2f(0.5, _hue_bar->get_centroid().y));

        float hue_triangle_radius = hue_bar_height;
        Vector2f hue_triangle_right_centroid = {1 - hue_bar_x_margin - hue_triangle_radius, 1 - hue_bar_y_margin - hue_bar_height - 1.5*hue_bar_y_margin - (1 / sqrt(2)) * hue_triangle_radius};
        Vector2f hue_triangle_left_centroid = {1 - hue_triangle_right_centroid.x, hue_triangle_right_centroid.y};

        _hue_triangle_right = new Shape();
        _hue_triangle_right->as_circle(hue_triangle_right_centroid, hue_triangle_radius, 3);

        _hue_triangle_left = new Shape();
        _hue_triangle_left->as_circle(hue_triangle_right_centroid, hue_triangle_radius, 3);

        auto hue_triangle_rotation_offset = degrees(90);

        for (auto* shape : {_hue_triangle_left, _hue_triangle_right})
            shape->rotate(hue_triangle_rotation_offset);

        _hue_triangle_right->set_centroid(hue_triangle_right_centroid);
        _hue_triangle_left->set_centroid(hue_triangle_left_centroid);

        _hue_triangle_left_frame = new Shape();
        _hue_triangle_left_frame->as_wireframe({
            _hue_triangle_left->get_vertex_position(1),
            _hue_triangle_left->get_vertex_position(2),
            _hue_triangle_left->get_vertex_position(3),
        });
        _hue_triangle_left_frame->set_color(RGBA(0, 0, 0, 1));

        _hue_triangle_right_frame = new Shape();
        _hue_triangle_right_frame->as_wireframe({
           _hue_triangle_right->get_vertex_position(1),
           _hue_triangle_right->get_vertex_position(2),
           _hue_triangle_right->get_vertex_position(3),
        });
        _hue_triangle_right_frame->set_color(RGBA(0, 0, 0, 1));


        float cursor_radius = std::max<float>(5 * one_px.x, 0.1 * hue_bar_height);
        _cursor = new Shape();
        _cursor->as_circle({0, 0}, cursor_radius, 8);

        _cursor_frame = new Shape();
        _cursor_frame->as_circle({0, 0}, cursor_radius + one_px.x, 8);
        _cursor_frame->set_color(RGBA(0, 0, 0, 1));

        /*
        float cross_length = 0.5 * hue_bar_height;
        float cross_height = 0.1 * hue_bar_height;

        _cursor_hcross = new Shape();
        _cursor_hcross->as_rectangle({0, 0}, {cross_length, cross_height});

        _cursor_hcross_frame = new Shape();
        _cursor_hcross_frame->as_frame({0, 0}, {cross_length, cross_height}, one_px.x, one_px.y);
        _cursor_hcross_frame->set_color(RGBA(0, 0, 0, 1));

        _cursor_vcross = new Shape();
        _cursor_vcross->as_rectangle({0, 0}, {cross_height, cross_length});

        _cursor_vcross_frame = new Shape();
        _cursor_vcross_frame->as_frame({0, 0}, {cross_height, cross_length}, one_px.x, one_px.y);
        _cursor_vcross_frame->set_color(RGBA(0, 0, 0, 1));

        for (auto* shape : {_cursor_hcross, _cursor_hcross_frame, _cursor_vcross, _cursor_vcross_frame})
            shape->set_centroid(hsv_triangle_center);
            */

        add_render_task(_hsv_triangle);
        add_render_task(_hsv_triangle_frame);

        auto hue_bar_render_task = RenderTask(_hue_bar, _hue_bar_shader);
        hue_bar_render_task.register_color("_current_color_hsva", _shader_color);
        hue_bar_render_task.register_float("_hue_offset", &_shader_color->h);
        hue_bar_render_task.register_vec2("_canvas_size", _shader_canvas_size);

        add_render_task(hue_bar_render_task);
        add_render_task(_hue_bar_frame);

        add_render_task(_hue_bar_cursor);
        add_render_task(_hue_bar_cursor_frame_inner);
        add_render_task(_hue_bar_cursor_frame_outer);

        add_render_task(_hue_triangle_right);
        add_render_task(_hue_triangle_left);

        add_render_task(_hue_triangle_right_frame);
        add_render_task(_hue_triangle_left_frame);

        add_render_task(_cursor_frame);
        add_render_task(_cursor);

        update();
    }

    void HsvTriangleSelect::ShapeArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _shader_canvas_size->x = w;
        _shader_canvas_size->y = h;

        Vector2f now = {1.f / w, 1.f / h};
        if (now != one_px)
        {
            one_px = now;
            on_realize(area);
        }

        update();
        gtk_gl_area_queue_render(area);
    }

    void HsvTriangleSelect::ShapeArea::update()
    {
        _shader_color->h = current_color.h;
        _hue_triangle_right->set_color(current_color);

        _hsv_triangle->set_vertex_color(0, HSVA(current_color.h, 1, 1, 1));
        _hsv_triangle->set_vertex_color(1, RGBA(1, 1, 1, 1));
        _hsv_triangle->set_vertex_color(2, RGBA(0, 0, 0, 1));

        Vector2f a = _hsv_triangle->get_vertex_position(0); // value 1 saturation 1
        Vector2f b = _hsv_triangle->get_vertex_position(1); // value 1 saturation 0
        Vector2f c = _hsv_triangle->get_vertex_position(2); // value 0 saturation 1

        Vector2f pos = a;

        // saturation axis
        float angle = std::atan2(b.y - pos.y, b.x - pos.x);
        float d = current_color.s * glm::distance(a, b);
        pos += Vector2f(cos(angle) * d, sin(angle) * d);

        // value axis
        angle = std::atan2(c.y - pos.y, c.x - pos.x);
        d = current_color.v * glm::distance(pos, c);
        pos += Vector2f(cos(angle) * d, sin(angle) * d);

        for (auto* shape : {_cursor, _cursor_frame})
            shape->set_centroid(pos);
    }

    HsvTriangleSelect::HsvTriangleSelect(float width)
    {
        if (int(width) % 2 == 0)
            width += 1;

        _shape_area = new ShapeArea();
        _shape_area->set_size_request({width, width});
    }

    /*
    HsvTriangleSelect::HsvTriangleSelect(float width)
    {
        _triangle_area = new TriangleShaderArea();
        _triangle_area->set_size_request(Vector2f{width, width});
    }

    HsvTriangleSelect::~HsvTriangleSelect()
    {
        delete _triangle_area;
    }

    HsvTriangleSelect::TriangleShaderArea::TriangleShaderArea()
    {
        _shader = new Shader();
        _shader->create_from_file(get_resource_path() + "shaders/hsv_triangle_select.frag", ShaderType::FRAGMENT);
    }

    HsvTriangleSelect::TriangleShaderArea::~TriangleShaderArea()
    {
        delete _shader;
        delete _shape;
    }

    void HsvTriangleSelect::TriangleShaderArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        _shape = new Shape();
        _shape->as_rectangle({0, 0}, {1, 1});

        auto size = get_size();
        _canvas_size = new Vector2f();
        on_resize(area, size.x, size.y);

        auto task = RenderTask(_shape, _shader);
        task.register_vec2("_canvas_size", _canvas_size);
        task.register_color("_current_color_hsv", &current_color);

        add_render_task(task);
    }

    void HsvTriangleSelect::TriangleShaderArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _canvas_size->x = w;
        _canvas_size->y = h;

        gtk_gl_area_queue_render(area);
    }
     */
}