//
// Copyright 2022 Clemens Cords
// Created on 10/12/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/shape.hpp>
#include <include/gl_area.hpp>
#include <include/overlay.hpp>
#include <include/get_resource_path.hpp>
#include <include/aspect_frame.hpp>

#include <app/app_component.hpp>
#include <app/global_state.hpp>

#include <deque>

namespace mousetrap
{
    class Canvas : public AppComponent
    {
        public:
            Canvas();
            operator Widget*();
            void update() override;

        private:
            class RenderArea
            {
                public:
                    RenderArea(Canvas*);
                    operator Widget*();

                    void set_transform_offset(float x, float y);
                    void set_transform_scale(float );

                    Vector2f get_transform_offset();
                    float get_transform_scale();

                    Vector2f get_size();
                    Vector2f get_layer_shape_size(); // in px
                    void update_autohide_grid();

                    // in pre-transform, widget dims
                    void set_pixel_highlight_position(float x, float y);

                private:
                    Canvas* _owner;

                    Vector2f* _canvas_size = new Vector2f(state::layer_resolution);
                    GLTransform* _transform = new GLTransform();
                    Vector2f _transform_offset = {0, 0};
                    float _transform_scale = 1;
                    void update_transform();

                    // Grid

                    static inline RGBA _grid_color = RGBA(1, 1, 1, 0.5);
                    std::vector<Shape*> _grid_shapes;

                    // Layers

                    struct LayerShape
                    {
                        std::vector<Shape*> frames;
                    };

                    GLArea _layer_area;
                    std::deque<LayerShape*> _layer_shapes;

                    static void on_layer_area_realize(Widget*, RenderArea* instance);
                    static void on_layer_area_resize(GLArea*, int, int, RenderArea* instance);

                    void schedule_layer_render_tasks();

                    // Transparency Tiling

                    struct SubtractOverlay
                    {
                        Shape* top;
                        Shape* right;
                        Shape* bottom;
                        Shape* left;
                    };

                    GLArea _transparency_area;
                    Shape* _transparency_shape;
                    SubtractOverlay _transparency_subtract_overlay;

                    static inline Shader* _transparency_shader = nullptr;

                    static void on_transparency_area_realize(Widget*, RenderArea* instance);
                    static void on_transparency_area_resize(GLArea*, int, int, RenderArea* instance);

                    // Pixel Highlight

                    MotionEventController _pixel_highlight_controller;
                    static void on_pixel_highlight_controller_motion(MotionEventController*, double x, double y, RenderArea* instance);
                    Shape* _pixel_highlight_shape = nullptr;

                    // main

                    Overlay _overlay;
            };

            RenderArea _render_area;

            // Scroll

            static inline const float adjustment_page_size = 1;

            static inline bool h_scroll_inverted = false;
            static inline bool v_scroll_inverted = true;
            static inline float scroll_speed = 10; // 1 * <scroll distance in px> => <scroll_speed> * <transform_offset in px>

            static inline const float adjustment_lower = -0.5 * adjustment_page_size;
            static inline const float adjustment_upper = +0.5 * adjustment_page_size;

            Adjustment _render_area_h_adjustment = Adjustment(0, adjustment_lower, adjustment_upper, 0, 0, adjustment_page_size);
            Adjustment _render_area_v_adjustment = Adjustment(0, adjustment_lower, adjustment_upper, 0, 0, adjustment_page_size);

            static void on_render_area_h_adjustment_value_changed(Adjustment* adjustment, Canvas* instance);
            static void on_render_area_v_adjustment_value_changed(Adjustment* adjustment, Canvas* instance);

            Scrollbar _render_area_h_scroll = Scrollbar(_render_area_h_adjustment, GTK_ORIENTATION_HORIZONTAL);
            Scrollbar _render_area_v_scroll = Scrollbar(_render_area_v_adjustment, GTK_ORIENTATION_VERTICAL);

            ScrollEventController _scroll_event_controller;
            static void on_scroll_event_controller_scroll(ScrollEventController*, double x, double y, Canvas* instance);
            void update_scrollbars();

            // Zoom

            Scale _zoom_scale = Scale(1, (1920 / std::min(state::layer_resolution.x, state::layer_resolution.y)) * 2.1, 1);
            static void on_zoom_scale_value_changed(Scale*, Canvas* instance);

            KeyEventController _key_event_controller;
            bool _control_active = false;
            static bool on_key_event_controller_key_pressed(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, Canvas* instance);
            static bool on_key_event_controller_key_released(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, Canvas* instance);

            // Layout

            Box _render_area_and_v_scrollbar = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _render_area_and_h_scrollbar = Box(GTK_ORIENTATION_VERTICAL);

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}

///

namespace mousetrap
{
    void Canvas::RenderArea::on_transparency_area_realize(Widget* widget, RenderArea* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_shader == nullptr)
        {
            _transparency_shader = new Shader();
            _transparency_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_shape = new Shape();
        instance->_transparency_subtract_overlay = SubtractOverlay{
                new Shape(),
                new Shape(),
                new Shape(),
                new Shape()
        };

        on_transparency_area_resize(area, state::layer_resolution.x, state::layer_resolution.y, instance);
        auto transparency_render_task = RenderTask(instance->_transparency_shape, _transparency_shader, nullptr);
        transparency_render_task.register_vec2("_canvas_size", instance->_canvas_size);

        area->add_render_task(transparency_render_task);

        auto& overlay = instance->_transparency_subtract_overlay;
        for (auto* shape : {overlay.top, overlay.bottom, overlay.left, overlay.right})
        {
            shape->set_color(RGBA(0, 0, 0, 0));
            auto task = RenderTask(shape, nullptr, instance->_transform, BlendMode::MULTIPLY);
            area->add_render_task(task);
        }

        area->queue_render();
    }

    void Canvas::RenderArea::on_transparency_area_resize(GLArea* area, int w_in, int h_in, RenderArea* instance)
    {
        area->make_current();

        instance->_canvas_size->x = w_in;
        instance->_canvas_size->y = h_in;

        float width = state::layer_resolution.x / instance->_canvas_size->x;
        float height = state::layer_resolution.y / instance->_canvas_size->y;

        float eps = 0;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = 10e4;
        float b = 10e4;

        instance->_transparency_shape->as_rectangle(
                {0, 0}, {1, 1}
        );

        instance->_transparency_subtract_overlay.top->as_rectangle(
                {x - a, y - b}, {x + w + a, y - b},
                {x - a, y}, {x + w + a, y}
        );

        instance->_transparency_subtract_overlay.bottom->as_rectangle(
                {x - a, y + h}, {x + w + a, y + h},
                {x - a, y + h + b}, {x + w + a, y + h + b}
        );

        instance->_transparency_subtract_overlay.left->as_rectangle(
                {x - a, y}, {x, y},
                {x, y + h}, {x - a, y + h}
        );

        instance->_transparency_subtract_overlay.right->as_rectangle(
                {x + w, y}, {x + w + a, y},
                {x + w + a, y + h}, {x + w, y + h}
        );

        area->queue_render();
    }

    void Canvas::RenderArea::on_layer_area_realize(Widget* widget, RenderArea* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        for (auto& layer : state::layers)
        {
            auto layer_shape = instance->_layer_shapes.emplace_back(new LayerShape());
            for (auto& frame: layer->frames)
                layer_shape->frames.emplace_back(new Shape());
        }

        instance->_pixel_highlight_shape = new Shape();
        on_layer_area_resize(area, state::layer_resolution.x, state::layer_resolution.y, instance);
        // also schedules render tasks

        // initial update of scrollbars on canvas realize:
        instance->_owner->on_zoom_scale_value_changed(&instance->_owner->_zoom_scale, instance->_owner);
    }

    void Canvas::RenderArea::on_layer_area_resize(GLArea* area, int w_in, int h_in, RenderArea* instance)
    {
        instance->_canvas_size->x = w_in;
        instance->_canvas_size->y = h_in;

        float width = state::layer_resolution.x / instance->_canvas_size->x;
        float height = state::layer_resolution.y / instance->_canvas_size->y;

        float eps = 0.001;
        float x = 0.5 - width / 2 + eps;
        float y = 0.5 - height / 2 + eps;
        float w = width - 2 * eps;
        float h = height - 2 * eps;

        float a = (float) std::numeric_limits<int>::max() / 4; // sic
        float b = (float) std::numeric_limits<int>::max() / 4;

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto& layer = state::layers.at(layer_i);
            auto& shape = instance->_layer_shapes.at(layer_i);

            for (size_t frame_i = 0; frame_i < layer->frames.size(); ++frame_i)
            {
                shape->frames.at(frame_i)->as_rectangle(
                        {0.5 - width / 2, 0.5 - height / 2},
                        {width, height}
                );
                shape->frames.at(frame_i)->set_texture(layer->frames.at(frame_i).texture);
            }
        }

        instance->_transparency_shape->as_rectangle(
                {0.5 - width / 2, 0.5 - height / 2},
                {width, height}
        );

        for (auto* line : instance->_grid_shapes)
            delete line;

        instance->_grid_shapes.clear();

        x = instance->_layer_shapes.at(0)->frames.at(0)->get_top_left().x;
        y = instance->_layer_shapes.at(0)->frames.at(0)->get_top_left().y;
        w = instance->_layer_shapes.at(0)->frames.at(0)->get_size().x;
        h = instance->_layer_shapes.at(0)->frames.at(0)->get_size().y;

        for (size_t i = 0; i < state::layer_resolution.x + 1; ++i)
        {
            auto* line = instance->_grid_shapes.emplace_back(new Shape());
            line->as_line(
                    {x + ((float(i) / state::layer_resolution.x) * w), y},
                    {x + ((float(i) / state::layer_resolution.x) * w), y + h}
            );
        }

        for (size_t i = 0; i < state::layer_resolution.y + 1; ++i)
        {
            auto* line = instance->_grid_shapes.emplace_back(new Shape());
            line->as_line(
                    {x, y + ((float(i) / state::layer_resolution.y) * h)},
                    {x + w, y + ((float(i) / state::layer_resolution.y) * h)}
            );
        }

        for (auto* shape : instance->_grid_shapes)
            shape->set_color(_grid_color);

        float pixel_w = width / state::layer_resolution.x * instance->_transform_scale;
        float pixel_h = height / state::layer_resolution.y * instance->_transform_scale;
        instance->_pixel_highlight_shape->as_wireframe({
            {0, 0},
            {pixel_w, 0},
            {pixel_w, pixel_h},
            {0, pixel_h}
        });
        instance->_pixel_highlight_shape->set_color(RGBA{1, 0, 1, 1});
        instance->_pixel_highlight_shape->set_centroid({0.5, 0.5});

        instance->schedule_layer_render_tasks();
        area->queue_render();
    }

    void Canvas::RenderArea::schedule_layer_render_tasks()
    {
        _layer_area.clear_render_tasks();

        size_t frame_i = state::current_frame;
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto task = RenderTask(
                    _layer_shapes.at(layer_i)->frames.at(frame_i),
                    nullptr,
                    _transform,
                    state::layers.at(layer_i)->blend_mode
            );
            _layer_area.add_render_task(task);
        }

        for (auto* shape : _grid_shapes)
        {
            auto task = RenderTask(shape, nullptr, _transform);
            _layer_area.add_render_task(task);
        }

        _layer_area.add_render_task(_pixel_highlight_shape);
        _layer_area.queue_render();
    }

    Canvas::RenderArea::RenderArea(Canvas* owner)
        : _owner(owner)
    {
        _transparency_area.connect_signal_realize(on_transparency_area_realize, this);
        _transparency_area.connect_signal_resize(on_transparency_area_resize, this);
        _layer_area.connect_signal_realize(on_layer_area_realize, this);
        _layer_area.connect_signal_resize(on_layer_area_resize, this);

        for (auto* area : {&_transparency_area, &_layer_area})
        {
            area->set_expand(true);
            area->set_size_request(state::layer_resolution);
        }

        _pixel_highlight_controller.connect_signal_motion(on_pixel_highlight_controller_motion, this);
        _overlay.add_controller(&_pixel_highlight_controller);

        _overlay.set_child(&_transparency_area);
        _overlay.add_overlay(&_layer_area);

        _overlay.set_cursor(GtkCursorType::CELL);
    }

    void Canvas::RenderArea::on_pixel_highlight_controller_motion(
        MotionEventController*,
        double x, double y,
        RenderArea* instance)
    {
        float widget_w = instance->_layer_area.get_size().x;
        float widget_h = instance->_layer_area.get_size().y;

        instance->set_pixel_highlight_position(x / widget_w, y / widget_h);
        instance->_layer_area.queue_render();
    }

    void Canvas::RenderArea::set_pixel_highlight_position(float x, float y)
    {
        Vector2f pos = {x, y};

        // align with texture-space pixel grid
        auto* layer_shape =  _layer_shapes.at(0)->frames.at(0);
        Vector2f layer_top_left = layer_shape->get_top_left();
        layer_top_left = to_gl_position(layer_top_left);
        layer_top_left = _transform->apply_to(layer_top_left);
        layer_top_left = from_gl_position(layer_top_left);

        Vector2f layer_size = layer_shape->get_size();
        layer_size *= _transform_scale;

        float x_dist = abs(pos.x - layer_top_left.x);
        float y_dist = abs(pos.y - layer_top_left.y);

        Vector2f pixel_size = {layer_size.x / state::layer_resolution.x, layer_size.y / state::layer_resolution.y};

        x_dist /= pixel_size.x;
        y_dist /= pixel_size.y;

        x_dist = floor(x_dist);
        y_dist = floor(y_dist);

        pos.x = layer_top_left.x + x_dist * pixel_size.x;
        pos.y = layer_top_left.y + y_dist * pixel_size.y;

        // align with widget-space pixel grid
        pos *= _layer_area.get_size();
        pos.x = round(pos.x);
        pos.y = round(pos.y);
        pos /= _layer_area.get_size();

        if (_pixel_highlight_shape != nullptr)
            _pixel_highlight_shape->set_top_left({pos.x, pos.y});
    }

    Vector2f Canvas::RenderArea::get_size()
    {
        return *_canvas_size;
    }

    Vector2f Canvas::RenderArea::get_layer_shape_size()
    {
        if (_layer_shapes.empty())
            return {1, 1};

        return _layer_shapes.at(0)->frames.at(0)->get_size() * (*_canvas_size) * Vector2f(_transform_scale, _transform_scale);
    }

    void Canvas::RenderArea::set_transform_scale(float scale)
    {
        _transform_scale = scale;

        float width = state::layer_resolution.x / _canvas_size->x;
        float height = state::layer_resolution.y / _canvas_size->y;

        float eps = 0.001;
        float pixel_w = width / state::layer_resolution.x * _transform_scale - eps;
        float pixel_h = height / state::layer_resolution.y * _transform_scale - eps;
        _pixel_highlight_shape->as_wireframe({
               {0, 0},
               {pixel_w, 0},
               {pixel_w, pixel_h},
               {0, pixel_h}
       });
        _pixel_highlight_shape->set_color(RGBA{1, 0, 1, 1});
        _pixel_highlight_shape->set_centroid({0.5, 0.5});

        update_transform();
    }

    void Canvas::RenderArea::set_transform_offset(float x, float y)
    {
        _transform_offset = {x, y};
        update_transform();
    }

    Vector2f Canvas::RenderArea::get_transform_offset()
    {
        return _transform_offset;
    }

    float Canvas::RenderArea::get_transform_scale()
    {
        return _transform_scale;
    }

    void Canvas::RenderArea::update_transform()
    {
        _transform->reset();
        _transform->translate(_transform_offset);
        _transform->scale(_transform_scale, _transform_scale);

        if (_transparency_area.get_is_realized())  // TODO: why is this necessary?
            on_transparency_area_resize(&_transparency_area, _canvas_size->x, _canvas_size->y, this);

        _transparency_area.queue_render();
        _layer_area.queue_render();
    }

    void Canvas::RenderArea::update_autohide_grid()
    {}

    Canvas::RenderArea::operator Widget*()
    {
        return &_overlay;
    }

    void Canvas::on_render_area_h_adjustment_value_changed(Adjustment* adjustment, Canvas* instance)
    {
        instance->_render_area.set_transform_offset(
            (h_scroll_inverted ? -1 : 1) * instance->_render_area_h_adjustment.get_value(),
            (v_scroll_inverted ? -1 : 1) * instance->_render_area_v_adjustment.get_value()
        );
    }

    void Canvas::on_render_area_v_adjustment_value_changed(Adjustment* adjustment, Canvas* instance)
    {
        instance->_render_area.set_transform_offset(
            (h_scroll_inverted ? -1 : 1) * instance->_render_area_h_adjustment.get_value(),
            (v_scroll_inverted ? -1 : 1) * instance->_render_area_v_adjustment.get_value()
        );
    }

    void Canvas::on_scroll_event_controller_scroll(ScrollEventController*, double x, double y, Canvas* instance)
    {
        instance->_render_area.operator Widget*()->grab_focus();

        if (instance->_control_active)
        {
            instance->_zoom_scale.set_value(instance->_zoom_scale.get_value() + y);
            return;
        }

        auto normalized_x = x / instance->_render_area.get_size().x;
        auto normalized_y = y / instance->_render_area.get_size().y;

        if (instance->h_scroll_inverted)
            normalized_x *= -1;

        if (instance->v_scroll_inverted)
            normalized_y *= -1;

        instance->_render_area_h_adjustment.set_value(instance->_render_area_h_adjustment.get_value() + (normalized_x * scroll_speed));
        instance->_render_area_v_adjustment.set_value(instance->_render_area_v_adjustment.get_value() + (normalized_y * scroll_speed));
    }

    void Canvas::update_scrollbars()
    {
        auto canvas_size = _render_area.get_layer_shape_size();
        auto area_size = _render_area.get_size();
        canvas_size /= area_size;

        auto dist = canvas_size.x;
        auto lower = 0 - dist;
        auto upper = 0 + dist;

        _render_area_h_adjustment.configure(0, lower, upper, 0, 0, 0);

        dist = canvas_size.y;
        lower = 0 - dist;
        upper = 0 + dist;

        _render_area_v_adjustment.configure(0, lower, upper, 0, 0, 0);

        // TODO: resize scrollbar cursor
    }

    void Canvas::on_zoom_scale_value_changed(Scale* scale_widget, Canvas* instance)
    {
        instance->_render_area.set_transform_scale(scale_widget->get_value());
        instance->_render_area.update_autohide_grid();
        instance->update_scrollbars();
    }

    bool Canvas::on_key_event_controller_key_pressed(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, Canvas* instance)
    {
        if (keyval == GDK_KEY_Control_L or keyval == GDK_KEY_Control_R)
        {
            instance->_control_active = true;
        }

        return false;
    }

    bool Canvas::on_key_event_controller_key_released(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, Canvas* instance)
    {
        if (keyval == GDK_KEY_Control_L or keyval == GDK_KEY_Control_R)
        {
            instance->_control_active = false;
        }

        return false;
    }

    Canvas::operator Widget*()
    {
        return &_main;
    }

    void Canvas::update()
    {}

    Canvas::Canvas()
        : _render_area(this)
    {
        _zoom_scale.connect_signal_value_changed(on_zoom_scale_value_changed, this);
        _zoom_scale.set_hexpand(true);

        _render_area_h_adjustment.connect_signal_value_changed(on_render_area_h_adjustment_value_changed, this);
        _render_area_v_adjustment.connect_signal_value_changed(on_render_area_v_adjustment_value_changed, this);

        _render_area_h_adjustment.set_value(0);
        _render_area_v_adjustment.set_value(0);

        _render_area_and_v_scrollbar.push_back(_render_area);
        _render_area_and_v_scrollbar.push_back(&_render_area_v_scroll);
        _render_area_and_h_scrollbar.push_back(&_render_area_and_v_scrollbar);
        _render_area_and_h_scrollbar.push_back(&_render_area_h_scroll);

        _render_area_and_h_scrollbar.push_back(&_zoom_scale);

        _scroll_event_controller.connect_signal_scroll(on_scroll_event_controller_scroll, this);
        _key_event_controller.connect_signal_key_pressed(on_key_event_controller_key_pressed, this);
        _key_event_controller.connect_signal_key_released(on_key_event_controller_key_released, this);

        auto* area = _render_area.operator Widget *();
        area->add_controller(&_scroll_event_controller);
        area->add_controller(&_key_event_controller);
        area->set_focusable(true);
        gtk_widget_set_focus_on_click(area->operator GtkWidget*(), true);

        _main.push_back(&_render_area_and_h_scrollbar);
    }

}

