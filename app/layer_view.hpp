// 
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
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
        DECLARE_GLOBAL_ACTION(layer_view, layer_move_up);
        DECLARE_GLOBAL_ACTION(layer_view, layer_move_down);
        DECLARE_GLOBAL_ACTION(layer_view, layer_new_above_current);
        DECLARE_GLOBAL_ACTION(layer_view, layer_new_below_current);

        DECLARE_GLOBAL_ACTION(layer_view, layer_duplicate);
        DECLARE_GLOBAL_ACTION(layer_view, layer_delete);
        DECLARE_GLOBAL_ACTION(layer_view, layer_merge_down);
        DECLARE_GLOBAL_ACTION(layer_view, layer_flatten_all);
        DECLARE_GLOBAL_ACTION(layer_view, layer_create_from_visible);
        DECLARE_GLOBAL_ACTION(layer_view, set_layer_visible);
        DECLARE_GLOBAL_ACTION(layer_view, set_layer_locked);

        DECLARE_GLOBAL_ACTION(layer_view, show_all_other_layers);
        DECLARE_GLOBAL_ACTION(layer_view, hide_all_other_layers);
    }

    namespace state
    {
        void initialize_layer_view_actions();
        void update_layer_view_actions_inhibition();

        void swap_layers_at_position(size_t a, size_t b);
    }

    class LayerView : public AppComponent
    {
        public:
            LayerView();
            
            void update() override;
            operator Widget*() override;

            size_t get_preview_size() const;
            void set_preview_size(size_t);
            
        private:
            class LayerPreview
            {
                public:
                    LayerPreview(Layer*, size_t frame_i);
                    ~LayerPreview();

                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);
                    void set_preview_size(size_t);
                    void set_opacity(float);
                    void set_visible(bool);
                    void set_locked(bool);

                    void queue_render();

                private:
                    Layer* _layer;
                    size_t _frame_i;
                    
                    GLArea _area;

                    static inline Shader* _transparency_tiling_shader = nullptr;
                    Vector2f _canvas_size = Vector2f(1, 1);
                    
                    Shape* _transparency_tiling_shape = nullptr;
                    Shape* _layer_shape = nullptr;

                    static void on_realize(Widget*, LayerPreview*);
                    static void on_resize(GLArea*, int w, int h, LayerPreview*);
            };

            class LayerRow
            {
                public:
                    LayerRow(LayerView*, Layer*, size_t frame_i);

                    void update();
                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);

                    void set_opacity(float);
                    void set_visible(bool);
                    void set_locked(bool);
                    void set_blend_mode(BlendMode);
                    void set_name(const std::string&);
                    void set_preview_size(size_t);

                private:
                    LayerView* _owner;
                    Layer* _layer;
                    size_t _frame_i;

                    void signal_layer_updated();

                    Box _inner_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit);
                    Box _outer_box = Box(GTK_ORIENTATION_VERTICAL);

                    LayerPreview _layer_preview;
                    Frame _layer_preview_frame;
                    AspectFrame _layer_preview_aspect_frame = AspectFrame(active_state->layer_resolution.x / float(active_state->layer_resolution.y));
                    ListView _layer_preview_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

                    ToggleButton _is_visible_toggle_button;
                    ImageDisplay _is_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_visible.png");
                    ImageDisplay _is_not_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png");
                    static void on_is_visible_toggle_button_toggled(ToggleButton*, LayerRow* instance);

                    ToggleButton _is_locked_toggle_button;
                    ImageDisplay _is_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_locked.png");
                    ImageDisplay _is_not_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png");
                    static void on_is_locked_toggle_button_toggled(ToggleButton*, LayerRow* instance);

                    Entry _layer_name_entry;
                    static void on_layer_name_entry_activated(Entry*, LayerRow* instance);

                    LevelBar _opacity_level_bar = LevelBar(0, 1);
                    Overlay _opacity_level_bar_name_entry_overlay;

                    Scale _opacity_scale = Scale(0, 1, 0.01);
                    SpinButton _opacity_spin_button = SpinButton(0, 1, 0.001);
                    Label _opacity_header = Label("Layer Opacity");

                    Box _opacity_scale_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _opacity_box = Box(GTK_ORIENTATION_VERTICAL);

                    Popover _opacity_popover;
                    MenuButton _opacity_menu_button;

                    static void on_opacity_scale_value_changed(Scale*, LayerRow* instance);
                    static void on_opacity_spin_button_value_changed(SpinButton*, LayerRow* instance);

                    DropDown _blend_mode_drop_down;
                    std::vector<Label*> _blend_mode_drop_down_label_items;
                    std::vector<Label*> _blend_mode_drop_down_list_items;

                    // blendmode -> {frame_label item, list item}
                    static inline const std::vector<std::pair<BlendMode, std::pair<std::string, std::string>>> blend_mode_to_label = {
                        {BlendMode::NORMAL,           {"&#945;", "Alpha Blending"}},
                        {BlendMode::ADD,              {"&#43;", "Add"}},
                        {BlendMode::SUBTRACT,         {"&#8722;", "Subtract"}},
                        {BlendMode::REVERSE_SUBTRACT, {"&#8760;", "Reverse Subtract"}},
                        {BlendMode::MULTIPLY,         {"&#215;", "Multiply"}},
                        {BlendMode::MIN,              {"&#60;", "Minimum"}},
                        {BlendMode::MAX,              {"&#62;", "Maximum"}},
                    };

                    Box _visible_locked_buttons_box = Box(GTK_ORIENTATION_HORIZONTAL);
            };
            
            // control bar

            ImageDisplay _layer_move_up_icon = ImageDisplay(get_resource_path() + "icons/layer_move_up.png");
            Button _layer_move_up_button;

            ImageDisplay _layer_move_down_icon = ImageDisplay(get_resource_path() + "icons/layer_move_down.png");
            Button _layer_move_down_button;

            ImageDisplay _layer_create_icon = ImageDisplay(get_resource_path() + "icons/layer_create.png");
            Button _layer_create_button;

            ImageDisplay _layer_duplicate_icon = ImageDisplay(get_resource_path() + "icons/layer_duplicate.png");
            Button _layer_duplicate_button;

            ImageDisplay _layer_delete_icon = ImageDisplay(get_resource_path() + "icons/layer_delete.png");
            Button _layer_delete_button;

            ImageDisplay _layer_merge_down_icon = ImageDisplay(get_resource_path() + "icons/layer_merge_down.png");
            Button _layer_merge_down_button;

            ImageDisplay _layer_flatten_all_icon = ImageDisplay(get_resource_path() + "icons/layer_flatten_all.png");
            Button _layer_flatten_all_button;

            Box _control_bar_box = Box(GTK_ORIENTATION_HORIZONTAL);

            // menu & actions
           
            MenuButton _header_menu_button;
            Label _header_menu_button_label = Label("Layers");

            MenuModel _menu;
            PopoverMenu _header_menu_button_popover = PopoverMenu(&_menu);

            size_t _preview_size = state::settings_file->get_value_as<int>("layer_view", "layer_preview_size");
            MenuModel _preview_size_submenu;

            Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _preview_size_label = Label("Preview Size (px): ");
            SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);
            static void on_preview_size_spin_button_value_changed(SpinButton*, LayerView* instance);

            // layout

            std::deque<LayerRow> _layer_rows;
            ListView _layer_rows_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_MULTIPLE);
            static void on_layer_rows_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView* instance);

            ScrolledWindow _layer_rows_scrolled_window;
            Box _layer_rows_scrolled_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _layer_rows_scrolled_window_spacer;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            Tooltip _tooltip;
    };

    namespace state
    {
        static inline LayerView* layer_view = nullptr;
    }
}
