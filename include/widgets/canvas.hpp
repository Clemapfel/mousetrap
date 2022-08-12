// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/frame.hpp>
#include <include/shape.hpp>
#include <include/get_resource_path.hpp>
#include <include/pixel_buffer.hpp>
#include <include/scrollbar.hpp>

namespace mousetrap
{
    class Canvas : public Widget
    {
        public:
            Canvas(size_t width, size_t height);
            ~Canvas();

            GtkWidget* get_native() override {
                return GTK_WIDGET(_scrollbar_overlay->get_native());
            }

        private:
            static gboolean on_render(GtkGLArea*, GdkGLContext*, Canvas* instance);
            static void on_resize(GtkGLArea*, int, int, Canvas* instance);
            static void on_realize(GtkGLArea*, Canvas* instance);

            static inline float scroll_speed_factor = 0.1;
            static inline bool scroll_y_inverted = false;
            static inline bool scroll_x_inverted = true;
            static gboolean on_scroll_event(GtkWidget*, GdkEventScroll*, Canvas* instance);

            static void on_horizontal_scroll_value_changed(GtkRange* range, Canvas* instance);
            static void on_vertical_scroll_value_changed(GtkRange* range, Canvas* instance);

            void reformat();

            Vector2f _widget_size = {1, 1};
            Vector2f _resolution;
            
            // transform
            void translate_transform(float x, float y, bool update_gui);
            Vector2f _translation_offset = {0, 0};
            
            // scrollbar
            Overlay* _scrollbar_overlay;
            Box* _scrollbar_box;

            static inline const float max_scroll_value = 1.9;

            Scrollbar* _scrollbar_horizontal;
            Scrollbar* _scrollbar_vertical;

            // gl area
            WidgetWrapper<GtkGLArea>* _gl_area;

            // transparency background
            Shape* _transparency_tiling;
            Shader* _transparency_tiling_shader;

            // grid
            static inline const RGBA _default_grid_color = RGBA(0.0, 0.0, 0.0, 1);
            RGBA _grid_color = _default_grid_color;
            void set_grid_color(RGBA);

            bool _grid_enabled = false; // application interface
            bool _show_grid = true;     // depends on scale
            std::vector<Shape*> _grid;

            // guides
            static inline const RGBA _default_guide_color = RGBA(0.9, 0.9, 0.9, 1);
            RGBA _guide_color = _default_grid_color;
            void set_guide_color(RGBA);

            Shape* _canvas_frame;
            std::array<Shape*, 2> _half_guides;
            std::array<Shape*, 4> _third_guides;

            bool _draw_canvas_frame = false;
            bool _draw_half_guides = false;
            bool _draw_third_guides = false;

            // overlay
            std::array<Shape*, 4> _infinity_frame_overlay;

            // layer
            struct Layer
            {
                Layer(size_t width, size_t height);

                Shape* _shape;
                Texture* _texture;
                PixelBuffer* _pixel_buffer;
            };

            Layer* _layer;
    };
}

// ###

namespace mousetrap
{
    Canvas::Layer::Layer(size_t width, size_t height)
    {
        _shape = new Shape();
        _shape->as_rectangle({0, 0}, {1, 1});

        _texture = new Texture();
        _shape->set_texture(_texture);

        _pixel_buffer = new PixelBuffer(width, height);

        // TODO
        auto temp = Image();
        temp.create_from_file(get_resource_path() + "mole.png");

        for (size_t x = 0; x < temp.get_size().x; ++x)
            for (size_t y = 0; y < temp.get_size().y; ++y)
                _pixel_buffer->set_pixel(
                    x + 0.5 * width - 0.5 * temp.get_size().x,
                    y + 0.5 * height - 0.5 * temp.get_size().y,
                    temp.get_pixel(x, y)
                );
        // TODO

        _pixel_buffer->flush();
        _pixel_buffer->unbind();
    }

    Canvas::Canvas(size_t width, size_t height)
        : _resolution(width, height)
    {
        float scrollbar_width = 15;

        _gl_area = new WidgetWrapper<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()));
        gtk_gl_area_set_has_alpha(_gl_area->_native, TRUE);
        _gl_area->set_size_request({width * 3, height * 3});

        // scroll bars
        _scrollbar_vertical = new Scrollbar(Adjustment(0, -max_scroll_value, +max_scroll_value, 0.001, 0, 0), GTK_ORIENTATION_VERTICAL);
        _scrollbar_horizontal = new Scrollbar(Adjustment(0, -max_scroll_value, +max_scroll_value, 0.001, 0, 0), GTK_ORIENTATION_HORIZONTAL);

        _scrollbar_horizontal->set_valign(GTK_ALIGN_END);
        _scrollbar_horizontal->set_size_request({1, scrollbar_width});
        _scrollbar_vertical->set_halign(GTK_ALIGN_END);
        _scrollbar_vertical->set_size_request({1, scrollbar_width});

        _scrollbar_overlay = new Overlay();
        _scrollbar_overlay->set_under(_gl_area);
        _scrollbar_overlay->set_over(_scrollbar_horizontal);
        _scrollbar_overlay->set_over(_scrollbar_vertical);

        _scrollbar_horizontal->connect_signal("value-changed", on_horizontal_scroll_value_changed, this);
        _scrollbar_vertical->connect_signal("value-changed", on_vertical_scroll_value_changed, this);

        // shaders
        _transparency_tiling_shader = new Shader();
        _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        // gl area
        _gl_area->connect_signal("resize", on_resize, this);
        _gl_area->connect_signal("render", on_render, this);
        _gl_area->connect_signal("realize", on_realize, this);

        _gl_area->add_events(GDK_SMOOTH_SCROLL_MASK);
        _gl_area->connect_signal("scroll-event", on_scroll_event, this);
    }

    Canvas::~Canvas()
    {
        delete _scrollbar_overlay;
        delete _scrollbar_box;
        delete _scrollbar_horizontal;
        delete _scrollbar_vertical;

        delete _gl_area;

        delete _transparency_tiling;
        delete _transparency_tiling_shader;

        for (auto* s : _grid)
            delete s;

        delete _canvas_frame;

        for (auto* s : _half_guides)
            delete s;

        for (auto* s : _third_guides)
            delete s;

        for (auto* s : _infinity_frame_overlay)
            delete s;

        delete _layer;
    }

    void Canvas::set_guide_color(RGBA color)
    {
        _guide_color = color;
        _canvas_frame->set_color(color);

        for (auto* s : _half_guides)
            s->set_color(color);

        for (auto* s : _third_guides)
            s->set_color(color);
    }

    void Canvas::set_grid_color(RGBA color)
    {
        _grid_color = color;

        for (auto* s : _grid)
            s->set_color(color);
    }

    void Canvas::reformat()
    {
        _transparency_tiling->as_rectangle({0, 0}, {1, 1});

        float lower_bound = -1;
        float upper_bound = +2;

        for (size_t i = 1; i < _resolution.x; ++i)
        {
            _grid.at(i)->as_line(
                {i * (1.f / _resolution.x), 0},
                {i * (1.f / _resolution.x), 1}
            );
        }

        for (size_t i = 1; i < _resolution.x; ++i)
        {
            _grid.at(i)->as_line(
                {0, i * (1.f / _resolution.y)},
                {1, i * (1.f / _resolution.y)}
            );
        }

        float eps = 0.001;
        _canvas_frame->as_wireframe({
              {eps, eps}, {eps, 1 - eps}, {1 - eps, 1 - eps}, {1 - eps, eps}
        });

        _half_guides[0]->as_line({0, 0.5}, {1, 0.5});
        _half_guides[1]->as_line({0.5, 0}, {0.5, 1});

        _third_guides[0]->as_line({1/3.f, 0}, {1/3.f, 1});
        _third_guides[1]->as_line({2/3.f, 0}, {2/3.f, 1});
        _third_guides[2]->as_line({0, 1/3.f}, {1, 1/3.f});
        _third_guides[3]->as_line({0, 2/3.f}, {1, 2/3.f});

        set_guide_color(_guide_color);

        for (size_t i = 0; i < 4; ++i)
            _infinity_frame_overlay[i] = new Shape();

        static const float dist = 100000; // simulates infinite plane

        _infinity_frame_overlay[0]->as_rectangle(
            {-dist, -dist}, {1 + dist, -dist}, {1 + dist, 0}, {-dist, 0}
        );

        _infinity_frame_overlay[1]->as_rectangle(
            {1, 0}, {1 + dist, 0}, {1 + dist, 1}, {1, 1}
        );

        _infinity_frame_overlay[2]->as_rectangle(
            {-dist, 1}, {1 + dist, 1}, {1 + dist, 1 + dist}, {-dist, 1 + dist}
        );

        _infinity_frame_overlay[3]->as_rectangle(
            {-dist, 0}, {0, 0}, {0, 1}, {-dist, 1}
        );

        for (auto* s : _infinity_frame_overlay)
            s->set_color(RGBA(0, 0, 0, 0));

        _layer->_shape->as_rectangle(
            {0, 0}, {1, 1}
        );
    }

    void Canvas::on_realize(GtkGLArea* area, Canvas* instance)
    {
        gtk_gl_area_make_current(area);
        glEnable(GL_BLEND);

        assert(instance->_resolution.x > 1 and instance->_resolution.y > 1);

        instance->_transparency_tiling = new Shape();

        for (size_t i = 1; i < instance->_resolution.x; ++i)
            instance->_grid.push_back(new Shape());

        for (size_t i = 1; i < instance->_resolution.y; ++i)
            instance->_grid.push_back(new Shape());

        for (auto* s : instance->_grid)
            s->set_color(RGBA(0, 0, 0, 1));

        // guides

        instance->_canvas_frame = new Shape();

        for (size_t i = 0; i < instance->_third_guides.size(); ++i)
            instance->_half_guides[i] = new Shape();

        for (size_t i = 0; i < instance->_third_guides.size(); ++i)
            instance->_third_guides[i] = new Shape();

        // infinity frame overlay

        for (size_t i = 0; i < 4; ++i)
            instance->_infinity_frame_overlay[i] = new Shape();

        instance->_layer = new Layer(instance->_resolution.x, instance->_resolution.y);
        instance->reformat();
    }

    gboolean Canvas::on_render(GtkGLArea* area, GdkGLContext*, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        static auto noop_transform = GLTransform();
        static auto noop_shader = Shader();
        
        auto transform = GLTransform();
        transform.translate(instance->_translation_offset);

        // tiling

        glUseProgram(instance->_transparency_tiling_shader->get_program_id());
        instance->_transparency_tiling_shader->set_uniform_vec2("_canvas_size", instance->_widget_size);
        instance->_transparency_tiling->render(*(instance->_transparency_tiling_shader), noop_transform);
        glUseProgram(0);

        // TODO
        instance->_layer->_texture->bind();
        instance->_layer->_pixel_buffer->bind();
        instance->_layer->_shape->render(noop_shader, transform);
        instance->_layer->_pixel_buffer->unbind();
        instance->_layer->_texture->unbind();
        // TODO

        // grid
        if (instance->_grid_enabled and instance->_show_grid)
        {
            for (auto* line : instance->_grid)
                line->render(noop_shader, transform);
        }

        // guides

        if (instance->_draw_canvas_frame)
            instance->_canvas_frame->render(noop_shader, transform);

        if (instance->_draw_half_guides)
            for (auto* s : instance->_half_guides)
                s->render(noop_shader, transform);

        if (instance->_draw_third_guides)
            for (auto* s : instance->_third_guides)
                s->render(noop_shader, transform);

        // inf frame
        glBlendFunc(GL_SRC_COLOR, GL_ZERO); // override source with overlay, which is fully transparent

        for (auto& shape : instance->_infinity_frame_overlay)
            shape->render(noop_shader, transform);

        glFlush();
        return FALSE;
    }

    void Canvas::on_resize(GtkGLArea* area, int w, int h, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        if (h % 2 != 0)
            h -= 1;

        if (w % 2 != 0)
            w -= 1;

        instance->_widget_size = {w, h};
        instance->reformat();

        if (std::min(float(w) / instance->_resolution.x, float(h) / instance->_resolution.y) < 7)
            instance->_show_grid = false;
        else
            instance->_show_grid = true;

        gtk_gl_area_queue_render(instance->_gl_area->_native);
    }

    gboolean Canvas::on_scroll_event(GtkWidget*, GdkEventScroll* event, Canvas* instance)
    {
        Vector2f pointer_coord = {event->x_root, event->y_root}; // relative to widget

        double x, y;
        gdk_event_get_scroll_deltas((GdkEvent*) event, &x, &y);

        instance->_translation_offset += Vector2f{
               scroll_speed_factor * (scroll_x_inverted ? -1 : 1) * x,
               scroll_speed_factor * (scroll_y_inverted ? -1 : 1) * y
        };

        instance->_translation_offset = glm::clamp(instance->_translation_offset,
               Vector2f(-max_scroll_value, -max_scroll_value),
               Vector2f(+max_scroll_value, +max_scroll_value)
        );

        instance->_scrollbar_horizontal->set_all_signals_blocked(true);
        instance->_scrollbar_vertical->set_all_signals_blocked(true);

        instance->_scrollbar_horizontal->get_adjustment().set_value(
                (scroll_x_inverted ? -1 : 1) * instance->_translation_offset.x
        );

        instance->_scrollbar_vertical->get_adjustment().set_value(
                (scroll_y_inverted ? -1 : 1) * instance->_translation_offset.y
        );

        instance->_scrollbar_horizontal->set_all_signals_blocked(false);
        instance->_scrollbar_vertical->set_all_signals_blocked(false);

        gtk_gl_area_queue_render(instance->_gl_area->_native);
        return false;
    }

    void Canvas::on_horizontal_scroll_value_changed(GtkRange* range, Canvas* instance)
    {
        auto value = gtk_range_get_value(range);
        instance->_translation_offset.x = (scroll_x_inverted ? -1 : 1) * value;
        gtk_gl_area_queue_render(instance->_gl_area->_native);
    }

    void Canvas::on_vertical_scroll_value_changed(GtkRange* range, Canvas* instance)
    {
        auto value = gtk_range_get_value(range);
        instance->_translation_offset.y = (scroll_y_inverted ? -1 : 1) * value;
        gtk_gl_area_queue_render(instance->_gl_area->_native);
    }
}