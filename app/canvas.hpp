// 
// Copyright 2022 Clemens Cords
// Created on 10/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/project_state.hpp>
#include <app/app_signals.hpp>
#include <app/msaa_texture.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(canvas, toggle_grid_visible)
        DECLARE_GLOBAL_ACTION(canvas, open_grid_color_settings)

        DECLARE_GLOBAL_ACTION(canvas, toggle_brush_outline_visible)
        DECLARE_GLOBAL_ACTION(canvas, toggle_horizontal_mirror_visible)
        DECLARE_GLOBAL_ACTION(canvas, toggle_vertical_mirror_visible)

        DECLARE_GLOBAL_ACTION(canvas, reset_transform)
    }

    struct Canvas : public AppComponent,
            public signals::BrushSelectionChanged,
            public signals::ActiveToolChanged,
            public signals::LayerFrameSelectionChanged,
            public signals::ColorSelectionChanged,
            public signals::SelectionChanged,
            public signals::OnionskinVisibilityToggled,
            public signals::OnionskinLayerCountChanged,
            public signals::LayerImageUpdated,
            public signals::LayerCountChanged,
            public signals::LayerPropertiesChanged,
            public signals::LayerResolutionChanged,
            public signals::ColorOffsetChanged,
            public signals::ImageFlipChanged
    {
        public:
            Canvas();
            operator Widget*() override;

        protected:
            void on_brush_selection_changed() override;
            void on_active_tool_changed() override;
            void on_color_selection_changed() override;
            void on_selection_changed() override;
            void on_onionskin_visibility_toggled() override;
            void on_onionskin_layer_count_changed() override;

            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;
            void on_layer_resolution_changed() override;
            void on_layer_frame_selection_changed() override;

            void on_color_offset_changed() override;
            void on_image_flip_changed() override;

        private:
            // global properties

            float _scale = 1;
            void set_scale(float);

            Vector2f _offset = {0, 0}; // widget-space coords
            void set_offset(float, float);

            bool _grid_visible = true;
            void set_grid_visible(bool);

            bool _brush_outline_visible = true;
            void set_brush_outline_visible(bool);

            Vector2i _brush_position = {0, 0}; // texture-space coords
            void set_brush_position(Vector2i);

            bool _background_visible = true;
            void set_background_visible(bool);

            Vector2i _origin = {0, 0};
            Vector2i _destination = {0, 0};

            void draw(const ProjectState::DrawData&);

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

                    void set_horizontal_symmetry_enabled(bool);
                    void set_vertical_symmetry_enabled(bool);

                    void set_scale(float);
                    void set_offset(Vector2f);
                    void set_color(HSVA);

                private:
                    Canvas* _owner;

                    GLArea _area;

                    Vector2f* _canvas_size = new Vector2f{1, 1};
                    static void on_area_realize(Widget* widget, SymmetryRulerLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, SymmetryRulerLayer* instance);

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
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

                    void set_brush_position(Vector2i);
                    void set_brush_outline_visible(bool);
                    void set_outline_color(RGBA);

                    void set_scale(float);
                    void set_offset(Vector2f);

                    void on_layer_resolution_changed();
                    void on_color_selection_changed();
                    void on_brush_selection_changed();

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
                    gint* _outline_visible = new gint(true);
                    RGBA* _outline_color = new RGBA(1, 1, 1, 0.5);

                    void reformat();
            };

            BrushShapeLayer* _brush_shape_layer = new BrushShapeLayer(this);

            class WireframeLayer
            {
                public:
                    WireframeLayer(Canvas*);
                    operator Widget*();

                    void on_layer_resolution_changed();

                    void set_scale(float);
                    void set_offset(Vector2f);
                    
                    // all texture-space coords
                    void set_as_line(Vector2i from, Vector2i to); 
                    void set_as_rectangle(Vector2i a, Vector2i b);
                    void set_as_circle(Vector2i a, Vector2i b);

                    ProjectState::DrawData draw();

                private:
                    enum Mode
                    {
                        LINE,
                        CIRCLE,
                        RECTANGLE,
                    };
                    Mode _current_mode = RECTANGLE;
                    
                    Canvas* _owner;
                    GLArea _area;

                    float _scale = 1;
                    Vector2f _offset = {0, 0};
                    void reformat();

                    Vector2f _canvas_size = {1, 1};
                    static void on_area_realize(Widget* widget, WireframeLayer* instance);
                    static void on_area_resize(GLArea*, int w, int h, WireframeLayer* instance);
                    static bool on_area_render(GLArea*, GdkGLContext*, WireframeLayer* instance);

                    MultisampledRenderTexture _render_texture;
                    Shape* _render_texture_shape = nullptr;
                    RenderTask* _render_texture_task = nullptr;
                    
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
                    Vector2i _line_start_point; // texture space coords
                    Vector2i _line_end_point;
                    
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
                    Vector2i _rectangle_a_point;
                    Vector2i _rectangle_b_point;
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

                    void set_animation_paused(bool);
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

                    Shape* _outline_top = nullptr;
                    Shape* _outline_right = nullptr;
                    Shape* _outline_bottom = nullptr;
                    Shape* _outline_left = nullptr;
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

                private:
                    Canvas* _owner;

                    GLArea _proxy;
                    Vector2f _canvas_size;

                    Vector2i widget_space_pos_to_texture_space_pos(float x, float y);

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
            };

            UserInputLayer* _user_input_layer = new UserInputLayer(this);

            // debug

            CheckButton _background_visible_button;
            Label _background_visible_label = Label("Background Visible: ");
            Button _draw_button;
            Label _draw_button_label = Label("DRAW");
            Box _background_visible_box = Box(GTK_ORIENTATION_HORIZONTAL);

            SpinButton _scale_button = SpinButton(1, 100, 0.1);
            Label _scale_label = Label("Scale: ");

            SpinButton _x_offset_button = SpinButton(-1000, +1000, 1);
            SpinButton _y_offset_button = SpinButton(-1000, +1000, 1);
            Label _offset_label = Label("Offset (xy):");
            Box _transform_box = Box(GTK_ORIENTATION_HORIZONTAL);

            CheckButton _grid_visible_button;
            Label _grid_visible_label = Label("Grid: ");
            Box _grid_box = Box(GTK_ORIENTATION_HORIZONTAL);

            CheckButton _h_ruler_active_button;
            SpinButton _h_ruler_button = SpinButton(0, active_state->get_layer_resolution().x - 1, 1);
            CheckButton _v_ruler_active_button;
            SpinButton _v_ruler_button = SpinButton(0, active_state->get_layer_resolution().y - 1, 1);
            Label _ruler_label = Label("Symmetry (hv): ");
            Box _ruler_box = Box(GTK_ORIENTATION_HORIZONTAL);

            CheckButton _brush_outline_button;
            SpinButton _brush_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
            SpinButton _brush_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);
            Label _brush_label = Label("Brush (xy): ");
            Box _brush_box = Box(GTK_ORIENTATION_HORIZONTAL);

            CheckButton _line_visible_button;
            SpinButton _line_start_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
            SpinButton _line_start_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);
            SpinButton _line_end_x_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().x + 100, 1);
            SpinButton _line_end_y_pos_button = SpinButton(0 - 100, active_state->get_layer_resolution().y + 100, 1);
            Label _line_label = Label("Line (xy -> xy): ");
            Box _line_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Box _debug_box = Box(GTK_ORIENTATION_VERTICAL);

            // main layout

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
            Overlay _layer_overlay;

    };

    namespace state
    {
        inline Canvas* canvas = nullptr;
    }

}
/*
class Canvas : public AppComponent
{
    public:
        Canvas();
        operator Widget*();
        void update();

        void set_transform_offset(Vector2f);
        Vector2f get_transform_offset() const;

        void set_transform_scale(float);
        float get_transform_scale() const;

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
        float _transform_scale_step = state::settings_file->get_value_as<float>("canvas", "scale_step");
        GLTransform* _transform = new GLTransform();

        // layer handling user input
        struct ControlLayer : public CanvasLayer
        {
            public:
                ControlLayer(Canvas*);
                ~ControlLayer() = default;

                operator Widget*() override;
                void update() override;

            private:
                GLArea _area;

                void update_pixel_position();
                static void on_realize(Widget*, ControlLayer* instance);
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
                static bool on_modifiers_changed(KeyEventController*, GdkModifierType keyval, ControlLayer* instance);

                size_t _scroll_scale_trigger;
                size_t _lock_axis_trigger;
                std::map<guint, std::vector<guint>> _trigger_hash_to_allowed_keys;

                bool should_trigger_scroll_scale(guint keyval);
                bool should_trigger_lock_axis(guint keyval);

                bool _scroll_scale_active = false;
                bool _lock_axis_active = false;
                Vector2f _lock_axis_anchor_point;

                ScrollEventController _scroll_controller;
                static void on_scroll_begin(ScrollEventController*, ControlLayer* instance);
                static void on_scroll(ScrollEventController*, double x, double y, ControlLayer* instance);
                static void on_scroll_end(ScrollEventController*, ControlLayer* instance);

                bool _translation_scroll_x_inverted = state::settings_file->get_value_as<bool>("canvas", "translation_scroll_x_inverted");
                bool _translation_scroll_y_inverted = state::settings_file->get_value_as<bool>("canvas", "translation_scroll_y_inverted");
                float _translation_scroll_sensitivity = state::settings_file->get_value_as<float>("canvas", "translation_scroll_sensitivity");

                bool _scale_scroll_inverted = state::settings_file->get_value_as<bool>("canvas", "scale_scroll_inverted");
                float _scale_scroll_sensitivity = state::settings_file->get_value_as<float>("canvas", "scale_scroll_sensitivity");

                inline Action _scale_step_up_action);
                inline Action _scale_step_down_action);
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

                static inline int* _cursor_outline_shader_right_flag = new int(1);
                static inline int* _cursor_outline_shader_top_flag = new int(2);
                static inline int* _cursor_outline_shader_left_flag = new int(3);
                static inline int* _cursor_outline_shader_bottom_flag = new int(4);
                static inline float* _cursor_outline_time_s = new float(0);

                Shape* _cursor_shape = nullptr;
                Shape* _cursor_outline_top = nullptr;
                Shape* _cursor_outline_right = nullptr;
                Shape* _cursor_outline_bottom = nullptr;
                Shape* _cursor_outline_left = nullptr;
                Shape* _cursor_outline_outline = nullptr;

                Vector2f _canvas_size = Vector2f(1, 1);
                Vector2f _cursor_position = Vector2f(0, 0);
                float _transform_scale = 1;

                float _brush_opacity;
                HSVA _brush_color;
                Brush* _current_brush;
                bool _cursor_in_bounds = false;

                void reformat();
                void reschedule_render_tasks();

                static void on_realize(Widget*, BrushCursorLayer* instance);
                static void on_resize(GLArea*, int, int, BrushCursorLayer* instance);
        };

        BrushCursorLayer _brush_cursor_layer = BrushCursorLayer(this);

        // LINES

        class LineToolLayer : public CanvasLayer
        {
            public:
                LineToolLayer(Canvas*);

                operator Widget*() override;
                void update() override;

                void signal_click();

            private:
                GLArea _area;

                bool _anchored = false;
                Vector2i _start_point;
                Vector2i _end_point;

                Shape* _start_shape = nullptr;
                Shape* _end_shape = nullptr;

                Shape* _line_shape;
                Shape* _line_outline;

                static void on_realize(Widget*, LineToolLayer* instance);
                static void on_resize(GLArea*, int, int, LineToolLayer* instance);

                Vector2f _canvas_size = Vector2f(1, 1);
        };

        LineToolLayer _line_tool_layer = LineToolLayer(this);

        // SELECTION

        class SelectionLayer : public CanvasLayer
        {
            public:
                SelectionLayer(Canvas*);

                operator Widget*() override;
                void update() override;

            private:
                GLArea _area;

                static inline int* _outline_shader_right_flag = new int(1);
                static inline int* _outline_shader_top_flag = new int(2);
                static inline int* _outline_shader_left_flag = new int(3);
                static inline int* _outline_shader_bottom_flag = new int(4);
                static inline float* _outline_time_s = new float(0);

                Vector2f* _canvas_size = new Vector2f(1, 1);
                Shader* _outline_shader;

                Shape* _outline_top = nullptr;
                Shape* _outline_right = nullptr;
                Shape* _outline_bottom = nullptr;
                Shape* _outline_left = nullptr;
                Shape* _outline_outline = nullptr;

                Vector2f _outline_top_initial_position;
                Vector2f _outline_right_initial_position;
                Vector2f _outline_bottom_initial_position;
                Vector2f _outline_left_initial_position;
                Vector2f _outline_outline_initial_position;

                void reschedule_render_tasks();
                void reformat();

                static void on_realize(Widget*, SelectionLayer* instance);
                static void on_resize(GLArea*, int, int, SelectionLayer* instance);
        };

        SelectionLayer _selection_layer = SelectionLayer(this);

        // UNDO / REDO

        enum class BackupMode
        {
            UNDO,
            REDO,
            NONE
        };

        struct VectorCompare
        {
            bool operator()(const Vector2i& a, const Vector2i& b) const
            {
                if (a.x == b.x)
                    return a.y < b.y;
                else
                    return a.x < b.x;
            }
        };
        std::set<Vector2i, VectorCompare> _nodraw_set; // pixels already drawn this stroke

        /// \param mode: if undo, inverse action is pushed to undo queue, else pushed to redo queue
        void draw(const SubImage&, BlendMode mode, BackupMode = BackupMode::NONE);
        void add_brush_to_subimage(Vector2i brush_position, Brush*, RGBA, SubImage& image);
        void undo_safepoint();

        size_t _undo_queue_size_byte = 0;
        std::deque<SubImage> _undo_queue;
        std::deque<SubImage> _redo_queue;

        inline Action _undo_draw_step_action);
        void on_undo_action();

        Action _redo_draw_step_action);
        void on_redo_action();

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
#include <app/canvas/brush_cursor_layer.hpp>
#include <app/canvas/tool_behavior.hpp>
#include <app/canvas/selection_layer.hpp>
#include <app/canvas/line_tool_layer.hpp>

namespace mousetrap
{
// CANVAS

Canvas::Canvas()
{
    _canvas_layer_overlay.set_child(_transparency_tiling_layer);
    _canvas_layer_overlay.add_overlay(_layers_layer);
    _canvas_layer_overlay.add_overlay(_grid_layer);
    _canvas_layer_overlay.add_overlay(_brush_cursor_layer);
    _canvas_layer_overlay.add_overlay(_line_tool_layer);
    _canvas_layer_overlay.add_overlay(_selection_layer);

    _canvas_layer_overlay.add_overlay(_control_layer);

    set_transform_scale(12);

    // setup undo / redo architecture

    _undo_draw_step_action.set_function([](Canvas* instance) -> void {
        instance->on_undo_action();
    }, this);
    _undo_draw_step_action.add_shortcut(state::keybindings_file->get_value("canvas", "undo_draw_step"));
    state::app->add_action(_undo_draw_step_action);

    _redo_draw_step_action.set_function([](Canvas* instance) -> void {
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

    operator Widget*()->add_tick_callback([](FrameClock, Canvas* instance) -> bool {
        instance->update();
        return false;
    }, this);
}

void Canvas::on_undo_action()
{
    if (_undo_queue.empty() or _undo_queue.back().is_empty())
        return;

    _redo_queue.emplace_back();

    _nodraw_set.clear();
    draw(_undo_queue.back(), BlendMode::NONE, BackupMode::REDO);
    _nodraw_set.clear();

    _undo_queue.pop_back();
}

void Canvas::on_redo_action()
{
    if (_redo_queue.empty() or _redo_queue.back().is_empty())
        return;

    _nodraw_set.clear();

    _undo_queue.emplace_back();

    _nodraw_set.clear();
    draw(_redo_queue.back(), BlendMode::NONE, BackupMode::UNDO);
    _nodraw_set.clear();

    _redo_queue.pop_back();
}

Canvas::operator Widget*()
{
    return &_canvas_layer_overlay;
}

float Canvas::get_transform_scale() const
{
    return *_transform_scale;
}

void Canvas::set_transform_scale(float scale)
{
    *_transform_scale = scale <= 0 ? 0.001 : scale;
    _transform->reset();
    _transform->translate(*_transform_offset);
    _transform->scale(*_transform_scale, *_transform_scale);
}

void Canvas::set_transform_offset(Vector2f xy)
{
    *_transform_offset = xy;
    _transform->reset();
    _transform->translate(*_transform_offset);
    _transform->scale(*_transform_scale, *_transform_scale);
}

Vector2f Canvas::get_transform_offset() const
{
    return *_transform_offset;
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
    auto& frame = active_state->layers.at(active_state->current_layer)->frames.at(active_state->get_current_frame_index());
    auto& image = *frame.image;

    for (auto& it : subimage)
    {
        auto pos = Vector2i(it.x, it.y);

        if (pos.x < 0 or pos.x >= frame.image->get_size().x or pos.y < 0 or pos.y >= frame.image->get_size().y)
            continue;

        if (_nodraw_set.find(pos) != _nodraw_set.end())
            continue;

        if (backup_mode == BackupMode::UNDO)
            _undo_queue.back().add(pos, frame.image->get_pixel(pos.x, pos.y));
        else if (backup_mode == BackupMode::REDO)
            _redo_queue.back().add(pos, frame.image->get_pixel(pos.x, pos.y));

        frame.draw_pixel(pos, it.color, blend_mode);
        _nodraw_set.insert(pos);
    }

    frame.update_texture();
    _layers_layer.update();
}

void Canvas::draw_brush(Vector2i position, Brush* brush, RGBA color)
{
    auto to_draw = SubImage();
    add_brush_to_subimage(position, brush, color, to_draw);
    draw(to_draw, BlendMode::NORMAL, BackupMode::UNDO);
}

void Canvas::draw_brush_line(Vector2i a, Vector2i b, Brush* brush, RGBA color)
{
    auto to_draw = SubImage();

    auto& frame = active_state->layers.at(active_state->current_layer)->frames.at(active_state->get_current_frame_index());
    auto& brush_image = brush->get_image();
    auto points = generate_line_points(a, b);

    for (auto& position : points)
        add_brush_to_subimage(position, brush, color, to_draw);

    draw(to_draw, BlendMode::NORMAL, BackupMode::UNDO);
}

void Canvas::update()
{
    // TODO
    return;
    // TODO

    _control_layer.update();
    _brush_cursor_layer.update();
    _line_tool_layer.update();
    _transparency_tiling_layer.update();
    _layers_layer.update();
    _grid_layer.update();
    _selection_layer.update();
}
}
 */