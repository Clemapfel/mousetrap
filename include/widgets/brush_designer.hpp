// 
// Copyright 2022 Clemens Cords
// Created on 8/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>

namespace mousetrap
{
    class BrushDesigner : public Widget
    {
        public:
            BrushDesigner(size_t n_cols, size_t n_rows);
            ~BrushDesigner();

            GtkWidget* get_native() {
                return _area->get_native();
            }

        private:
            static void on_button_press(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance);
            static void on_pointer_motion(GtkWidget *widget, GdkEventMotion *event, BrushDesigner* instance);
            static void on_button_release(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance);

            bool _button_active = false;

            struct RenderArea : public GLArea
            {
                RenderArea(size_t n_rows, size_t n_cols);
                ~RenderArea();

                void on_realize(GtkGLArea*);
                void on_resize(GtkGLArea*, int, int);

                void switch_shape(Vector2f cursor_position);

                size_t _w, _h;
                Vector2f _size = {1, 1};

                std::vector<Shape*> _squares;
                std::vector<Shape*> _lines;
            };

            RenderArea* _area;
    };
}

// ###

namespace mousetrap
{
    BrushDesigner::RenderArea::RenderArea(size_t n_rows, size_t n_cols)
        : _w(n_rows), _h(n_cols)
    {}

    void BrushDesigner::RenderArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        if (_w == 0 or _h == 0)
            return;

        Vector2f size = {1 / float(_w), 1 / float(_h)};

        _squares.reserve(_w * _h);

        for (size_t y = 0; y < _h; ++y)
        {
            for (size_t x = 0; x < _w; ++x)
            {
                _squares.push_back(new Shape());
                _squares.back()->as_rectangle({x * size.x, y * size.y}, {size.x, size.y});
                _squares.back()->set_color(RGBA(0, 0, 0, 0));
            }
        }

        _lines.push_back(new Shape());
        _lines.back()->as_line({0, 0}, {0, 1});

        _lines.push_back(new Shape());
        _lines.back()->as_line({1, 0}, {1, 1});

        for (size_t i = 0; i < _w; ++i)
        {
            _lines.push_back(new Shape());
            _lines.back()->as_line({i * size.x, 0}, {i * size.x, 1});
        }

        _lines.push_back(new Shape());
        _lines.back()->as_line({0, 0}, {1, 0});

        _lines.push_back(new Shape());
        _lines.back()->as_line({1, 0}, {1, 1});

        for (size_t i = 0; i < _h; ++i)
        {
            _lines.push_back(new Shape());
            _lines.back()->as_line({0, i * size.y}, {1, i * size.y});
        }

        for (auto* s : _squares)
            add_render_task(s);

        for (auto* l : _lines)
        {
            l->set_color(RGBA(0, 0, 0, 1));
            add_render_task(l);
        }
    }

    void BrushDesigner::RenderArea::on_resize(GtkGLArea* area, int w, int h)
    {
        _size = Vector2f(w, h);
        queue_render();
    }

    BrushDesigner::RenderArea::~RenderArea()
    {
        for (auto* s : _squares)
            delete s;

        for (auto* l : _lines)
            delete l;
    }

    void BrushDesigner::RenderArea::switch_shape(Vector2f cursor_position)
    {
        cursor_position /= _size;

        float x_bounds = 1.f / _w;
        float y_bounds = 1.f / _h;

        for (auto* s : _squares)
        {
            auto top_left = s->get_vertex_position(0);

            if (cursor_position.x >= top_left.x and cursor_position.x <= top_left.x + x_bounds and
                cursor_position.y >= top_left.y and cursor_position.y <= top_left.y + y_bounds)
            {

                auto color = s->get_vertex_color(0);
                color.a = 1 - color.a;
                s->set_color(color);
                return;
            }
        }
    }

    void BrushDesigner::on_button_press(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance)
    {
        if (event->button == 1)
        {
            instance->_button_active = true;
            Vector2f pos = {event->x, event->y};
            instance->_area->switch_shape(pos);
        }
    }

    void BrushDesigner::on_button_release(GtkWidget* widget, GdkEventButton* event, BrushDesigner* instance)
    {
        if (event->button == 1)
        {
            instance->_button_active = false;
        }
    }

    void BrushDesigner::on_pointer_motion(GtkWidget* widget, GdkEventMotion* event, BrushDesigner* instance)
    {
        if (instance->_button_active)
        {
            Vector2f pos = {event->x, event->y};
            instance->_area->switch_shape(pos);
        }
    }

    BrushDesigner::BrushDesigner(size_t n_cols, size_t n_rows)
    {
        _area = new RenderArea(n_cols, n_rows);

        _area->set_hexpand(false);
        _area->set_vexpand(false);
        _area->set_size_request({n_cols * 10, n_rows * 10});

        _area->add_events(GDK_BUTTON_PRESS_MASK);
        _area->add_events(GDK_BUTTON_RELEASE_MASK);
        _area->add_events(GDK_POINTER_MOTION_MASK);

        //_area->connect_signal("motion-notify-event", on_pointer_motion, this);
        _area->connect_signal("button-press-event", on_button_press, this);
        _area->connect_signal("button-release-event", on_button_release, this);
    }

    BrushDesigner::~BrushDesigner()
    {
        delete _area;
    }
}