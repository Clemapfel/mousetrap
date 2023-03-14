// 
// Copyright 2022 Clemens Cords
// Created on 10/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/project_state.hpp>
#include <app/app_signals.hpp>
#include "include/msaa_texture.hpp"
#include <app/color_picker.hpp>

/*
 Symmetry:
    x pos, y pos
    x toggle, y toggle
    x color, y color

Background:
    visible toggle

Grid:
    visible toggle
    color set

Selection:
    toggle animated
    toggle visible
    color set

Scale:
    scale value
    reset scale + offset

Offset (Debug):
    x pos, y pos

User Input:
    y scroll inverted
    x scroll inverted
    draw non-mouse button
 */

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(canvas, toggle_grid_visible)
        DECLARE_GLOBAL_ACTION(canvas, open_grid_color_picker)

        DECLARE_GLOBAL_ACTION(canvas, toggle_brush_outline_visible)
        DECLARE_GLOBAL_ACTION(canvas, toggle_background_visible)
        DECLARE_GLOBAL_ACTION(canvas, toggle_horizontal_symmetry_active)
        DECLARE_GLOBAL_ACTION(canvas, toggle_vertical_symmetry_active)
        DECLARE_GLOBAL_ACTION(canvas, open_symmetry_color_picker)

        DECLARE_GLOBAL_ACTION(canvas, reset_transform)
        DECLARE_GLOBAL_ACTION(canvas, toggle_x_scroll_inverted)
        DECLARE_GLOBAL_ACTION(canvas, toggle_y_scroll_inverted)

        DECLARE_GLOBAL_ACTION(canvas, copy_to_clipboard)
        DECLARE_GLOBAL_ACTION(canvas, paste_clipboard)

        DECLARE_GLOBAL_ACTION(canvas, move_float_up)
        DECLARE_GLOBAL_ACTION(canvas, move_float_right)
        DECLARE_GLOBAL_ACTION(canvas, move_float_down)
        DECLARE_GLOBAL_ACTION(canvas, move_float_left)

        DECLARE_GLOBAL_ACTION(canvas, apply_bucket_fill);
        DECLARE_GLOBAL_ACTION(canvas, apply_color_select);
        DECLARE_GLOBAL_ACTION(canvas, apply_gradient);
        DECLARE_GLOBAL_ACTION(canvas, apply_marquee_neighborhood_select);

        DECLARE_GLOBAL_ACTION(canvas, select_all);
        DECLARE_GLOBAL_ACTION(canvas, invert_selection);
        DECLARE_GLOBAL_ACTION(canvas, selection_mode_replace);
        DECLARE_GLOBAL_ACTION(canvas, selection_mode_add);
        DECLARE_GLOBAL_ACTION(canvas, selection_mode_subtract);
        DECLARE_GLOBAL_ACTION(canvas, selection_outline_animated);
        DECLARE_GLOBAL_ACTION(canvas, toggle_allow_drawing_outside_selection);
    }

    struct Canvas : public AppComponent,
            public signals::BrushSelectionChanged,
            public signals::ActiveToolChanged,
            public signals::LayerFrameSelectionChanged,
            public signals::ColorSelectionChanged,
            public signals::SelectionChanged,
            public signals::SelectionModeChanged,
            public signals::OnionskinVisibilityToggled,
            public signals::OnionskinLayerCountChanged,
            public signals::LayerImageUpdated,
            public signals::LayerCountChanged,
            public signals::LayerPropertiesChanged,
            public signals::LayerResolutionChanged,
            public signals::ColorOffsetChanged,
            public signals::ImageFlipChanged,
            public signals::CursorPositionChanged
    {
        public:
            Canvas();
            operator Widget*() override;

            Widget* get_control_bar();

        protected:
            void on_brush_selection_changed() override;
            void on_active_tool_changed() override;
            void on_color_selection_changed() override;
            void on_selection_changed() override;
            void on_selection_mode_changed() override;
            void on_onionskin_visibility_toggled() override;
            void on_onionskin_layer_count_changed() override;

            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;
            void on_layer_resolution_changed() override;
            void on_layer_frame_selection_changed() override;

            void on_color_offset_changed() override;
            void on_image_flip_changed() override;

            void on_cursor_position_changed() override;

        private:
            // global properties

            float _scale = 1;
            void set_scale(float);

            Vector2f _offset = {0, 0}; // widget-space coords
            void set_offset(float, float);

            Vector2f _canvas_size = {1, 1};
            void set_canvas_size(Vector2f);

            bool _grid_visible = state::settings_file->get_value_as<bool>("canvas", "grid_visible");
            void set_grid_visible(bool);

            bool _brush_outline_visible = state::settings_file->get_value_as<bool>("canvas", "brush_outline_visible");
            void set_brush_outline_visible(bool);

            bool _background_visible = state::settings_file->get_value_as<bool>("canvas", "background_visible");
            void set_background_visible(bool);

            bool _horizontal_symmetry_active = state::settings_file->get_value_as<bool>("canvas", "horizontal_symmetry_active");
            void set_horizontal_symmetry_active(bool);

            bool _vertical_symmetry_active = state::settings_file->get_value_as<bool>("canvas", "vertical_symmetry_active");
            void set_vertical_symmetry_active(bool);

            size_t _horizontal_symmetry_pixel_position = active_state->get_layer_resolution().x / 2;
            void set_horizontal_symmetry_pixel_position(size_t);

            size_t _vertical_symmetry_pixel_position = active_state->get_layer_resolution().y / 2;
            void set_vertical_symmetry_pixel_position(size_t);

            Vector2i _cursor_position = {0, 0};
            void set_cursor_position(Vector2i);

            bool _cursor_in_bounds = false;
            void set_cursor_in_bounds(bool);

            Vector2f _widget_cursor_position = {0, 0};
            void set_widget_cursor_position(Vector2f);

            Vector2i _origin = {0, 0};
            Vector2i _destination = {0, 0};

            void draw(const DrawData&);

            // layers

            class TransparencyTilingLayer
            {
                public:
                    TransparencyTilingLayer(Canvas* owner);
                    operator Widget*();

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void set_background_visible(bool);

                    void on_layer_resolution_changed();

                private:
                    Canvas* _owner;

                    GLArea _area;
                    Shape* _shape = nullptr;
                    Shader* _shader = nullptr;

                    Vector2f* _canvas_size = new Vector2f(1, 1);
                    static void on_area_realize(Widget* widget, TransparencyTilingLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, TransparencyTilingLayer* instance);

                    bool _visible = true;

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    void reformat();
            };

            TransparencyTilingLayer* _transparency_tiling_layer = new TransparencyTilingLayer(this);

            class LayerLayer
            {
                public:
                    LayerLayer(Canvas* owner);
                    operator Widget*();

                    void on_layer_image_updated();
                    void on_layer_count_changed();
                    void on_layer_properties_changed();
                    void on_layer_resolution_changed();
                    void on_layer_frame_selection_changed();

                    void on_color_offset_changed();
                    void on_image_flip_changed();

                    void set_scale(float);
                    void set_offset(Vector2f);

                private:
                    Canvas* _owner;

                    GLArea _area;
                    std::vector<Shape*> _layer_shapes;

                    Shader* _post_fx_shader = nullptr;

                    float* _h_offset = new float(0);
                    float* _s_offset = new float(0);
                    float* _v_offset = new float(0);
                    float* _r_offset = new float(0);
                    float* _g_offset = new float(0);
                    float* _b_offset = new float(0);
                    float* _a_offset = new float(0);

                    int* _flip_horizontally = new int(0);
                    int* _flip_vertically = new int(0);

                    static inline const int* yes = new int(1);
                    static inline const int* no = new int(0);

                    ApplyScope _color_offset_apply_scope = active_state->get_color_offset_apply_scope();
                    ApplyScope _image_flip_apply_scope = active_state->get_image_flip_apply_scope();

                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_area_realize(Widget* widget, LayerLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, LayerLayer* instance);

                    void queue_render_tasks();

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    void reformat();
            };

            LayerLayer* _layer_layer = new LayerLayer(this);

            class OnionskinLayer
            {
                public:
                    OnionskinLayer(Canvas* owner);
                    operator Widget*();

                    void on_layer_image_updated();
                    void on_layer_count_changed();
                    void on_layer_properties_changed();
                    void on_layer_resolution_changed();
                    void on_layer_frame_selection_changed();

                    void on_onionskin_visibility_toggled();
                    void on_onionskin_layer_count_changed();

                    void set_scale(float);
                    void set_offset(Vector2f);

                private:
                    Canvas* _owner;

                    GLArea _area;
                    Shader* _onionskin_shader = nullptr;
                    std::vector<Shape*> _frame_shapes;

                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_area_realize(Widget* widget, OnionskinLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, OnionskinLayer* instance);

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    void reformat();
            };

            OnionskinLayer* _onionskin_layer = new OnionskinLayer(this);

            class GridLayer
            {
                public:
                    GridLayer(Canvas*);
                    operator Widget*();

                    void on_layer_resolution_changed();

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void set_visible(bool);

                private:
                    Canvas* _owner;

                    GLArea _area;
                    std::vector<Shape*> _h_shapes;
                    std::vector<Shape*> _v_shapes;

                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_area_realize(Widget* widget, GridLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, GridLayer* instance);

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    bool _visible_requested = true;
                    void update_visibility();
                    void reformat();
            };

            GridLayer* _grid_layer = new GridLayer(this);

            class SymmetryRulerLayer
            {
                public:
                    SymmetryRulerLayer(Canvas*);
                    operator Widget*();

                    void on_layer_resolution_changed();

                    void set_horizontal_symmetry_pixel_position(size_t px);
                    void set_vertical_symmetry_pixel_position(size_t px);

                    void set_horizontal_symmetry_active(bool);
                    void set_vertical_symmetry_active(bool);

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void set_color(HSVA);

                    void set_cursor_position(Vector2i);

                private:
                    Canvas* _owner;

                    GLArea _area;

                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_area_realize(Widget* widget, SymmetryRulerLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, SymmetryRulerLayer* instance);

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    Vector2i _cursor_position = {0, 0};
                    HSVA _color = RGBA(1, 1, 1, 1).operator HSVA();

                    size_t _h_position = 0.5 * active_state->get_layer_resolution().x;
                    size_t _v_position = 0.5 * active_state->get_layer_resolution().y;
                    size_t _h_enabled = true;
                    size_t _v_enabled = true;

                    Shape* _h_anchor_left = nullptr;
                    Shape* _h_anchor_left_outline = nullptr;
                    Shape* _h_anchor_right = nullptr;
                    Shape* _h_anchor_right_outline = nullptr;
                    Shape* _h_ruler = nullptr;
                    Shape* _h_ruler_outline = nullptr;

                    Shape* _v_anchor_top = nullptr;
                    Shape* _v_anchor_top_outline = nullptr;
                    Shape* _v_anchor_bottom = nullptr;
                    Shape* _v_anchor_bottom_outline = nullptr;
                    Shape* _v_ruler = nullptr;
                    Shape* _v_ruler_outline = nullptr;

                    void reformat();
            };

            SymmetryRulerLayer* _symmetry_ruler_layer = new SymmetryRulerLayer(this);

            class BrushShapeLayer
            {
                public:
                    BrushShapeLayer(Canvas*);
                    operator Widget*();

                    void set_brush_outline_visible(bool);
                    void set_outline_color(RGBA);

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void set_cursor_position(Vector2i);
                    void set_cursor_in_bounds(bool);

                    void on_layer_resolution_changed();
                    void on_color_selection_changed();
                    void on_brush_selection_changed();
                    void on_active_tool_changed();

                private:
                    Canvas* _owner;

                    GLArea _area;

                    Shape* _brush_shape = nullptr;
                    Texture* _brush_texture = nullptr;
                    RenderTask* _brush_shape_task = nullptr;

                    Shader* _outline_shader = nullptr;
                    RenderTexture* _render_texture = nullptr;
                    Shape* _render_shape = nullptr;
                    RenderTask* _render_shape_task = nullptr;

                    Vector2f* _canvas_size = new Vector2f(1, 1);
                    static void on_area_realize(Widget* widget, BrushShapeLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, BrushShapeLayer* instance);
                    static bool on_area_render(GLArea*, GdkGLContext*, BrushShapeLayer* instance);

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    Vector2i _position = {0, 0}; // gl coords
                    bool _visible = true;
                    gint* _outline_visible = new gint(true);
                    RGBA* _outline_color = new RGBA(1, 1, 1, 0.5);

                    void reformat();
            };

            BrushShapeLayer* _brush_shape_layer = new BrushShapeLayer(this);

            class GradientLayer
            {
                public:
                    GradientLayer(Canvas*);
                    operator Widget*();

                    void on_layer_resolution_changed();
                    void set_scale(float);
                    void set_offset(Vector2f);
                    void on_color_selection_changed();
                    void set_origin_point(Vector2f);
                    void set_destination_point(Vector2f);
                    void set_clamp(bool);

                    using ShapeMode = int;
                    static inline const ShapeMode LINEAR = 0;
                    static inline const ShapeMode CIRCULAR = 1;
                    void set_shape_mode(ShapeMode);

                    using DitherMode = int;
                    static inline const DitherMode DITHER_NONE = 0;
                    static inline const DitherMode DITHER_2x2 = 1;
                    static inline const DitherMode DITHER_4x4 = 2;
                    static inline const DitherMode DITHER_8x8 = 3;
                    void set_dither_mode(DitherMode);

                    // TODO
                    void recompile_shader();
                    // TODO

                private:
                    Canvas* _owner;
                    GLArea _area;

                    float _scale;
                    Vector2f _offset;

                    static inline Shader* _shader = nullptr;
                    Shape* _shader_shape = nullptr;
                    RenderTask* _shader_task = nullptr;

                    RGBA* _origin_color_rgba = new RGBA(0, 0, 0, 1);
                    RGBA* _destination_color_rgba = new RGBA(1, 1, 1, 1);

                    gint* _dither_mode = new int(DITHER_4x4);
                    Vector2f* _origin_point = new Vector2f(0, 1);
                    Vector2f* _destination_point = new Vector2f(0, -1);
                    gint* _is_circular = new int(0);
                    gint* _clamp = new int(0);
                    Vector2f* _canvas_size = new Vector2f(1, 1);

                    RenderTexture* _render_texture = nullptr;
                    Shape* _render_shape = nullptr;
                    RenderTask* _render_task = nullptr;

                    void reformat();

                    static void on_area_realize(Widget* widget, GradientLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, GradientLayer* instance);
                    static bool on_area_render(GLArea*, GdkGLContext*, GradientLayer* instance);
            };

            GradientLayer* _gradient_layer = new GradientLayer(this);

            class WireframeLayer
            {
                public:
                    WireframeLayer(Canvas*);
                    operator Widget*();

                    void on_layer_resolution_changed();

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void set_widget_cursor_position(Vector2f);

                    void set_a(Vector2i pixel_pos);
                    void set_b(Vector2i pixel_pos);

                    void set_line_visible(bool);
                    void set_rectangle_visible(bool);
                    void set_circle_visible(bool);

                    void update_visibility_from_cursor_pos(Vector2i pixel_pos);

                    DrawData draw();

                private:
                    Canvas* _owner;
                    GLArea _area;

                    Vector2i _a = {0, 0};
                    Vector2i _b = active_state->get_layer_resolution();

                    bool _line_visible = false;
                    bool _rectangle_visible = false;
                    bool _circle_visible = true;

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    Vector2f _widget_cursor_position = {0, 0};
                    void reformat();
                    void update_highlighting();

                    Vector2f _canvas_size = {1, 1};
                    static void on_area_realize(Widget* widget, WireframeLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, WireframeLayer* instance);
                    static bool on_area_render(GLArea*, GdkGLContext*, WireframeLayer* instance);

                    MultisampledRenderTexture _render_texture;
                    Shape* _render_texture_shape = nullptr;
                    RenderTask* _render_texture_task = nullptr;
                    Shader* _render_shader = nullptr;
                    
                    // MODE: LINE

                    struct LineToolShape
                    {
                        Shape* origin_anchor_shape = nullptr;
                        Shape* origin_anchor_inner_outline = nullptr;
                        Shape* origin_anchor_outer_outline = nullptr;

                        Shape* line_shape = nullptr;
                        Shape* line_outline = nullptr;

                        Shape* destination_anchor_shape = nullptr;
                        Shape* destination_anchor_inner_outline = nullptr;
                        Shape* destination_anchor_outer_outline = nullptr;
                    };

                    LineToolShape _line_tool_shape;
                    std::vector<RenderTask> _line_tool_render_tasks;

                    // MODE: RECTANGLE / CIRCLE
                    
                    struct RectangleToolShape
                    {
                        Shape* rectangle_shape = nullptr;
                        Shape* rectangle_inner_outline = nullptr;
                        Shape* rectangle_outer_outline = nullptr;

                        Shape* top_left_anchor_shape = nullptr;
                        Shape* top_left_anchor_inner_outline = nullptr;
                        Shape* top_left_anchor_outer_outline = nullptr;

                        Shape* top_right_anchor_shape = nullptr;
                        Shape* top_right_anchor_inner_outline = nullptr;
                        Shape* top_right_anchor_outer_outline = nullptr;

                        Shape* bottom_left_anchor_shape = nullptr;
                        Shape* bottom_left_anchor_inner_outline = nullptr;
                        Shape* bottom_left_anchor_outer_outline = nullptr;

                        Shape* bottom_right_anchor_shape = nullptr;
                        Shape* bottom_right_anchor_inner_outline = nullptr;
                        Shape* bottom_right_anchor_outer_outline = nullptr;

                        Shape* top_anchor_shape = nullptr;
                        Shape* top_anchor_inner_outline = nullptr;
                        Shape* top_anchor_outer_outline = nullptr;

                        Shape* bottom_anchor_shape = nullptr;
                        Shape* bottom_anchor_inner_outline = nullptr;
                        Shape* bottom_anchor_outer_outline = nullptr;

                        Shape* left_anchor_shape = nullptr;
                        Shape* left_anchor_inner_outline = nullptr;
                        Shape* left_anchor_outer_outline = nullptr;

                        Shape* right_anchor_shape = nullptr;
                        Shape* right_anchor_inner_outline = nullptr;
                        Shape* right_anchor_outer_outline = nullptr;

                        Shape* center_cross = nullptr;
                        Shape* center_cross_outline = nullptr;

                        Shape* circle = nullptr;
                        Shape* circle_inner_outline = nullptr;
                        Shape* circle_outer_outline = nullptr;
                    };

                    RectangleToolShape _rectangle_tool_shape;
                    std::vector<RenderTask> _rectangle_tool_render_tasks;
                    std::vector<RenderTask> _circle_tool_render_tasks;
            };

            WireframeLayer* _wireframe_layer = new WireframeLayer(this);

            class SelectionLayer
            {
                public:
                    SelectionLayer(Canvas*);
                    operator Widget*();

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void on_selection_changed();

                    bool get_animated() const;
                    void set_animated(bool);

                    void set_color(HSVA);

                private:
                    Canvas* _owner;
                    GLArea _area;

                    static inline int* _outline_shader_right_flag = new int(1);
                    static inline int* _outline_shader_top_flag = new int(2);
                    static inline int* _outline_shader_left_flag = new int(3);
                    static inline int* _outline_shader_bottom_flag = new int(4);
                    static inline float* _outline_time_s = new float(0);
                    static inline int* _animation_paused = new int(0);

                    HSVA _color = RGBA(1, 1, 1, 1).operator HSVA();

                    Vector2f* _canvas_size = new Vector2f(1, 1);
                    Shader* _outline_shader;

                    Shape* _outline_left_to_right = nullptr;
                    Shape* _outline_top_to_bottom = nullptr;
                    Shape* _outline_right_to_left = nullptr;
                    Shape* _outline_bottom_to_top = nullptr;
                    Shape* _outline_outline = nullptr;

                    Vector2f _outline_top_initial_position;
                    Vector2f _outline_right_initial_position;
                    Vector2f _outline_bottom_initial_position;
                    Vector2f _outline_left_initial_position;
                    Vector2f _outline_outline_initial_position;

                    void reschedule_render_tasks();

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    void reformat();

                    static void on_realize(Widget*, SelectionLayer* instance);
                    static void on_resize(GLArea*, int, int, SelectionLayer* instance);
            };

            SelectionLayer* _selection_layer = new SelectionLayer(this);

            class UserInputLayer
            {
                public:
                    UserInputLayer(Canvas*);
                    operator Widget*();

                    void set_scale(float);
                    void set_offset(Vector2f);

                private:
                    Canvas* _owner;
                    GLArea _proxy;

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    Vector2f _canvas_size = {1, 1};
                    static void on_area_resize(GLArea*, int, int, UserInputLayer*);

                    bool should_trigger(const std::string& trigger, guint keyval);
                    bool _scroll_scale_active = false;
                    bool _lock_axis_movement = false;

                    float _scale_backup = 1;
                    size_t _n_frames_since_pinch = 0;

                    Vector2f _absolute_widget_space_pos = {0, 0};
                    Vector2f _normalized_widget_space_pos = {0, 0};
                    void update_cursor_pos();

                    bool _mouse_button_pressed = false;
                    Vector2i _previous_cursor_pos = {0, 0};
                    Vector2iSet _stroke_points;

                    ClickEventController _click_controller;
                    static void on_click_pressed(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance);
                    static void on_click_released(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance);

                    MotionEventController _motion_controller;
                    static void on_motion_enter(MotionEventController*, double x, double y, UserInputLayer* instance);
                    static void on_motion_leave(MotionEventController*, UserInputLayer* instance);
                    static void on_motion(MotionEventController*, double x, double y, UserInputLayer* instance);

                    KeyEventController _key_controller;
                    static bool on_key_pressed(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, UserInputLayer* instance);
                    static bool on_key_released(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, UserInputLayer* instance);

                    ScrollEventController _scroll_controller;
                    static void on_scroll_begin(ScrollEventController*, UserInputLayer* instance);
                    static void on_scroll_end(ScrollEventController*, UserInputLayer* instance);
                    static void on_scroll(ScrollEventController*, double x, double y, UserInputLayer* instance);

                    DragEventController _drag_controller;
                    static void on_drag_begin(DragEventController*, double x, double y, UserInputLayer* instance);
                    static void on_drag_update(DragEventController*, double x, double y, UserInputLayer* instance);
                    static void on_drag_end(DragEventController*, double x, double y, UserInputLayer* instance);

                    ZoomEventController _zoom_controller;
                    static void on_scale_changed(ZoomEventController*, double distance, UserInputLayer* instance);

                    ShortcutController _shortcut_controller;
            };

            UserInputLayer* _user_input_layer = new UserInputLayer(this);

            class ControlBar
            {
                public:
                    ControlBar(Canvas* owner);
                    operator Widget*();

                    void set_grid_visible(bool);
                    void set_background_visible(bool);
                    void set_brush_outline_visible(bool);

                    void on_layer_resolution_changed();

                    void set_horizontal_symmetry_active(bool);
                    void set_vertical_symmetry_active(bool);
                    void set_horizontal_symmetry_pixel_position(size_t px);
                    void set_vertical_symmetry_pixel_position(size_t px);

                    void set_cursor_position(Vector2i);
                    void set_cursor_in_bounds(bool);
                    void set_scale(float);

                private:
                    Canvas* _owner;

                    Button _reset_view_button;
                    ImageDisplay _reset_view_icon = ImageDisplay(get_resource_path() + "icons/canvas_reset_view.png");

                    SeparatorLine _view_spacer;

                    ToggleButton _brush_outline_visible_toggle_button;
                    ImageDisplay _brush_outline_visible_icon = ImageDisplay(get_resource_path() + "icons/canvas_brush_outline_visible.png");

                    SpinButton _scale_scale = SpinButton(1, 99, 0.1);

                    Box _view_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    ToggleButton _grid_visible_toggle_button;
                    ImageDisplay _grid_visible_icon = ImageDisplay(get_resource_path() + "icons/canvas_grid.png");

                    ToggleButton _background_visible_toggle_button;
                    ImageDisplay _background_visible_icon = ImageDisplay(get_resource_path() + "icons/canvas_background.png");

                    Box _visibility_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    Button _flip_horizontally_button;
                    ImageDisplay _flip_horizontally_icon = ImageDisplay(get_resource_path() + "icons/canvas_flip_horizontally.png");

                    Button _flip_vertically_button;
                    ImageDisplay _flip_vertically_icon = ImageDisplay(get_resource_path() + "icons/canvas_flip_vertically.png");

                    Box _flip_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    Box _selection_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    ToggleButton _allow_draw_outside_selection_button;
                    ImageDisplay _allow_draw_outside_selection_icon = ImageDisplay(get_resource_path() + "icons/canvas_allow_draw_outside_selection.png");

                    Button _invert_selection_button;
                    ImageDisplay _invert_selection_icon = ImageDisplay(get_resource_path() + "icons/canvas_invert_selection.png");

                    DropDown _mode_dropdown;

                    Label _replace_list_label = Label("Replace");
                    Label _replace_selected_label = Label("Replace");
                    Label _add_list_label = Label("Add");
                    Label _add_selected_label = Label("Add");
                    Label _subtract_list_label = Label("Subtract");
                    Label _subtract_selected_label = Label("Subtract");

                    ToggleButton _horizontal_symmetry_toggle_button;
                    ImageDisplay _horizontal_symmetry_icon = ImageDisplay(get_resource_path() + "icons/canvas_horizontal_symmetry.png");

                    ToggleButton _vertical_symmetry_toggle_button;
                    ImageDisplay _vertical_symmetry_icon = ImageDisplay(get_resource_path() + "icons/canvas_vertical_symmetry.png");

                    MenuButton _symmetry_control_menu_button;
                    Popover _symmetry_control_menu_popover;
                    Label _symmetry_control_label = Label("Symmetry");

                    Box _symmetry_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    class SymmetryControlMenu
                    {
                        public:
                            SymmetryControlMenu(ControlBar*);
                            operator Widget*();

                            void on_layer_resolution_changed();

                            void set_horizontal_symmetry_active(bool);
                            void set_vertical_symmetry_active(bool);
                            void set_horizontal_symmetry_pixel_position(size_t px);
                            void set_vertical_symmetry_pixel_position(size_t px);

                        private:
                            ControlBar* _owner;
                            Box _main = Box(GTK_ORIENTATION_VERTICAL);

                            Label _title_label = Label("<b>Symmetry</b>");

                            Label _h_label = Label("Horizontal\t\t<tt>&#9135;</tt>");
                            Label _h_enabled_label = Label("Enabled: ");
                            Label _h_position_label = Label("Position (px):");
                            Box _h_check_button_box = Box(GTK_ORIENTATION_HORIZONTAL);
                            CheckButton _h_check_button;
                            SpinButton _h_spin_button = SpinButton(1, 1, 1);
                            Button _h_center_button;
                            Box _h_button_box = Box(GTK_ORIENTATION_HORIZONTAL);
                            Label _h_center_button_label = Label("center");
                            Box _h_box = Box(GTK_ORIENTATION_VERTICAL);

                            Label _v_label = Label("Vertical\t\t<tt>&#65372;</tt>");
                            Label _v_enabled_label = Label("Enabled: ");
                            Label _v_position_label = Label("Position (px):");
                            Box _v_check_button_box = Box(GTK_ORIENTATION_HORIZONTAL);
                            CheckButton _v_check_button;
                            SpinButton _v_spin_button = SpinButton(1, 1, 1);
                            Button _v_center_button;
                            Box _v_button_box = Box(GTK_ORIENTATION_HORIZONTAL);
                            Label _v_center_button_label = Label("center");
                            Box _v_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    };

                    SymmetryControlMenu* _symmetry_control_menu = new SymmetryControlMenu(this);

                    bool _position_visible = true;
                    Label _position_label;

                    MenuButton _menu_button;
                    Label _menu_button_label = Label("Canvas");

                    Box _scrolled_window_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    ScrolledWindow _scrolled_window;
            };

            ControlBar _control_bar = ControlBar(this);

            class ToolOptions
            {
                public:
                    ToolOptions(Canvas*);
                    operator Widget*();

                    void on_active_tool_changed();

                private:
                    Canvas* _owner;

                    Revealer _main_revealer;
                    Overlay _main;

                    // Gradient

                    class GradientOptions
                    {
                        public:
                            GradientOptions(Canvas*);
                            operator Widget*();

                        private:
                            Canvas* _owner;

                            void update_shader_anchors();

                            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                            Label _origin_label = Label("Origin (xy):");
                            SpinButton _origin_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
                            SpinButton _origin_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);

                            Label _destination_label = Label("Dest (xy):");
                            SpinButton _destination_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
                            SpinButton _destination_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);

                            Label _dithering_label = Label("Dithering: ");
                            DropDown _dithering_mode_dropdown;
                            Label _dithering_none_selected_label = Label("None");
                            Label _dithering_none_list_label = Label("None");

                            Label _dithering_2x2_selected_label = Label("2x2");
                            Label _dithering_2x2_list_label = Label("2x2");

                            Label _dithering_4x4_selected_label = Label("4x4");
                            Label _dithering_4x4_list_label = Label("4x4");

                            Label _dithering_8x8_selected_label = Label("8x8");
                            Label _dithering_8x8_list_label = Label("8x8");

                            Label _mode_label = Label("Shape:");
                            DropDown _mode_dropdown;
                            Label _circular_mode_selected_label = Label("Circular");
                            Label _circular_mode_list_label = Label("Circular");

                            Label _linear_mode_selected_label = Label("Linear");
                            Label _linear_mode_list_label = Label("Linear");

                            CheckButton _clamp_button;
                            Label _clamp_label = Label("Clamp:");
                    };

                    GradientOptions _gradient_options;
                    Revealer _gradient_revealer;
            };

            ToolOptions _tool_options = ToolOptions(this);

            CheckButton _line_visible_button;
            SpinButton _line_start_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
            SpinButton _line_start_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);
            SpinButton _line_end_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
            SpinButton _line_end_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);
            Label _line_label = Label("Line (xy -> xy): ");
            Box _line_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Box _debug_box = Box(GTK_ORIENTATION_VERTICAL);

            // main layout
            Adjustment* _x_offset_adjustment = new Adjustment();
            Adjustment* _y_offset_adjustment = new Adjustment();
            Scrollbar* _x_offset_scrollbar = new Scrollbar(GTK_ORIENTATION_HORIZONTAL);
            Scrollbar* _y_offset_scrollbar = new Scrollbar(GTK_ORIENTATION_VERTICAL);

            Button _reset_view_button;
            Box _y_scrollbar_and_reset_button_box = Box(GTK_ORIENTATION_VERTICAL);
            Box _x_scrollbar_and_canvas_box = Box(GTK_ORIENTATION_VERTICAL);
            void update_adjustment_bounds();

            Box _canvas_and_scrollbars_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);
            Overlay _layer_overlay;

    };

    namespace state
    {
        inline Canvas* canvas = nullptr;
    }

}