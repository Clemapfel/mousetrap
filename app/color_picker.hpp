// 
// Copyright 2022 Clemens Cords
// Created on 8/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/frame.hpp>

namespace mousetrap
{
    class ColorPicker : public Widget
    {
        public:
            ColorPicker();
            ~ColorPicker();

            GtkWidget* get_native() override;

            void update() override;

        private:
            void update(HSVA Color);

            static void on_render_area_realize(GtkGLArea*, ColorPicker* instance);
            static void on_render_area_resize(GtkGLArea*, int, int , ColorPicker* instance);

            static void on_render_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, ColorPicker* instance);
            static void on_render_area_button_press_event(GtkWidget* widget, GdkEventButton* event, ColorPicker* instance);
            static void on_render_area_button_release_event(GtkWidget* widget, GdkEventButton* event, ColorPicker* instance);

            Frame* _main;
            GLArea* _render_area;

            Shader* _hue_bar_shader;
            Shape* _hue_bar_shape;
            Shape* _hue_bar_frame;

            Shape* _hue_bar_cursor;
            Shape* _hue_bar_cursor_frame;
            Shape* _hue_bar_cursor_window;
            Shape* _hue_bar_cursor_window_frame;

            Shader* _hsv_shape_shader;
            Shape* _hsv_shape;
            Shape* _hsv_shape_frame;

            Shape* _hsv_shape_cursor;
            Shape* _hsv_shape_cursor_frame;

            Shape* _hsv_shape_cursor_window;
            Shape* _hsv_shape_cursor_window_frame;

            Vector2f* _canvas_size;
            HSVA* _current_color_hsva;
            Vector2f* _square_top_left;
            Vector2f* _square_size;

            void reformat();
    };
}

// ###

namespace mousetrap
{
    ColorPicker::ColorPicker()
    {
        _hue_bar_shader = new Shader();
        _render_area = new GLArea();

        _render_area->connect_signal("realize", on_render_area_realize, this);
        _render_area->connect_signal("resize", on_render_area_resize, this);

        /*
        _render_area->add_events(
            GDK_POINTER_MOTION_MASK,
            GDK_BUTTON_PRESS_MASK,
            GDK_BUTTON_RELEASE_MASK
        );

        _render_area->connect_signal("motion-notify-event", on_render_area_motion_notify_event, this);
        _render_area->connect_signal("button-press-event", on_render_area_motion_notify_event, this);
        _render_area->connect_signal("button-release-event", on_render_area_motion_notify_event, this);
        */

        _main = new Frame(1);
        _main->add(_render_area);
    }

    ColorPicker::~ColorPicker()
    {
        std::cerr << "In ColorPicker::~ColorPicker: TODO" << std::endl;
    }

    GtkWidget* ColorPicker::get_native()
    {
        return _main->get_native();
    }

    void ColorPicker::on_render_area_realize(GtkGLArea* area, ColorPicker* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_hue_bar_shape = new Shape();
        instance->_hue_bar_frame = new Shape();

        instance->_hue_bar_cursor = new Shape();
        instance->_hue_bar_cursor_frame = new Shape();

        instance->_hue_bar_cursor_window = new Shape();
        instance->_hue_bar_cursor_window_frame = new Shape();

        instance->_hsv_shape = new Shape();
        instance->_hsv_shape_frame = new Shape();

        instance->_hsv_shape_cursor = new Shape();
        instance->_hsv_shape_cursor_frame = new Shape();

        instance->_hsv_shape_cursor_window = new Shape();
        instance->_hsv_shape_cursor_window_frame = new Shape();

        instance->_hue_bar_shader = new Shader();
        instance->_hue_bar_shader->create_from_file(get_resource_path() + "shaders/color_picker_hue_gradient.frag", ShaderType::FRAGMENT);

        instance->_hsv_shape_shader = new Shader();
        instance->_hsv_shape_shader->create_from_file(get_resource_path() + "shaders/color_picker_hsv_square.frag", ShaderType::FRAGMENT);

        instance->_canvas_size = new Vector2f{1, 1};
        instance->_current_color_hsva = new HSVA();
        instance->_square_top_left = new Vector2f(0, 0);
        instance->_square_size = new Vector2f(1, 1);

        auto* self = instance->_render_area;

        auto hue_bar_task = RenderTask(instance->_hue_bar_shape, instance->_hue_bar_shader);
        hue_bar_task.register_int("_vertical", new int(1));
        hue_bar_task.register_color("_current_color_hsva", instance->_current_color_hsva);
        self->add_render_task(hue_bar_task);

        self->add_render_task(instance->_hue_bar_frame);

        self->add_render_task(instance->_hue_bar_cursor);
        self->add_render_task(instance->_hue_bar_cursor_frame);

        self->add_render_task(instance->_hue_bar_cursor_window);
        self->add_render_task(instance->_hue_bar_cursor_window_frame);

        auto hsv_shape_task = RenderTask(instance->_hsv_shape, instance->_hsv_shape_shader);
        hsv_shape_task.register_color("_current_color_hsva", instance->_current_color_hsva);
        hsv_shape_task.register_vec2("_square_top_left", instance->_square_top_left);
        hsv_shape_task.register_vec2("_square_size", instance->_square_size);

        self->add_render_task(hsv_shape_task);

        self->add_render_task(instance->_hsv_shape_frame);

        /*
        self->add_render_task(instance->_hsv_shape_cursor);
        self->add_render_task(instance->_hsv_shape_cursor_frame);

        self->add_render_task(instance->_hsv_shape_cursor_window);
        self->add_render_task(instance->_hsv_shape_cursor_window_frame);
         */

        gtk_gl_area_queue_render(area);
    }

    void ColorPicker::on_render_area_resize(GtkGLArea* area, int w, int h, ColorPicker* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_canvas_size->x = w;
        instance->_canvas_size->y = h;

        instance->reformat();

        gtk_gl_area_queue_render(area);
    }

    void ColorPicker::reformat()
    {
        float x_margin = state::margin_unit / _canvas_size->x;
        float y_margin = state::margin_unit / _canvas_size->y;

        float hsv_shape_size = 1;
        float cursor_size = 0.1;

        _hue_bar_shape->as_rectangle(
                {0, 0},
                {cursor_size, hsv_shape_size}
        );

        float hsv_shape_x = _hue_bar_shape->get_top_left().x + _hue_bar_shape->get_size().x + 1.5 * x_margin;
        _hsv_shape->as_rectangle(
            {hsv_shape_x, 0},
            {1 - hsv_shape_x, hsv_shape_size}
        );

        _hsv_shape_cursor->as_rectangle({0, 0}, {cursor_size, cursor_size * (_canvas_size->x / _canvas_size->y)});
        _hsv_shape_cursor_window->as_rectangle({0, 0}, {
            _hsv_shape_cursor->get_size().x * 0.8,
            _hsv_shape_cursor->get_size().y * 0.8
        });

        _hue_bar_cursor->as_rectangle({0, 0}, _hsv_shape_cursor->get_size());
        _hue_bar_cursor_window->as_rectangle({0, 0}, {
                _hsv_shape_cursor_window->get_size()
        });

        _hue_bar_cursor->set_centroid(_hue_bar_shape->get_centroid());
        _hue_bar_cursor_window->set_centroid(_hue_bar_cursor->get_centroid());

        _hsv_shape_cursor->set_centroid(_hsv_shape->get_centroid());
        _hsv_shape_cursor_window->set_centroid(_hsv_shape_cursor->get_centroid());

        _hue_bar_frame->as_wireframe(*_hue_bar_shape);
        _hue_bar_cursor_frame->as_wireframe(*_hue_bar_cursor);
        _hue_bar_cursor_window_frame->as_wireframe(*_hue_bar_cursor_window);

        _hsv_shape_frame->as_wireframe(*_hsv_shape);
        _hsv_shape_cursor_frame->as_wireframe(*_hsv_shape_cursor);
        _hsv_shape_cursor_window_frame->as_wireframe(*_hsv_shape_cursor_window);

        for (auto* frame : {_hue_bar_frame, _hue_bar_cursor_frame, _hue_bar_cursor_window_frame, _hsv_shape_frame, _hsv_shape_cursor_frame, _hsv_shape_cursor_window_frame})
            frame->set_color(RGBA(0, 0, 0, 1));

        float ratio = _hsv_shape->get_size().y / _hsv_shape->get_size().x;
        gtk_aspect_frame_set(GTK_ASPECT_FRAME(_main->get_native()), 0.5, 0.5, ratio, false);

        *_square_top_left = _hsv_shape->get_top_left();
        *_square_size = _hsv_shape->get_size();
        update();
    }

    void ColorPicker::update()
    {
        update(state::primary_color);
    }

    void ColorPicker::update(HSVA color)
    {
        *_current_color_hsva = color;

        _hsv_shape_cursor_window->set_color(HSVA(color.h, color.s, color.v, 1));
        _hue_bar_cursor_window->set_color(HSVA(color.h, 1, 1, 1));

        _render_area->queue_render();
    }

    void ColorPicker::on_render_area_button_press_event(GtkWidget* widget, GdkEventButton* event, ColorPicker* instance)
    {}

    void ColorPicker::on_render_area_button_release_event(GtkWidget* widget, GdkEventButton* event, ColorPicker* instance)
    {}

    void ColorPicker::on_render_area_motion_notify_event(GtkWidget * widget, GdkEventMotion * event,
                                                         ColorPicker * instance)
    {}
}
