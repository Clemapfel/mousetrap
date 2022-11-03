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

            class PixelHighlightLayer : public CanvasLayer
            {
                public:
                    PixelHighlightLayer(Canvas* owner);
                    ~PixelHighlightLayer();

                    operator Widget*();
                    void update();

                private:
                    RGBA cursor_color = state::settings_file->get_value_as<RGBA>("canvas", "cursor_color");
                    GLArea _area;

                    Shape* _outline_shape;
                    Shape* _fill_shape;

                    Vector2f* _canvas_size = new Vector2f(1, 1);

                    static void on_realize(Widget*, PixelHighlightLayer* instance);
                    static void on_resize(GLArea*, int, int, PixelHighlightLayer* instance);

                    MotionEventController _motion_controller;
                    static void on_motion(MotionEventController*, double x, double y, PixelHighlightLayer* instance);
            };
            PixelHighlightLayer _pixel_highlight_layer = PixelHighlightLayer(this);

            // TOOL: SHAPES

            class ShapeToolLayer : public CanvasLayer
            {
                public:
                    ShapeToolLayer(Canvas*);
                    ~ShapeToolLayer();

                    operator Widget*();

                private:
                    float _margin = state::margin_unit; // in px
                    Vector2f _centroid = Vector2f(0.5, 0.5);
                    Vector2f _size = Vector2f(0.75, 0.75);
                    bool _initialized = false;

                    void reformat();

                    GLArea _area;
                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_realize(Widget*, ShapeToolLayer* instance);
                    static void on_resize(GLArea*, int, int, ShapeToolLayer* instance);

                    std::vector<Shape*> _edges;
                    Shape* _circle;
            };

            ShapeToolLayer* _shape_tool_layer;

            // main
            Overlay _render_area_overlay;
    };
}

#include <app/canvas/transparency_tiling_area.hpp>
#include <app/canvas/grid_layer.hpp>
#include <app/canvas/layers_layer.hpp>
#include <app/canvas/pixel_highlight_layer.hpp>

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

        _edges.clear();

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
        auto m = _margin;
        auto w = _size.x;
        auto h = _size.y;

        _area.make_current();

        size_t count = 0;
        auto add = [&](float a_x, float a_y, float b_x, float b_y) -> Shape* {

            Shape* out;
            if (not _initialized)
                out = _edges.emplace_back(new Shape());
            else
                out = _edges.at(count);

            count += 1;
            out->as_line(Vector2f(a_x, a_y), Vector2f(b_x, b_y));
        };

        // center cross

        auto c = Vector2f(x + 0.5 * w, y + 0.5 * h);

        add(c.x, c.y, c.x + 0, c.y - m);
        add(c.x, c.y, c.x + m, c.y + 0);
        add(c.x, c.y, c.x + 0, c.y - m);
        add(c.x, c.y, c.x - m, c.y + 0);

        // top left square
        add(x, y, x + m, y);
        add(x + m, y, x + m, y + m);
        add(x + m, y + m, x, y + m);
        add(x, y + m, x, y);

        // top right square
        add(x + w - m, y, x + w, y);
        add(x + w, y, x + w, y + m);
        add(x + w, y + m, x + w - m, y + m);
        add(x + w - m, y + m, x + w - m, y);

        // bottom right square
        add(x + w - m, y + h - m, x + w, y + h - m);
        add(x + w, y + h - m, x + w, y + h);
        add(x + w, y + h, x + w - m, y + h);
        add(x + w - m, y + h, x + w - m, y + h - m);

        // bottom left square
        add(x, y + h - m, x + m, y + h - m);
        add(x + m, y + h - m, y + m, y + h);
        add(x + m, y + h, x, y + h);
        add(x, y + h, x, y + h - m);

        // outline
        add(x + m, y, x + w - m, y);
        add(x + w, y + m, x + w, y + h - m);
        add(x + w - m, y + h, x + m, y + h);
        add(x, y + h - m, x, y + m);

        // half margin outline
        add(x + m, y + 0.5 * m, x + w - m, y + 0.5 * m);
        add(x + w - 0.5 * m, y + m, x + w - 0.5 * m, y + h - m);
        add(x + w - m, y + h - 0.5 * m, x + m, y + h - 0.5 * m);
        add(x + 0.5 * m, y + h - m, x + 0.5 * m, y + m);

        // circlefir
        _circle->as_ellipse(c, w, h, 32);
    }

    // CANVAS

    Canvas::Canvas()
    {
        _render_area_overlay.set_child(_transparency_tiling_layer);
        _render_area_overlay.add_overlay(_layers_layer);
        _render_area_overlay.add_overlay(_grid_layer);
        _render_area_overlay.add_overlay(_pixel_highlight_layer);

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
        _pixel_highlight_layer.update();

        for (auto* widget : {
                _transparency_tiling_layer.operator Widget *(),
                _layers_layer.operator Widget *(),
                _grid_layer.operator Widget *(),
                _pixel_highlight_layer.operator Widget*()
        })
            ((GLArea*) widget)->queue_render();
    }

    void Canvas::set_current_pixel_position(int x, int y)
    {
        _selected_pixel = {x, y};
    }

    void Canvas::update()
    {
        return;
    }
}