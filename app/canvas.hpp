// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <deque>

#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/get_resource_path.hpp>
#include <include/overlay.hpp>

#include <app/layer.hpp>

namespace mousetrap
{
    namespace state
    {
        std::deque<Layer> layers;
    }

    class Canvas : public Widget
    {
        public:
            Canvas(Vector2ui resolution);

            operator GtkWidget*() override;

            void add_layer(RGBA color);
            void add_layer(const Image&);

        private:

            // shared state

            static inline float negative_infinity = -1000;
            static inline float positive_infinity = +1000;

            GLTransform* _transform;
            static inline GLTransform* _noop_transform = new GLTransform();

            Vector2f* _canvas_size;
            Vector2f* _resolution;

            Overlay* _main_overlay;

            // underlay

            WidgetWrapper<GtkGLArea>* _underlay;

            Shape* _underlay_transparency_tiling;
            Shader* _underlay_transparency_tiling_shader;
            Shader* _underlay_noop_shader;

            Shape* _underlay_subtract_top;
            Shape* _underlay_subtract_right;
            Shape* _underlay_subtract_bottom;
            Shape* _underlay_subtract_left;

            static void on_underlay_realize(GtkGLArea*, Canvas*);
            static void on_underlay_resize(GtkGLArea*, int, int, Canvas*);
            static gboolean on_underlay_render(GtkGLArea*, GdkGLContext*, Canvas*);

            // layer

            struct LayerShape
            {
                Layer layer;
                Shape* shape;
            };

            std::deque<LayerShape*> _layers;

            WidgetWrapper<GtkGLArea>* _layer_area;

            Shape* _subtract_top;
            Shape* _subtract_right;
            Shape* _subtract_bottom;
            Shape* _subtract_left;

            Shape* _layer_boundary;
            Shader* _noop_shader;

            static void on_gl_area_realize(GtkGLArea*, Canvas*);
            static void on_gl_area_resize(GtkGLArea*, int, int, Canvas*);
            static gboolean on_gl_area_render(GtkGLArea*, GdkGLContext*, Canvas*);
    };
}

// ###

namespace mousetrap
{
    Canvas::operator GtkWidget*()
    {
        return _main_overlay->operator GtkWidget*();
    }

    Canvas::Canvas(Vector2ui resolution)
    {
        _resolution = new Vector2f(resolution.x, resolution.y);

        _layer_area = new WidgetWrapper<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()));
        gtk_gl_area_set_auto_render(_layer_area->operator _GtkGLArea*(), true);

        _layer_area->connect_signal("realize", on_gl_area_realize, this);
        _layer_area->connect_signal("resize", on_gl_area_resize, this);
        _layer_area->connect_signal("render", on_gl_area_render, this);

        _underlay = new WidgetWrapper<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()));
        _underlay->connect_signal("realize", on_underlay_realize, this);
        _underlay->connect_signal("resize", on_underlay_resize, this);
        _underlay->connect_signal("render", on_underlay_render, this);

        _underlay->set_expand(true);
        _underlay->set_size_request(*_resolution);

        _layer_area->set_expand(true);
        _layer_area->set_size_request(*_resolution);

        _main_overlay = new Overlay();
        _main_overlay->set_child(_underlay);
        _main_overlay->add_overlay(_layer_area);

        _canvas_size = new Vector2f(1, 1);
        _noop_transform = new GLTransform();
        _transform = new GLTransform();
    }

    void Canvas::on_underlay_realize(GtkGLArea* area, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_underlay_transparency_tiling = new Shape();

        instance->_underlay_transparency_tiling_shader = new Shader();
        instance->_underlay_transparency_tiling_shader->create_from_file(
            get_resource_path() + "shaders/transparency_tiling.frag",
            ShaderType::FRAGMENT
        );

        instance->_underlay_noop_shader = new Shader();

        instance->_underlay_subtract_top = new Shape();
        instance->_underlay_subtract_right = new Shape();
        instance->_underlay_subtract_bottom = new Shape();
        instance->_underlay_subtract_left = new Shape();

        on_underlay_resize(area, instance->_resolution->x, instance->_resolution->y, instance);
        gdk_gl_context_clear_current();
    }

    void Canvas::on_underlay_resize(GtkGLArea* area, int w_in, int h_in, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_canvas_size->x = w_in;
        instance->_canvas_size->y = h_in;

        float width = instance->_resolution->x / instance->_canvas_size->x;
        float height = instance->_resolution->y / instance->_canvas_size->y;

        float eps = 0.001;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = positive_infinity;
        float b = positive_infinity;

        instance->_underlay_transparency_tiling->as_rectangle(
            {-a, -b}, {2 * a, 2 * b}
        );

        instance->_underlay_subtract_top->as_rectangle(
                {x - a, y - b}, {x + w + a, y - b},
                {x - a, y}, {x + w + a, y}
        );

        instance->_underlay_subtract_bottom->as_rectangle(
                {x - a, y + h}, {x + w + a, y + h},
                {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        instance->_underlay_subtract_left->as_rectangle(
                {x - a, y}, {x, y},
                {x, y + h}, {x - a, y + h}
        );

        instance->_underlay_subtract_right->as_rectangle(
                {x + w, y}, {x + w + a, y},
                {x + w + a, y + h}, {x + w, y + h}
        );

        gtk_gl_area_queue_render(area);
    }

    gboolean Canvas::on_underlay_render(GtkGLArea* area, GdkGLContext*, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        auto* shader = instance->_underlay_transparency_tiling_shader;
        glUseProgram(shader->get_program_id());
        shader->set_uniform_vec2("_canvas_size", *instance->_canvas_size);

        set_blend_mode(BlendMode::NORMAL);
        instance->_underlay_transparency_tiling->render(*shader, *instance->_noop_transform);

        set_blend_mode(BlendMode::SUBTRACT);
        instance->_underlay_subtract_top->render(*instance->_underlay_noop_shader, *instance->_transform);
        instance->_underlay_subtract_right->render(*instance->_underlay_noop_shader, *instance->_transform);
        instance->_underlay_subtract_bottom->render(*instance->_underlay_noop_shader, *instance->_transform);
        instance->_underlay_subtract_left->render(*instance->_underlay_noop_shader, *instance->_transform);

        glFlush();
        gdk_gl_context_clear_current();
        return FALSE;
    }

    void Canvas::on_gl_area_realize(GtkGLArea* area, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_subtract_top = new Shape();
        instance->_subtract_bottom = new Shape();
        instance->_subtract_left = new Shape();
        instance->_subtract_right = new Shape();

        instance->_layer_boundary = new Shape();
        instance->_noop_shader = new Shader();

        // TODO
        auto pixbuf = gdk_pixbuf_new_from_file((get_resource_path() + "mole.png").c_str(), nullptr);
        Vector2ui pixbuf_size = Vector2ui{
                gdk_pixbuf_get_width(pixbuf),
                gdk_pixbuf_get_height(pixbuf)
        };
        gdk_pixbuf_scale_simple(pixbuf, 3 * pixbuf_size.x, 3 * pixbuf_size.y, GDK_INTERP_NEAREST);
        auto image = Image();
        image.create_from_pixbuf(pixbuf);
        instance->add_layer(image);
        instance->add_layer(RGBA(1, 1, 1, 1));

        instance->_layers.back()->layer.blend_mode = BlendMode::MULTIPLY;

        // TODO

        on_gl_area_resize(area, instance->_resolution->x, instance->_resolution->y, instance);
        gdk_gl_context_clear_current();
    }

    void Canvas::on_gl_area_resize(GtkGLArea* area, int w_in, int h_in, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_canvas_size->x = w_in;
        instance->_canvas_size->y = h_in;

        float width = instance->_resolution->x / instance->_canvas_size->x;
        float height = instance->_resolution->y / instance->_canvas_size->y;

        float eps = 0.001;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = positive_infinity;
        float b = positive_infinity;

        instance->_layer_boundary->as_wireframe({
                {x, y}, {x + w, y},
                {x + w, y + h}, {x, y + h}
        });
        instance->_layer_boundary->set_color(RGBA(0, 0, 0, 1));

        for (auto* layer : instance->_layers)
        {
            layer->shape->as_rectangle(
                    {0.5 - width / 2, 0.5 - height / 2},
                    {width, height}
            );
        }

        instance->_subtract_top->as_rectangle(
                {x - a, y - b}, {x + w + a, y - b},
                {x - a, y}, {x + w + a, y}
        );

        instance->_subtract_bottom->as_rectangle(
                {x - a, y + h}, {x + w + a, y + h},
                {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        instance->_subtract_left->as_rectangle(
                {x - a, y}, {x, y},
                {x, y + h}, {x - a, y + h}
        );

        instance->_subtract_right->as_rectangle(
                {x + w, y}, {x + w + a, y},
                {x + w + a, y + h}, {x + w, y + h}
        );

        gtk_gl_area_queue_render(area);
    }

    gboolean Canvas::on_gl_area_render(GtkGLArea* area, GdkGLContext*, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);

        for (auto* layer : instance->_layers)
        {
            set_blend_mode(layer->layer.blend_mode);
            layer->shape->render(*instance->_noop_shader, *instance->_transform);
        }

        set_blend_mode(BlendMode::SUBTRACT);
        instance->_subtract_top->render(*instance->_noop_shader, *instance->_transform);
        instance->_subtract_right->render(*instance->_noop_shader, *instance->_transform);
        instance->_subtract_bottom->render(*instance->_noop_shader, *instance->_transform);
        instance->_subtract_left->render(*instance->_noop_shader, *instance->_transform);

        set_blend_mode(BlendMode::NORMAL);
        instance->_layer_boundary->render(*instance->_noop_shader, *instance->_transform);

        glFlush();
        gdk_gl_context_clear_current();
        return FALSE;
    }

    void Canvas::add_layer(RGBA color)
    {
        auto image = Image();
        image.create(_resolution->x, _resolution->y, color);
        add_layer(image);
    }

    void Canvas::add_layer(const Image& image)
    {
        gtk_gl_area_make_current(_layer_area->operator GtkGLArea*());

        _layers.push_back(new LayerShape{
                Layer{
                        new Texture()
                },
                new Shape()
        });

        float width = _resolution->x / _canvas_size->x;
        float height = _resolution->y / _canvas_size->y;

        _layers.back()->shape->as_rectangle(
                {0.5 - width / 2, 0.5 - height / 2},
                {width, height}
        );

        auto image_size = Vector2f(image.get_size().x, image.get_size().y);

        if (image_size == *_resolution)
            ((Texture*) _layers.back()->layer.texture)->create_from_image(image);
        else
        {
            auto image_padded = Image();
            image_padded.create(_resolution->x, _resolution->y, RGBA(0, 0, 0, 0));

            for (size_t x = 0.5 * (image_size.x - _resolution->x); x < image_size.x and x < _resolution->x; ++x)
                for (size_t y = 0.5 * (image_size.y - _resolution->y); y < image_size.y and x < _resolution->y; ++y)
                    image_padded.set_pixel(x, y, image.get_pixel(x, y));

            ((Texture*) _layers.back()->layer.texture)->create_from_image(image);
        }

        _layers.back()->shape->set_texture(_layers.back()->layer.texture);
        gdk_gl_context_clear_current();
    }
}