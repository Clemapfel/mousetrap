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

            void draw_brush(Vector2i position, Brush* brush, RGBA color);
            void draw_brush_line(Vector2i a, Vector2i b, Brush* brush, RGBA color);

        private:

            struct CanvasLayer
            {
                public:
                    CanvasLayer(Canvas* owner)
                        : _owner(owner)
                    {};

                    virtual operator Widget*() = 0;
                    virtual void update() = 0;

                protected:
                    Canvas* _owner;
            };

            // is mouse button currently down
            bool* _click_pressed = new bool(false);

            // position in texture space
            Vector2i* _current_pixel_position = new Vector2i(0, 0);

            // position in widget space
            Vector2f* _current_cursor_position = new Vector2f(0, 0);

            // is cursor in canvas widget area
            bool* _cursor_in_bounds = new bool(false);

            // widget size
            Vector2f* _canvas_size = new Vector2f(1, 1);

            // global transform
            Vector2f* _transform_offset = new Vector2f(0, 0);
            float* _transform_scale = new float(1);
            GLTransform* _transform = new GLTransform();

            // layer handling user input
            struct ControlLayer : public CanvasLayer
            {
                public:
                    ControlLayer(Canvas*);

                    operator Widget*() override;
                    void update() override;

                private:
                    GLArea _area;

                    void update_pixel_position();
                    static void on_resize(GLArea*, int, int, ControlLayer* instance);

                    MotionEventController _motion_controller;
                    static void on_motion_enter(MotionEventController*, double x, double y, ControlLayer* instance);
                    static void on_motion_leave(MotionEventController*, ControlLayer* instance);
                    static void on_motion(MotionEventController*, double x, double y, ControlLayer* instance);

                    ClickEventController _click_controller;
                    static void on_click_pressed(ClickEventController*, size_t n, double x, double y, ControlLayer* instance);
                    static void on_click_released(ClickEventController*, size_t n, double x, double y, ControlLayer* instance);
            };

            ControlLayer _control_layer = ControlLayer(this);

            // TRANSPARENCY BACKGROUND

            class TransparencyTilingLayer : public CanvasLayer
            {
                public:
                    TransparencyTilingLayer(Canvas*);
                    ~TransparencyTilingLayer();

                    void update() override;
                    operator Widget*() override;

                private:
                    GLArea _area;
                    Shape* _shape = nullptr;

                    Shape* _subtract_top = nullptr;
                    Shape* _subtract_right = nullptr;
                    Shape* _subtract_bottom = nullptr;
                    Shape* _subtract_left = nullptr;

                    static inline Shader* _shader = nullptr;
                    void reformat();

                    static void on_realize(Widget*, TransparencyTilingLayer* instance);
                    static void on_resize(GLArea*, int, int, TransparencyTilingLayer* instance);
            };

            TransparencyTilingLayer _transparency_tiling_layer = TransparencyTilingLayer(this);

            // GRID

            RGBA _grid_color = state::settings_file->get_value_as<RGBA>("canvas", "grid_color");
            bool _grid_visible = state::settings_file->get_value_as<bool>("canvas", "grid_visible");

            class GridLayer : public CanvasLayer
            {
                public:
                    GridLayer(Canvas*);
                    ~GridLayer();

                    void update();
                    operator Widget*() override;

                private:
                    GLArea _area;
                    std::vector<Shape*> _h_lines;
                    std::vector<Shape*> _v_lines;

                    bool _grid_visible = true;
                    RGBA _grid_color = RGBA(-1, -1, -1, 0);
                    Vector2f _canvas_size = {0, 0};
                    Vector2ui _layer_resolution;

                    void reformat();

                    static void on_realize(Widget*, GridLayer* instance);
                    static void on_resize(GLArea*, int, int, GridLayer* instance);
            };

            GridLayer _grid_layer = GridLayer(this);

            // LAYERS

            class LayersLayer : public CanvasLayer
            {
                public:
                    LayersLayer(Canvas* owner);
                    ~LayersLayer();

                    void update() override;
                    operator Widget*() override;

                private:
                    void reformat();

                    GLArea _area;
                    std::vector<Shape*> _layer_shapes;

                    size_t _current_frame = -1;
                    size_t _n_layers = -1;
                    Vector2ui _layer_resolution;

                    Vector2f _canvas_size = {-1, -1};

                    static void on_realize(Widget*, LayersLayer* instance);
                    static void on_resize(GLArea*, int, int, LayersLayer* instance);
            };

            LayersLayer _layers_layer = LayersLayer(this);

            /*
            // BRUSH

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

                    ClickEventController _click_controller;
                    bool _click_active;
                    static void on_click_pressed(ClickEventController*, size_t n, double x, double y, BrushCursorLayer* instance);
                    static void on_click_released(ClickEventController*, size_t n, double x, double y, BrushCursorLayer* instance);

                    Vector2i widget_to_texture_coords(Vector2f);
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

             */

            // UNDO / REDO

            enum class BackupMode
            {
                UNDO,
                REDO,
                NONE
            };

            /// \param mode: if undo, inverse action is pushed to undo queue, else pushed to redo queue
            void draw(const SubImage&, BackupMode = BackupMode::NONE);
            void undo_safepoint();

            size_t _undo_queue_size_byte = 0;
            std::deque<SubImage> _undo_queue;
            std::deque<SubImage> _redo_queue;

            Action _undo_draw_step_action = Action("canvas.undo_draw_step");
            void on_undo_action();

            Action _redo_draw_step_action = Action("canvas.redo_draw_step");
            void on_redo_action();

            ShortcutController _shortcut_controller = ShortcutController(state::app);

            // main
            Overlay _canvas_layer_overlay;
    };
}

#include <app/canvas/control_layer.hpp>
#include <app/canvas/transparency_tiling_area.hpp>
#include <app/canvas/grid_layer.hpp>
#include <app/canvas/layers_layer.hpp>

//#include <app/canvas/brush_cursor_area.hpp>

namespace mousetrap
{
    // CANVAS

    Canvas::Canvas()
    {
        _canvas_layer_overlay.set_child(_transparency_tiling_layer);
        _canvas_layer_overlay.add_overlay(_layers_layer);
        _canvas_layer_overlay.add_overlay(_grid_layer);

        _canvas_layer_overlay.add_overlay(_control_layer);

        set_transform_scale(12);

        // setup undo / redo architecture

        _undo_draw_step_action.set_do_function([](Canvas* instance) -> void {
            instance->on_undo_action();
        }, this);
        _undo_draw_step_action.add_shortcut(state::keybindings_file->get_value("canvas", "undo_draw_step"));
        state::app->add_action(_undo_draw_step_action);

        _redo_draw_step_action.set_do_function([](Canvas* instance) -> void {
            instance->on_redo_action();
        }, this);
        _redo_draw_step_action.add_shortcut(state::keybindings_file->get_value("canvas", "redo_draw_step"));
        state::app->add_action(_redo_draw_step_action);

        for (auto* action : {&_undo_draw_step_action, &_redo_draw_step_action})
            _shortcut_controller.add_action(action->get_id());

        _undo_queue.emplace_back();
        _redo_queue.emplace_back();
    }

    void Canvas::on_undo_action()
    {
        if (_undo_queue.empty() or _undo_queue.back().is_empty())
            return;

        _redo_queue.emplace_back();
        draw(_undo_queue.back(), BackupMode::REDO);
        _undo_queue.pop_back();
    }

    void Canvas::on_redo_action()
    {
        if (_redo_queue.empty() or _redo_queue.back().is_empty())
            return;

        _undo_queue.emplace_back();
        draw(_redo_queue.back(), BackupMode::UNDO);
        _redo_queue.pop_back();
    }

    Canvas::operator Widget*()
    {
        return &_canvas_layer_overlay;
    }

    void Canvas::set_transform_scale(float scale)
    {
        *_transform_scale = scale;
        _transform->reset();
        _transform->translate(*_transform_offset);
        _transform->scale(*_transform_scale, *_transform_scale);
    }

    void Canvas::undo_safepoint()
    {
        if (not _undo_queue.empty())
            _undo_queue_size_byte += _undo_queue.back().get_n_pixels() * (32 + 32 + 4 * 32);

        _undo_queue.emplace_back();
        _redo_queue.clear();

        static size_t max_size_byte = state::settings_file->get_value_as<float>("canvas", "undo_cache_size") * 10e6;

        if (_undo_queue_size_byte > max_size_byte)
            _undo_queue.pop_front();
    }

    void Canvas::draw(const SubImage& subimage, BackupMode backup_mode)
    {
        auto& frame = state::layers.at(state::current_layer)->frames.at(state::current_frame);

        for (auto& it : subimage)
        {
            auto pos = Vector2i(it.x, it.y);

            if (pos.x < 0 or pos.x >= frame.image->get_size().x or pos.y < 0 or pos.y >= frame.image->get_size().y)
                continue;

            if (backup_mode == BackupMode::UNDO)
                _undo_queue.back().add(pos, frame.image->get_pixel(pos.x, pos.y));
            else if (backup_mode == BackupMode::REDO)
                _redo_queue.back().add(pos, frame.image->get_pixel(pos.x, pos.y));

            frame.draw_pixel(pos, it.color);
        }

        frame.update_texture();
        _layers_layer.update();
    }

    void Canvas::draw_brush(Vector2i position, Brush* brush, RGBA color)
    {
        auto to_draw = SubImage();
        auto& frame = state::layers.at(state::current_layer)->frames.at(state::current_frame);

        static float alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");
        auto& brush_image = brush->get_image();

        const int x_start = int(position.x) - int(brush_image.get_size().x / 2);
        const int y_start = int(position.y) - int(brush_image.get_size().y / 2);

        int x_offset = brush_image.get_size().x % 2 == 0 ? 1 : 0;
        int y_offset = brush_image.get_size().y % 2 == 0 ? 1 : 0;

        for (int x = x_start; x < x_start + int(brush_image.get_size().x); ++x)
        {
            for (int y = y_start; y < y_start + int(brush_image.get_size().y); ++y)
            {
                auto pos = Vector2i(x + x_offset, y + y_offset);
                if (pos.x < 0 or pos.x > frame.image->get_size().x or pos.y < 0 or pos.y > frame.image->get_size().y)
                    continue;

                auto source = brush_image.get_pixel(x - x_start, y - y_start);
                if (source.a > alpha_eps)
                    to_draw.add(pos, RGBA(
                            source.r * color.r,
                            source.g * color.g,
                            source.b * color.b,
                            source.a * color.a
                    ));
            }
        }

        draw(to_draw, BackupMode::UNDO);
    }

    void Canvas::draw_brush_line(Vector2i a, Vector2i b, Brush* brush, RGBA color)
    {
        auto to_draw = SubImage();

        auto& frame = state::layers.at(state::current_layer)->frames.at(state::current_frame);
        auto& brush_image = brush->get_image();
        auto points = get_line_points(a, b);

        auto x_offset = brush_image.get_size().x % 2 == 0 ? 1 : 0;
        auto y_offset = brush_image.get_size().y % 2 == 0 ? 1 : 0;

        static float alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

        for (auto& position : points)
        {
            auto x_start = position.x - brush_image.get_size().x / 2;
            auto y_start = position.y - brush_image.get_size().y / 2;

            for (int x = x_start; x < x_start + int(brush_image.get_size().x); ++x)
            {
                for (int y = y_start; y < y_start + int(brush_image.get_size().y); ++y)
                {
                    auto pos = Vector2i(x + x_offset, y + y_offset);
                    if (pos.x < 0 or pos.x > frame.image->get_size().x or pos.y < 0 or pos.y > frame.image->get_size().y)
                        continue;

                    auto source = brush_image.get_pixel(x - x_start, y - y_start);
                    if (source.a > alpha_eps)
                        to_draw.add(pos, RGBA(
                                source.r * color.r,
                                source.g * color.g,
                                source.b * color.b,
                                source.a * color.a
                        ));
                }
            }
        }

        draw(to_draw, BackupMode::UNDO);
    }

    void Canvas::update()
    {
        _control_layer.update();
        _transparency_tiling_layer.update();
        _layers_layer.update();
        _grid_layer.update();
    }
}