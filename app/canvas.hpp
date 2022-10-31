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

        private:
            Vector2f _transform_offset = {0, 0};
            float _transform_scale = 1;

            void signal_transform_updated();
            GLTransform* _transform = new GLTransform();

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

            // main
            Overlay _render_area_overlay;
    };
}

#include <app/canvas/transparency_tiling_area.hpp>
#include <app/canvas/grid_layer.hpp>

namespace mousetrap
{
    Canvas::LayersLayer::LayersLayer(Canvas* owner)
        : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
    }

    Canvas::LayersLayer::operator Widget*()
    {
        return &_area;
    }

    Canvas::LayersLayer::~LayersLayer()
    {
        for (auto* shape : _layer_shapes)
            delete shape;

        delete _canvas_size;
    }

    void Canvas::LayersLayer::on_realize(Widget* widget, LayersLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        size_t frame_i = state::current_frame;

        for (auto& layer : state::layers)
            instance->_layer_shapes.emplace_back(new Shape());

        instance->reformat();

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto task = RenderTask(
                instance->_layer_shapes.at(layer_i),
                nullptr,
                instance->_owner->_transform,
                state::layers.at(layer_i)->blend_mode
            );

            instance->_area.add_render_task(task);
        }

        area->queue_render();
    }

    void Canvas::LayersLayer::on_resize(GLArea* area, int w, int h, LayersLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
    }

    void Canvas::LayersLayer::reformat()
    {
        float width = state::layer_resolution.x / _canvas_size->x;
        float height = state::layer_resolution.y / _canvas_size->y;

        size_t frame_i = state::current_frame;
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto& layer = state::layers.at(layer_i);
            auto& shape = _layer_shapes.at(layer_i);

            shape->as_rectangle(
                {0.5 - width / 2, 0.5 - height / 2},
                {width, height}
            );
            shape->set_texture(layer->frames.at(frame_i).texture);
        }

        _area.queue_render();
    }

    // CANVAS

    Canvas::Canvas()
    {
        _transform->scale(13, 13);

        _render_area_overlay.set_child(_transparency_tiling_layer);
        _render_area_overlay.add_overlay(_layers_layer);
        _render_area_overlay.add_overlay(_grid_layer);

        _grid_layer.operator Widget*()->set_visible(_grid_visible);
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
        for (auto* widget : {
                _transparency_tiling_layer.operator Widget *(),
                _layers_layer.operator Widget *(),
                _grid_layer.operator Widget *()
        })
            ((GLArea*) widget)->queue_render();
    }

    void Canvas::update()
    {
        return;
    }
}