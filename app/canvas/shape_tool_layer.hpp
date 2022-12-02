// 
// Copyright 2022 Clemens Cords
// Created on 12/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    Canvas::ShapeToolLayer::ShapeToolLayer(Canvas* owner)
            : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
    }

    Canvas::ShapeToolLayer::~ShapeToolLayer()
    {
        delete _canvas_size;
        for (auto* s : _edges)
            delete s;

        delete _circle;
    };

    Canvas::ShapeToolLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::ShapeToolLayer::on_resize(GLArea* area, int w, int h, ShapeToolLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
        area->queue_render();
    }

    void Canvas::ShapeToolLayer::on_realize(Widget* widget, ShapeToolLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        for (auto* s : instance->_edges)
            delete s;
        delete instance->_circle;

        instance->_edges.clear();

        instance->_initialized = false;
        instance->reformat();
        instance->_initialized = true;

        area->clear_render_tasks();

        for (auto* s : instance->_edges)
            area->add_render_task(s);

        area->add_render_task(instance->_circle);
        area->queue_render();
    }

    void Canvas::ShapeToolLayer::reformat()
    {
        auto x = _centroid.x - _size.x;
        auto y = _centroid.y - _size.y;
        auto xm = _margin / _canvas_size->x;
        auto ym = _margin / _canvas_size->y;
        auto ixm = 1 * xm;
        auto iym = 1 * ym;
        auto w = _size.x;
        auto h = _size.y;

        _area.make_current();

        size_t count = 0;
        auto add = [&](float a_x, float a_y, float b_x, float b_y) -> void {

            Shape* out;
            if (not _initialized)
                out = _edges.emplace_back(new Shape());
            else
                out = _edges.at(count);

            count += 1;

            Vector2f a = {a_x, a_y};
            a *= *_canvas_size;
            a.x = round(a.x);
            a.y = round(a.y);
            a /= *_canvas_size;

            Vector2f b = {b_x, b_y};
            b *= *_canvas_size;
            b.x = round(b.x);
            b.y = round(b.y);
            b /= *_canvas_size;

            out->as_line(Vector2f(a_x, a_y), Vector2f(b_x, b_y));
        };

        // center cross
        auto c = Vector2f(x + 0.5 * w, y + 0.5 * h);

        add(c.x, c.y, c.x + 0, c.y - iym);
        add(c.x, c.y, c.x + ixm, c.y + 0);
        add(c.x, c.y, c.x + 0, c.y + iym);
        add(c.x, c.y, c.x - ixm, c.y + 0);

        // top left square
        add(x, y, x + xm, y);
        add(x + xm, y, x + xm, y + ym);
        add(x + xm, y + ym, x, y + ym);
        add(x, y + ym, x, y);

        // top right square
        add(x + w - xm, y, x + w, y);
        add(x + w, y, x + w, y + ym);
        add(x + w, y + ym, x + w - xm, y + ym);
        add(x + w - xm, y + ym, x + w - xm, y);

        // bottom right square
        add(x + w - xm, y + h - ym, x + w, y + h - ym);
        add(x + w, y + h - ym, x + w, y + h);
        add(x + w, y + h, x + w - xm, y + h);
        add(x + w - xm, y + h, x + w - xm, y + h - ym);

        // bottom left square
        add(x, y + h - ym, x + xm, y + h - ym);
        add(x + xm, y + h - ym, x + xm, y + h);
        add(x + xm, y + h, x, y + h);
        add(x, y + h, x, y + h - ym);

        // outline
        add(x + xm, y, x + w - xm, y);
        add(x + w, y + ym, x + w, y + h - ym);
        add(x + w - xm, y + h, x + xm, y + h);
        add(x, y + h - ym, x, y + ym);

        // half margin outline
        add(x + xm, y + iym, x + w - xm, y + iym);
        add(x + w - ixm, y + ym, x + w - ixm, y + h - ym);
        add(x + w - xm, y + h - iym, x + xm, y + h - iym);
        add(x + ixm, y + h - ym, x + ixm, y + ym);

        for (auto* s : _edges)
            s->set_color(RGBA(1, 0, 1, 1));

        // circle
        if (not _initialized)
            _circle = new Shape();

        std::vector<Vector2f> ellipse_points;

        const float step = 360.f / 64;
        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            ellipse_points.emplace_back(
                    c.x + cos(as_radians) * _size.x * 0.5,
                    c.y + sin(as_radians) * _size.y * 0.5
            );
        }

        _circle->as_wireframe(ellipse_points);
    }
}