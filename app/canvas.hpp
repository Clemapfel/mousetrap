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
            Vector2i* _previous_pixel_position = new Vector2i(0, 0);
            Vector2i* _current_pixel_position = new Vector2i(0, 0);

            // position in widget space
            Vector2f* _previous_cursor_position = new Vector2f(0, 0);
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

                    KeyEventController _key_controller;
                    static bool on_key_pressed(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, ControlLayer* instance);
                    static bool on_key_released(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, ControlLayer* instance);
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

            // BRUSH

            class BrushCursorLayer : public CanvasLayer
            {
                public:
                    BrushCursorLayer(Canvas* owner);
                    ~BrushCursorLayer();

                    operator Widget*() override;
                    void update() override;

                private:
                    GLArea _area;

                    float* _timer = new float(0); // seconds
                    Shape* _cursor_shape = nullptr;
                    Shape* _cursor_outline_shape_hlines = nullptr;
                    Shape* _cursor_outline_shape_vlines = nullptr;

                    Vector2f _canvas_size = Vector2f(1, 1);
                    Vector2f _cursor_position = Vector2f(0, 0);

                    float _brush_opacity;
                    HSVA _brush_color;
                    Brush* _current_brush;
                    bool _cursor_in_bounds = false;

                    void reformat();

                    static void on_realize(Widget*, BrushCursorLayer* instance);
                    static void on_resize(GLArea*, int, int, BrushCursorLayer* instance);
            };

            BrushCursorLayer _brush_cursor_layer = BrushCursorLayer(this);

            // UNDO / REDO

            enum class BackupMode
            {
                UNDO,
                REDO,
                NONE
            };

            /// \param mode: if undo, inverse action is pushed to undo queue, else pushed to redo queue
            void draw(const SubImage&, BlendMode mode, BackupMode = BackupMode::NONE);
            void add_brush_to_subimage(Vector2i brush_position, Brush*, RGBA, SubImage& image);
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

            // tool behavior
            void resolve_brush_click_pressed();
            void resolve_brush_click_released();
            void resolve_brush_motion();

            void resolve_eraser_click_pressed();
            void resolve_eraser_click_released();
            void resolve_eraser_motion();
    };
}

#include <app/canvas/control_layer.hpp>
#include <app/canvas/transparency_tiling_layer.hpp>
#include <app/canvas/grid_layer.hpp>
#include <app/canvas/layers_layer.hpp>
#include <app/canvas/brush_cursor_area.hpp>
#include <app/canvas/tool_behavior.hpp>

namespace mousetrap
{
    // CANVAS

    Canvas::Canvas()
    {
        _canvas_layer_overlay.set_child(_transparency_tiling_layer);
        _canvas_layer_overlay.add_overlay(_layers_layer);
        _canvas_layer_overlay.add_overlay(_grid_layer);
        _canvas_layer_overlay.add_overlay(_brush_cursor_layer);

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
        _undo_queue.emplace_back();
        _redo_queue.emplace_back();
        _redo_queue.emplace_back();
    }

    void Canvas::on_undo_action()
    {
        if (_undo_queue.empty() or _undo_queue.back().is_empty())
            return;

        _redo_queue.emplace_back();
        draw(_undo_queue.back(), BlendMode::NONE, BackupMode::REDO);
        _undo_queue.pop_back();
    }

    void Canvas::on_redo_action()
    {
        if (_redo_queue.empty() or _redo_queue.back().is_empty())
            return;

        _undo_queue.emplace_back();
        draw(_redo_queue.back(), BlendMode::NONE, BackupMode::UNDO);
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

    void Canvas::add_brush_to_subimage(Vector2i position, Brush* brush, RGBA color, SubImage& out)
    {
        auto& brush_image = brush->get_image();
        static float alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

        const int x_start = int(position.x) - int(brush_image.get_size().x / 2);
        const int y_start = int(position.y) - int(brush_image.get_size().y / 2);

        int x_offset = brush_image.get_size().x % 2 == 0 ? 1 : 0;
        int y_offset = brush_image.get_size().y % 2 == 0 ? 1 : 0;

        for (int x = x_start; x < x_start + int(brush_image.get_size().x); ++x)
        {
            for (int y = y_start; y < y_start + int(brush_image.get_size().y); ++y)
            {
                auto pos = Vector2i(x + x_offset, y + y_offset);

                auto source = brush_image.get_pixel(x - x_start, y - y_start);
                if (source.a > alpha_eps)
                    out.add(pos, RGBA(
                        color.r,
                        color.g,
                        color.b,
                        color.a
                    ));
            }
        }
    }

    void Canvas::draw(const SubImage& subimage, BlendMode blend_mode, BackupMode backup_mode)
    {
        auto& frame = state::layers.at(state::current_layer)->frames.at(state::current_frame);
        auto& image = *frame.image;

        for (auto& it : subimage)
        {
            auto pos = Vector2i(it.x, it.y);

            if (pos.x < 0 or pos.x >= frame.image->get_size().x or pos.y < 0 or pos.y >= frame.image->get_size().y)
                continue;

            if (backup_mode == BackupMode::UNDO)
                _undo_queue.back().add(pos, frame.image->get_pixel(pos.x, pos.y));
            else if (backup_mode == BackupMode::REDO)
                _redo_queue.back().add(pos, frame.image->get_pixel(pos.x, pos.y));

            frame.draw_pixel(pos, it.color, blend_mode);
        }

        frame.update_texture();
        _layers_layer.update();
    }

    void Canvas::draw_brush(Vector2i position, Brush* brush, RGBA color)
    {
        auto to_draw = SubImage();
        add_brush_to_subimage(position, brush, color, to_draw);
        draw(to_draw, BlendMode::NONE, BackupMode::UNDO);
    }

    void Canvas::draw_brush_line(Vector2i a, Vector2i b, Brush* brush, RGBA color)
    {
        auto to_draw = SubImage();

        auto& frame = state::layers.at(state::current_layer)->frames.at(state::current_frame);
        auto& brush_image = brush->get_image();
        auto points = get_line_points(a, b);

        for (auto& position : points)
            add_brush_to_subimage(position, brush, color, to_draw);

        draw(to_draw, BlendMode::NONE, BackupMode::UNDO);
    }

    void Canvas::update()
    {
        _control_layer.update();
        _brush_cursor_layer.update();
        _transparency_tiling_layer.update();
        _layers_layer.update();
        _grid_layer.update();
    }
}