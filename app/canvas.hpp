// 
// Copyright 2022 Clemens Cords
// Created on 10/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    class Canvas : public AppComponent
    {
        public:
            Canvas();
            operator Widget*();
            void update();

            void set_transform_offset(float x, float y);
            void set_transform_scale(float);

            void set_current_pixel_position(int x, int y);
            void update_brush_cursor();

        private:
            Vector2f _transform_offset = {0, 0};
            float _transform_scale = 1;

            void signal_transform_updated();
            GLTransform* _transform = new GLTransform();

            Vector2ui _selected_pixel = {0, 0};

            Vector2f align_with_pixel_grid(const Vector2f& edited);

            struct CanvasLayer
            {
                public:
                    CanvasLayer(Canvas* owner)
                        : _owner(owner)
                    {};

                    virtual operator Widget*() = 0;

                protected:
                    Canvas* _owner;
            };

            // TRANSPARENCY TILING

            class TransparencyTilingLayer : public CanvasLayer
            {
                public:
                    TransparencyTilingLayer(Canvas*);
                    ~TransparencyTilingLayer();

                    operator Widget*() override;

                private:
                    GLArea _area;
                    Shape* _shape = nullptr;

                    Shape* _subtract_top = nullptr;
                    Shape* _subtract_right = nullptr;
                    Shape* _subtract_bottom = nullptr;
                    Shape* _subtract_left = nullptr;

                    static inline Shader* _shader = nullptr;

                    Vector2f* _canvas_size = new Vector2f(1, 1);

                    void reformat();

                    static void on_realize(Widget*, TransparencyTilingLayer* instance);
                    static void on_resize(GLArea*, int, int, TransparencyTilingLayer* instance);
            };
            TransparencyTilingLayer _transparency_tiling_layer = TransparencyTilingLayer(this);

            // GRID

            class GridLayer : public CanvasLayer
            {
                public:
                    GridLayer(Canvas*);
                    ~GridLayer();

                    operator Widget*() override;

                    void set_grid_color(RGBA);

                private:
                    RGBA grid_color = state::settings_file->get_value_as<RGBA>("canvas", "grid_color");

                    GLArea _area;
                    std::vector<Shape*> _h_lines;
                    std::vector<Shape*> _v_lines;

                    Vector2f* _canvas_size = new Vector2f(1, 1);

                    void reformat();

                    static void on_realize(Widget*, GridLayer* instance);
                    static void on_resize(GLArea*, int, int, GridLayer* instance);
            };
            GridLayer _grid_layer = GridLayer(this);

            bool _grid_visible = state::settings_file->get_value_as<bool>("canvas", "grid_visible");

            // LAYERS

            class LayersLayer : public CanvasLayer
            {
                public:
                    LayersLayer(Canvas* owner);
                    ~LayersLayer();

                    operator Widget*();

                private:
                    void initialize();
                    void reformat();

                    GLArea _area;
                    std::vector<Shape*> _layer_shapes;

                    Vector2f* _canvas_size = new Vector2f(1, 1);

                    static void on_realize(Widget*, LayersLayer* instance);
                    static void on_resize(GLArea*, int, int, LayersLayer* instance);
            };
            LayersLayer _layers_layer = LayersLayer(this);

            // PIXEL HIGHLIGHT

            class BrushCursorLayer : public CanvasLayer
            {
                public:
                    BrushCursorLayer(Canvas* owner);
                    ~BrushCursorLayer();

                    operator Widget*();
                    void update();

                private:
                    float brush_cursor_color_alpha = state::settings_file->get_value_as<float>("canvas", "brush_cursor_color_alpha");
                    GLArea _area;

                    float* _timer = new float(0); // seconds
                    Shader* _brush_outline_shader = nullptr;
                    Shape* _cursor_shape = nullptr;

                    Shape* _cursor_outline_shape_hlines = nullptr;
                    Shape* _cursor_outline_shape_vlines = nullptr;

                    Vector2f* _canvas_size = new Vector2f(1, 1);
                    Vector2f* _cursor_position = new Vector2f(0, 0);

                    Overlay _overlay;

                    static void on_realize(Widget*, BrushCursorLayer* instance);
                    static void on_resize(GLArea*, int, int, BrushCursorLayer* instance);

                    MotionEventController _motion_controller;
                    static void on_motion_enter(MotionEventController*, double x, double y, BrushCursorLayer* instance);
                    static void on_motion_leave(MotionEventController*, BrushCursorLayer* instance);
                    static void on_motion(MotionEventController*, double x, double y, BrushCursorLayer* instance);
            };
            BrushCursorLayer _brush_cursor_layer = BrushCursorLayer(this);

            // TOOL: SHAPES

            class ShapeToolLayer : public CanvasLayer
            {
                public:
                    ShapeToolLayer(Canvas*);
                    ~ShapeToolLayer();

                    operator Widget*();

                private:
                    float _margin = state::margin_unit * 2; // in px
                    Vector2f _centroid = Vector2f(0.5, 0.5); // in gl coords
                    Vector2f _size = Vector2f(0.3, 0.7); // in gl coords
                    bool _initialized = false;

                    void reformat();

                    GLArea _area;
                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_realize(Widget*, ShapeToolLayer* instance);
                    static void on_resize(GLArea*, int, int, ShapeToolLayer* instance);

                    std::vector<Shape*> _edges;
                    Shape* _circle = nullptr;
            };

            // main
            Overlay _render_area_overlay;
    };
}

#include <app/canvas/transparency_tiling_area.hpp>
#include <app/canvas/grid_layer.hpp>
#include <app/canvas/layers_layer.hpp>
#include <app/canvas/brush_cursor_area.hpp>

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

    // CANVAS

    Canvas::Canvas()
    {
        _render_area_overlay.set_child(_transparency_tiling_layer);
        _render_area_overlay.add_overlay(_layers_layer);
        _render_area_overlay.add_overlay(_grid_layer);
        _render_area_overlay.add_overlay(_brush_cursor_layer);

        _grid_layer.operator Widget*()->set_visible(_grid_visible);

        set_transform_scale(12);
        _render_area_overlay.set_cursor(GtkCursorType::NONE);
    }

    Canvas::operator Widget*()
    {
        return &_render_area_overlay;
    }

    Vector2f Canvas::align_with_pixel_grid(const Vector2f& edited)
    {
        auto pos = edited;
        pos *= _render_area_overlay.get_size();
        pos.x = round(pos.x);
        pos.y = round(pos.y);
        pos /= _render_area_overlay.get_size();
        return pos;
    }

    void Canvas::set_transform_scale(float scale)
    {
        _transform_scale = scale;
        _transform->reset();
        _transform->translate(_transform_offset);
        _transform->scale(_transform_scale, _transform_scale);

        signal_transform_updated();
    }

    void Canvas::signal_transform_updated()
    {
        _brush_cursor_layer.update();

        for (auto* widget : {
                _transparency_tiling_layer.operator Widget *(),
                _layers_layer.operator Widget *(),
                _grid_layer.operator Widget *(),
                _brush_cursor_layer.operator Widget*()
        })
            ((GLArea*) widget)->queue_render();
    }

    void Canvas::set_current_pixel_position(int x, int y)
    {
        _selected_pixel = {x, y};
    }

    void Canvas::update_brush_cursor()
    {
        _brush_cursor_layer.update();
    }

    void Canvas::update()
    {
        return;
    }
}