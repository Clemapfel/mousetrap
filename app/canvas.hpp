// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <deque>

#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/get_resource_path.hpp>

#include <app/layer.hpp>

namespace mousetrap
{
    class Canvas : public Widget
    {
        public:
            Canvas(Vector2ui resolution);
            operator GtkWidget*() override;

            void add_layer(RGBA color);
            void add_layer(const Image&);

        private:
            AspectFrame* _frame;
            WidgetWrapper<GtkGLArea>* _gl_area;

            static inline float negative_infinity = -1000;
            static inline float positive_infinity = +1000;

            struct LayerShape
            {
                Layer layer;
                Shape* shape;
            };

            std::deque<LayerShape*> _layers;

            float _zoom = 1;
            Vector2f _offset = {0, 0};

            static inline Shader* _noop_shader = nullptr;
            static inline GLTransform* _noop_transform = nullptr;
            GLTransform* _transform;

            Shape* _transparency_tiling_background;
            Shader* _transparency_tiling_background_shader;

            Shape* _subtract_overlay_top;
            Shape* _subtract_overlay_right;
            Shape* _subtract_overlay_bottom;
            Shape* _subtract_overlay_left;

            Shape* _layer_boundary;

            Vector2f* _canvas_size;
            Vector2f* _resolution;

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
        return _gl_area->operator GtkWidget*();
    }

    Canvas::Canvas(Vector2ui resolution)
    {
        _resolution = new Vector2f(resolution.x, resolution.y);

        _gl_area = new WidgetWrapper<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()));
        gtk_gl_area_set_auto_render(_gl_area->operator _GtkGLArea*(), true);
        _gl_area->set_expand(true);

        _gl_area->connect_signal("realize", on_gl_area_realize, this);
        _gl_area->connect_signal("resize", on_gl_area_resize, this);
        _gl_area->connect_signal("render", on_gl_area_render, this);
        _gl_area->set_size_request(*_resolution);
    }

    void Canvas::on_gl_area_realize(GtkGLArea* area, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_canvas_size = new Vector2f(1, 1);

        instance->_transparency_tiling_background = new Shape();
        instance->_transparency_tiling_background->as_rectangle(
            Vector2f(negative_infinity),
            Vector2f(abs(negative_infinity) + positive_infinity)
        );

        instance->_transparency_tiling_background_shader = new Shader();
        instance->_transparency_tiling_background_shader->create_from_file(
            get_resource_path() + "shaders/transparency_tiling.frag",
            ShaderType::FRAGMENT
        );

        if (instance->_noop_shader == nullptr)
            instance->_noop_shader = new Shader();

        if (instance->_noop_transform == nullptr)
            instance->_noop_transform = new GLTransform();

        instance->_transform = new GLTransform();
        instance->_transform->scale(4, 4);

        instance->_layer_boundary = new Shape();
        instance->_subtract_overlay_top = new Shape();
        instance->_subtract_overlay_right = new Shape();
        instance->_subtract_overlay_bottom = new Shape();
        instance->_subtract_overlay_left = new Shape();

        on_gl_area_resize(area, 1, 1, instance);

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
        instance->add_layer(RGBA(1, 0, 0, 0.1));

        instance->_layers.back()->layer.blend_mode = BlendMode::MULTIPLY;

        // TODO

        gtk_gl_area_queue_render(area);
    }

    gboolean Canvas::on_gl_area_render(GtkGLArea*, GdkGLContext*, Canvas* instance)
    {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);

        // render transparency background

        auto* shader = instance->_transparency_tiling_background_shader;
        glUseProgram(shader->get_program_id());
        shader->set_uniform_vec2("_canvas_size", *instance->_canvas_size);

        set_blend_mode(BlendMode::NORMAL);
        instance->_transparency_tiling_background->render(*shader, *instance->_noop_transform);

        // render layers

        for (auto* layer : instance->_layers)
        {
            set_blend_mode(layer->layer.blend_mode);
            layer->shape->render(*instance->_noop_shader, *instance->_transform);
        }

        // render overlay

        set_blend_mode(BlendMode::SUBTRACT);
        instance->_subtract_overlay_top->render(*instance->_noop_shader, *instance->_transform);
        instance->_subtract_overlay_right->render(*instance->_noop_shader, *instance->_transform);
        instance->_subtract_overlay_bottom->render(*instance->_noop_shader, *instance->_transform);
        instance->_subtract_overlay_left->render(*instance->_noop_shader, *instance->_transform);

        set_blend_mode(BlendMode::NORMAL);
        instance->_layer_boundary->render(*instance->_noop_shader, *instance->_transform);

        glFlush();
        return FALSE;
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

        float a = positive_infinity;
        float b = positive_infinity;

        instance->_subtract_overlay_top->as_rectangle(
            {x - a, y - b}, {x + w + a, y - b},
            {x - a, y}, {x + w + a, y}
        );

        instance->_subtract_overlay_bottom->as_rectangle(
            {x - a, y + h}, {x + w + a, y + h},
            {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        instance->_subtract_overlay_left->as_rectangle(
            {x - a, y}, {x, y},
            {x, y + h}, {x - a, y + h}
        );

        instance->_subtract_overlay_right->as_rectangle(
            {x + w, y}, {x + w + a, y},
            {x + w + a, y + h}, {x + w, y + h}
        );

        gtk_gl_area_queue_render(area);
    }

    void Canvas::add_layer(RGBA color)
    {
        auto image = Image();
        image.create(_resolution->x, _resolution->y, color);
        add_layer(image);
    }

    void Canvas::add_layer(const Image& image)
    {
        gtk_gl_area_make_current(_gl_area->operator GtkGLArea*());

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
    }
}