// 
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/get_resource_path.hpp>
#include <include/geometric_shapes.hpp>

#include <include/components/global_state.hpp>

namespace mousetrap
{
    struct HsvTriangleSelect : public Widget, public Updatable
    {
        public:
            HsvTriangleSelect();
            void update();

            GtkWidget* get_native() {
                return _shape_area->get_native();
            }

        private:
            // signals
            static void on_pointer_motion(GtkWidget *widget, GdkEventMotion *event, HsvTriangleSelect* instance);
            static void on_button_press(GtkWidget* widget, GdkEventButton* event, HsvTriangleSelect* instance);
            static void on_button_release(GtkWidget* widget, GdkEventButton* event, HsvTriangleSelect* instance);

            bool _hue_bar_active = false;
            bool _hsv_triangle_active = false;

            struct ShapeArea : public GLArea
            {
                ShapeArea() = default;
                ~ShapeArea();

                void on_realize(GtkGLArea* area);
                void on_resize(GtkGLArea* area, int, int);

                void update();

                bool is_point_in_hsv_triangle(Vector2f);
                bool is_point_in_hue_bar(Vector2f);

                Vector2f color_to_hsv_triangle_cursor_position(HSVA);
                Vector2f color_to_hue_bar_cursor_position(HSVA);

                HSVA color_from_cursor_positions(); // from hue bar and tri cursor position

                void set_hue_bar_cursor(Vector2f);
                void set_hsv_triangle_cursor(Vector2f);

                Shape* _hsv_triangle;
                Shape* _hsv_triangle_frame;

                Shape* _hue_triangle;
                Shape* _hue_triangle_frame;

                Shape* _hue_triangle_transparency_tiling;
                Shader* _transparency_tiling_shader;

                Vector2f* _canvas_size = new Vector2f();

                Shape* _hue_bar;
                Shape* _hue_bar_frame;

                Shader* _hue_bar_shader;

                Shape* _hue_bar_cursor;
                Shape* _hue_bar_cursor_frame_inner;
                Shape* _hue_bar_cursor_frame_outer;

                Shape* _hsv_triangle_cursor;
                Shape* _hsv_triangle_cursor_frame;
            };

            ShapeArea* _shape_area;
    };
}

// ###

namespace mousetrap
{
    void HsvTriangleSelect::ShapeArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _canvas_size->x = w;
        _canvas_size->y = h;

        on_realize(area);
        gtk_gl_area_queue_render(area);
    }

    HsvTriangleSelect::ShapeArea::~ShapeArea()
    {}

    void HsvTriangleSelect::ShapeArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        float margin = 0.05;
        // hue bar

        float hue_bar_left_margin = margin;
        float hue_bar_right_margin = hue_bar_left_margin;
        float hue_bar_bottom_margin = margin;
        float hue_bar_top_margin = 0.5 * margin;

        Vector2f hue_bar_size = {1 - (hue_bar_left_margin + hue_bar_right_margin), 3 * margin};
        Vector2f hue_bar_pos = {hue_bar_left_margin, 1 - hue_bar_bottom_margin - hue_bar_size.y};

        if (not _hue_bar) _hue_bar = new Shape();
        _hue_bar->as_rectangle(hue_bar_pos, hue_bar_size);

        if (not _hue_bar_frame)
            _hue_bar_frame = new Shape();
        _hue_bar_frame->as_wireframe({
            _hue_bar->get_vertex_position(0),
            _hue_bar->get_vertex_position(1),
            _hue_bar->get_vertex_position(2),
            _hue_bar->get_vertex_position(3)
        });
        _hue_bar_frame->set_color(RGBA(0, 0, 0, 1));

        if (not _hue_bar_shader)
            _hue_bar_shader = new Shader();
        _hue_bar_shader->create_from_file(get_resource_path() + "shaders/color_picker_hue_gradient.frag", ShaderType::FRAGMENT);

        auto hue_bar_task = RenderTask(_hue_bar, _hue_bar_shader);

        static HSVA dummy = HSVA(1, 1, 1, 1);
        hue_bar_task.register_color("_current_color_hsva", &dummy);

        add_render_task(hue_bar_task);
        add_render_task(_hue_bar_frame);

        // hue bar cursor

        Vector2f one_px = {1.f / get_size().x, 1.f / get_size().y};

        float hue_bar_cursor_height = hue_bar_size.y + 4 * one_px.y;
        float hue_bar_cursor_frame_width_base = std::max<float>(0.075 * hue_bar_size.y, one_px.x * 7);
        float cursor_width = std::max<float>(0.05, hue_bar_cursor_frame_width_base * 2 + one_px.x * 7);
        auto hue_bar_cursor_frame_width = Vector2f(hue_bar_cursor_frame_width_base, hue_bar_cursor_frame_width_base * (one_px.y / one_px.x));
        Vector2f hue_bar_cursor_size = {cursor_width, hue_bar_cursor_height};

        if (not _hue_bar_cursor)
            _hue_bar_cursor = new Shape();

        _hue_bar_cursor->as_frame({0, 0}, hue_bar_cursor_size, hue_bar_cursor_frame_width.x, hue_bar_cursor_frame_width.y);

        if (not _hue_bar_cursor_frame_inner)
            _hue_bar_cursor_frame_inner = new Shape();
        _hue_bar_cursor_frame_inner->as_wireframe({
              {hue_bar_cursor_frame_width.x, hue_bar_cursor_frame_width.y},
              {hue_bar_cursor_size.x - hue_bar_cursor_frame_width.x, hue_bar_cursor_frame_width.y},
              {hue_bar_cursor_size.x - hue_bar_cursor_frame_width.x, hue_bar_cursor_size.y - hue_bar_cursor_frame_width.y},
              {hue_bar_cursor_frame_width.x, hue_bar_cursor_size.y - hue_bar_cursor_frame_width.y}
        });
        _hue_bar_cursor_frame_inner->set_color(RGBA(0, 0, 0, 1));

        if (not _hue_bar_cursor_frame_outer)
            _hue_bar_cursor_frame_outer = new Shape();
        _hue_bar_cursor_frame_outer->as_wireframe({
              {0, 0},
              {hue_bar_cursor_size.x, 0},
              {hue_bar_cursor_size.x, hue_bar_cursor_size.y},
              {0, hue_bar_cursor_size.y},
        });
        _hue_bar_cursor_frame_outer->set_color(RGBA(0, 0, 0, 1));

        for (auto* shape : {_hue_bar_cursor_frame_inner, _hue_bar_cursor_frame_outer, _hue_bar_cursor})
            shape->set_centroid(Vector2f(0.5, _hue_bar->get_centroid().y));

        add_render_task(_hue_bar_cursor);
        add_render_task(_hue_bar_cursor_frame_outer);
        add_render_task(_hue_bar_cursor_frame_inner);

        // hsv triangle

        float hsv_triangle_radius = (1 - hue_bar_size.y - hue_bar_bottom_margin - hue_bar_top_margin) / 1.6;
        Vector2f hsv_triangle_center = {0.5, hue_bar_pos.y - hue_bar_top_margin - hsv_triangle_radius};

        {
            // creating circle shape and reading vertices is easier than calcing position
            auto temp = Shape();
            temp.as_circle(hsv_triangle_center, hsv_triangle_radius, 3);
            temp.rotate(degrees(270));
            temp.set_centroid(hsv_triangle_center);

            if (not _hsv_triangle)
                _hsv_triangle = new Shape();
            _hsv_triangle->as_triangle(
                    temp.get_vertex_position(1),    // sic, 0 is centroid
                    temp.get_vertex_position(2),
                    temp.get_vertex_position(3)
            );
        }

        if (not _hsv_triangle_frame)
            _hsv_triangle_frame = new Shape();
        _hsv_triangle_frame->as_wireframe({
            _hsv_triangle->get_vertex_position(0),
            _hsv_triangle->get_vertex_position(1),
            _hsv_triangle->get_vertex_position(2)
        });
        _hsv_triangle_frame->set_color(RGBA(0, 0, 0, 1));

        add_render_task(_hsv_triangle);
        add_render_task(_hsv_triangle_frame);

        // hsv triangle cursor

        float hsv_triangle_cursor_radius = std::max<float>(5 * one_px.x, margin * 0.5);

        if (not _hsv_triangle_cursor)
            _hsv_triangle_cursor = new Shape();
        _hsv_triangle_cursor->as_circle({0.5, 0.5}, hsv_triangle_cursor_radius, 8);

        if (not _hsv_triangle_cursor_frame)
            _hsv_triangle_cursor_frame = new Shape();
        _hsv_triangle_cursor_frame->as_circle({0.5, 0.5}, hsv_triangle_cursor_radius * 1.1, 8);
        _hsv_triangle_cursor_frame->set_color(RGBA(0, 0, 0, 1));

        add_render_task(_hsv_triangle_cursor_frame);
        add_render_task(_hsv_triangle_cursor);

        // hue triangle

        float hue_triangle_radius = hue_bar_size.y * 1.25;

        if (not _hue_triangle)
            _hue_triangle = new Shape();
        _hue_triangle->as_circle({0, 0}, hue_triangle_radius, 3);

        if (not _hue_triangle_transparency_tiling)
            _hue_triangle_transparency_tiling = new Shape();
        _hue_triangle_transparency_tiling->as_circle({0, 0}, hue_triangle_radius, 3);

        for (auto* shape : {_hue_triangle, _hue_triangle_transparency_tiling})
            shape->rotate(degrees(90));

        {
            // align right vertex with right end of hue bar
            auto centroid = _hue_triangle->get_centroid();
            auto right = _hue_triangle->get_vertex_position(2);

            Vector2f hue_triangle_centroid = Vector2f{
                hue_bar_pos.x + hue_bar_size.x - abs(centroid.x - right.x),
                hue_bar_pos.y - 1.5 * margin - abs(centroid.y - right.y)
            };

            _hue_triangle->set_centroid(hue_triangle_centroid);
            _hue_triangle_transparency_tiling->set_centroid(hue_triangle_centroid);
        }

        if (not _hue_triangle_frame)
            _hue_triangle_frame = new Shape();
        _hue_triangle_frame->as_wireframe({
            _hue_triangle->get_vertex_position(1),
            _hue_triangle->get_vertex_position(2),
            _hue_triangle->get_vertex_position(3),
        });
        _hue_triangle_frame->set_color(RGBA(0, 0, 0, 1));

        if (not _transparency_tiling_shader)
            _transparency_tiling_shader = new Shader();
        _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        auto alpha_task = RenderTask(_hue_triangle_transparency_tiling, _transparency_tiling_shader);
        alpha_task.register_vec2("_canvas_size", _canvas_size);

        add_render_task(alpha_task);
        add_render_task(_hue_triangle);
        add_render_task(_hue_triangle_frame);

        update();

        gtk_gl_area_queue_render(area);
    }

    bool HsvTriangleSelect::ShapeArea::is_point_in_hsv_triangle(Vector2f pos)
    {
        static auto sign = [](Vector2f p1, Vector2f p2, Vector2f p3){
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
        };

        auto p1 = _hsv_triangle->get_vertex_position(0);
        auto p2 = _hsv_triangle->get_vertex_position(1);
        auto p3 = _hsv_triangle->get_vertex_position(2);

        float d1 = sign(pos, p1, p2);
        float d2 = sign(pos, p2, p3);
        float d3 = sign(pos, p3, p1);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    bool HsvTriangleSelect::ShapeArea::is_point_in_hue_bar(Vector2f pos)
    {
        Vector2f top_left = _hue_bar->get_top_left();
        Vector2f size = _hue_bar->get_size();

        return (pos.x >= top_left.x and pos.x <= top_left.x + size.x) and
               (pos.y >= top_left.y and pos.y <= top_left.y + size.y);
    }

    Vector2f HsvTriangleSelect::ShapeArea::color_to_hue_bar_cursor_position(HSVA color)
    {
        float hue = color.h;
        Vector2f top_left = _hue_bar->get_top_left();
        Vector2f size = _hue_bar->get_size();

        return {top_left.x + hue * size.x, top_left.y + size.y * 0.5};
    }

    Vector2f HsvTriangleSelect::ShapeArea::color_to_hsv_triangle_cursor_position(HSVA color)
    {
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

        return pos;
    }

    void HsvTriangleSelect::ShapeArea::set_hsv_triangle_cursor(Vector2f pos)
    {
        auto set_cursor_pos = [&](Vector2f pos) -> void
        {
            _hsv_triangle_cursor->set_centroid(pos);
            _hsv_triangle_cursor_frame->set_centroid(pos);
        };

        if (is_point_in_hsv_triangle(pos))
        {
            set_cursor_pos(pos);
            return;
        }

        // if outside of triangle and click began from within triangle, interpolate outside position

        auto centroid = _hsv_triangle->get_centroid();
        auto cursor_line = Line{centroid, pos};

        auto p1 = _hsv_triangle->get_vertex_position(0);
        auto p2 = _hsv_triangle->get_vertex_position(1);
        auto p3 = _hsv_triangle->get_vertex_position(2);

        auto triangle_line_12 = Line{p1, p2};
        auto triangle_line_23 = Line{p2, p3};
        auto triangle_line_31 = Line{p3, p1};

        Vector2f point;
        if (intersecting(cursor_line, triangle_line_12, &point))
            set_cursor_pos(point);

        else if (intersecting(cursor_line, triangle_line_23, &point))
            set_cursor_pos(point);

        else if (intersecting(cursor_line, triangle_line_31, &point))
            set_cursor_pos(point);

        queue_render();
    }

    void HsvTriangleSelect::ShapeArea::set_hue_bar_cursor(Vector2f pos)
    {
        auto set_cursor_pos = [&](float x) -> void
        {
            auto centroid = _hue_bar->get_centroid();

            for (auto* shape : {_hue_bar_cursor, _hue_bar_cursor_frame_inner, _hue_bar_cursor_frame_outer})
                shape->set_centroid({x, centroid.y});
        };

        Vector2f top_left = _hue_bar->get_top_left();
        Vector2f size = _hue_bar->get_size();

        if (pos.x < top_left.x)
            set_cursor_pos(top_left.x);
        else if (pos.x > top_left.x + size.x)
            set_cursor_pos(top_left.x + size.x);
        else
            set_cursor_pos(pos.x);
    }

    HSVA HsvTriangleSelect::ShapeArea::color_from_cursor_positions()
    {
        float hue = (_hue_bar_cursor_frame_outer->get_centroid().x - _hue_bar->get_top_left().x) / _hue_bar->get_size().x;

        Vector2f a = _hsv_triangle->get_vertex_position(0); // value 1 saturation 1
        Vector2f b = _hsv_triangle->get_vertex_position(1); // value 1 saturation 0
        Vector2f c = _hsv_triangle->get_vertex_position(2); // value 0 saturation 1

        Line saturation_plane = Line{a, b};
        Vector2f pos = _hsv_triangle_cursor->get_centroid();

        float value_plane_angle = atan2(pos.y - c.y, pos.x - c.x);

        auto value_plane_start = c;
        auto value_plane_end = Vector2f(c.x + cos(value_plane_angle), c.y + sin(value_plane_angle));

        Vector2f intersect;
        intersecting(Line{value_plane_start, value_plane_end}, saturation_plane, &intersect);

        float saturation = 1 - glm::distance(intersect, a) / glm::distance(a, b);
        float value = glm::distance(pos, c) / glm::distance(intersect, c);

        return HSVA(hue, saturation, value, current_color.a);
    }

    void HsvTriangleSelect::ShapeArea::update()
    {
        _hue_triangle->set_color(current_color);

        _hsv_triangle->set_vertex_color(0, HSVA(current_color.h, 1, 1, 1));
        _hsv_triangle->set_vertex_color(1, RGBA(1, 1, 1, 1));
        _hsv_triangle->set_vertex_color(2, RGBA(0, 0, 0, 1));

        queue_render();
    }

    void HsvTriangleSelect::on_button_press(GtkWidget* widget, GdkEventButton* event, HsvTriangleSelect* instance)
    {
        auto pos = Vector2f(event->x, event->y);
        pos /= instance->_shape_area->get_size();

        if (event->button != 1)
            return;

        if (instance->_shape_area->is_point_in_hsv_triangle(pos))
        {
            instance->_hsv_triangle_active = true;
            instance->_shape_area->set_hsv_triangle_cursor(pos);
            current_color = instance->_shape_area->color_from_cursor_positions();
            signal_color_updated();
        }
        else if (instance->_shape_area->is_point_in_hue_bar(pos))
        {
            instance->_hue_bar_active = true;
            instance->_shape_area->set_hue_bar_cursor(pos);
            current_color = instance->_shape_area->color_from_cursor_positions();
            signal_color_updated();
        }
    }

    void HsvTriangleSelect::on_button_release(GtkWidget* widget, GdkEventButton* event, HsvTriangleSelect* instance)
    {
        auto pos = Vector2f(event->x, event->y);
        pos /= instance->_shape_area->get_size();

        if (event->button != 1)
            return;

        instance->_hsv_triangle_active = false;
        instance->_hue_bar_active = false;
    }

    void HsvTriangleSelect::on_pointer_motion(GtkWidget* widget, GdkEventMotion* event, HsvTriangleSelect* instance)
    {
        auto pos = Vector2f(event->x, event->y);
        pos /= instance->_shape_area->get_size();

        if (instance->_hsv_triangle_active)
        {
            instance->_shape_area->set_hsv_triangle_cursor(pos);
            current_color = instance->_shape_area->color_from_cursor_positions();
            signal_color_updated();
        }
        else if (instance->_hue_bar_active)
        {
            instance->_shape_area->set_hue_bar_cursor(pos);
            current_color = instance->_shape_area->color_from_cursor_positions();
            signal_color_updated();
        }
    }

    HsvTriangleSelect::HsvTriangleSelect()
    {
        _shape_area = new ShapeArea();

        _shape_area->add_events(GDK_BUTTON_PRESS_MASK);
        _shape_area->add_events(GDK_BUTTON_RELEASE_MASK);
        _shape_area->add_events(GDK_POINTER_MOTION_MASK);

        _shape_area->connect_signal("motion-notify-event", on_pointer_motion, this);
        _shape_area->connect_signal("button-press-event", on_button_press, this);
        _shape_area->connect_signal("button-release-event", on_button_release, this);
    }

    void HsvTriangleSelect::update()
    {
        _shape_area->update();
        _shape_area->set_hue_bar_cursor(_shape_area->color_to_hue_bar_cursor_position(current_color));
        _shape_area->set_hsv_triangle_cursor(_shape_area->color_to_hsv_triangle_cursor_position(current_color));
    }
}