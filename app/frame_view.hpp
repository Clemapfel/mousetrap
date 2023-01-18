// 
// Copyright 2022 Clemens Cords
// Created on 12/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/global_state.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(frame_view, toggle_onionskin_visible);
        DECLARE_GLOBAL_ACTION(frame_view, increase_n_onionskin_layers);
        DECLARE_GLOBAL_ACTION(frame_view, decrease_n_onionskin_layers);
        DECLARE_GLOBAL_ACTION(frame_view, jump_to_start);
        DECLARE_GLOBAL_ACTION(frame_view, jump_to_end);
        DECLARE_GLOBAL_ACTION(frame_view, go_to_previous_frame);
        DECLARE_GLOBAL_ACTION(frame_view, go_to_next_frame);
        DECLARE_GLOBAL_ACTION(frame_view, play_pause);
        DECLARE_GLOBAL_ACTION(frame_view, frame_move_right);
        DECLARE_GLOBAL_ACTION(frame_view, frame_move_left);
        DECLARE_GLOBAL_ACTION(frame_view, frame_new_left_of_current);
        DECLARE_GLOBAL_ACTION(frame_view, frame_new_right_of_current);
        DECLARE_GLOBAL_ACTION(frame_view, frame_delete);
        DECLARE_GLOBAL_ACTION(frame_view, frame_make_keyframe_inbetween);
    }
    
    class FrameView : public AppComponent
    {
        public:
            FrameView();

            void update() override;
            operator Widget*() override;

            size_t get_preview_size() const;
            void set_preview_size(size_t);

        private:
            class FramePreview
            {
                public:
                    FramePreview(FrameView* owner, Layer* layer, size_t frame_i);

                    void update();
                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);
                    void set_is_inbetween(bool);
                    void set_preview_size(size_t);

                private:
                    FrameView* _owner;
                    Layer* _layer;
                    size_t _frame_i;

                    AspectFrame _aspect_frame;
                    GLArea _area;

                    static inline Shader* _transparency_tiling_shader = nullptr;
                    Vector2f _canvas_size = Vector2f(1, 1);

                    Shape* _transparency_tiling_shape = nullptr;
                    Shape* _layer_shape = nullptr;

                    static void on_realize(Widget*, FramePreview*);
                    static void on_resize(GLArea*, int w, int h, FramePreview*);

                    bool _is_inbetween;
                    Label _inbetween_label = Label("&#8943;");
                    Overlay _inbetween_label_overlay;
            };

            size_t _preview_size = state::settings_file->get_value_as<int>("frame_view", "frame_preview_size");

            class FrameColumn
            {
                public:
                    FrameColumn(FrameView*, size_t frame_i);
                    ~FrameColumn();

                    operator Widget*();
                    void update();

                    void set_frame(size_t i);
                    void select_layer(size_t i);

                    void set_is_first_frame(bool);
                    void set_preview_size(size_t);

                private:
                    FrameView* _owner;

                    size_t _frame_i;
                    ListView _list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
                    static void on_selection_changed(SelectionModel*, size_t i, size_t n_items, FrameColumn* instance);

                    struct PreviewElement
                    {
                        FramePreview preview;
                        ListView wrapper;
                        Frame frame;
                        Label frame_label;
                        Label layer_label;
                        SeparatorLine layer_label_spacer;
                        Box wrapper_box;
                    };

                    std::vector<PreviewElement*> _preview_elements;
            };

            class ControlBar
            {
                public:
                    ControlBar(FrameView* owner);
                    operator Widget*();

                private:
                    FrameView* _owner;

                    ToggleButton _toggle_onionskin_visible_button;
                    ImageDisplay _toggle_onionskin_visible_icon = ImageDisplay(get_resource_path() + "icons/toggle_onionskin_visible.png");
                    SpinButton _onionskin_n_layers_spin_button = SpinButton(0, 99, 1);
                    static void on_onionskin_spin_button_value_changed(SpinButton*, ControlBar* instance);

                    void on_toggle_onionskin_visible();
                    void set_n_onionskin_layers(size_t);

                    
                    Button _jump_to_start_button;
                    ImageDisplay _jump_to_start_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                    void on_jump_to_start();

                    Button _jump_to_end_button;
                    ImageDisplay _jump_to_end_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_end.png");
                    void on_jump_to_end();

                    Button _go_to_previous_frame_button;
                    ImageDisplay _go_to_previous_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_previous_frame.png");
                    void on_go_to_previous_frame();

                    Button _go_to_next_frame_button;
                    ImageDisplay _go_to_next_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_next_frame.png");
                    void on_go_to_next_frame();

                    Button _play_pause_button;
                    ImageDisplay _play_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_play.png");
                    ImageDisplay _pause_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_pause.png");
                    void on_play_pause();

                    Button _frame_move_right_button;
                    ImageDisplay _frame_move_right_icon = ImageDisplay(get_resource_path() + "icons/frame_move_right.png");
                    void on_frame_move_right();

                    Button _frame_move_left_button;
                    ImageDisplay _frame_move_left_icon = ImageDisplay(get_resource_path() + "icons/frame_move_left.png");
                    void on_frame_move_left();

                    Button _frame_new_left_of_current_button;
                    ImageDisplay _frame_new_left_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_left_of_current.png");
                    void on_frame_new_left_of_current();

                    Button _frame_new_right_of_current_button;
                    ImageDisplay _frame_new_right_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_right_of_current.png");
                    void on_frame_new_right_of_current();

                    Button _frame_delete_button;
                    ImageDisplay _frame_delete_icon = ImageDisplay(get_resource_path() + "icons/frame_delete.png");
                    void on_frame_delete();

                    Button _frame_make_keyframe_button;
                    ImageDisplay _frame_is_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    ImageDisplay _frame_is_not_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    void on_frame_make_keyframe_inbetween();

                    size_t _preview_size = state::settings_file->get_value_as<int>("frame_view", "frame_preview_size");
                    MenuModel _preview_size_submenu;

                    Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _preview_size_label = Label("Preview Size (px): ");
                    SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);
                    static void on_preview_size_spin_button_value_changed(SpinButton*, ControlBar* instance);
                    
                    MenuButton _menu_button;
                    Label _menu_button_label = Label("Frames");
                    MenuModel _menu;
                    PopoverMenu _popover_menu = PopoverMenu(&_menu);

                    Tooltip _tooltip;

                    Box _box = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            ControlBar _control_bar = ControlBar(this);

            size_t _selected_layer_i = 0;
            size_t _selected_frame_i = 0;
            void set_selection(size_t layer_i, size_t frame_i);

            std::vector<FrameColumn*> _frame_columns;

            ListView _frame_column_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
            static void on_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items, FrameView* instance);

            Box _frame_colum_list_view_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _frame_colum_list_view_window_spacer;

            ScrolledWindow _frame_column_list_view_window;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };

    namespace state
    {
        static inline FrameView* frame_view = nullptr;
    }
}
