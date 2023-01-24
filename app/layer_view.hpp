// 
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/tooltip.hpp>
#include <app/app_signals.hpp>

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
        DECLARE_GLOBAL_ACTION(layer_view, toggle_layer_visible);
        DECLARE_GLOBAL_ACTION(layer_view, toggle_layer_locked);

        DECLARE_GLOBAL_ACTION(layer_view, show_all_other_layers);
        DECLARE_GLOBAL_ACTION(layer_view, hide_all_other_layers);
    }

    class LayerView : public AppComponent,
        public signals::LayerFrameSelectionChanged,
        public signals::LayerImageUpdated,
        public signals::LayerCountChanged,
        public signals::LayerPropertiesChanged,
        public signals::LayerResolutionChanged
    {
        public:
            LayerView();
            operator Widget*() override;

            void set_preview_size(size_t);

        protected:
            void on_layer_frame_selection_changed() override;
            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;
            void on_layer_resolution_changed() override {
                // TODO
            }
            
        private:
            class LayerPreview
            {
                public:
                    LayerPreview(size_t layer_i);
                    ~LayerPreview();

                    operator Widget*();

                    void set_texture(const Texture*);
                    void set_preview_size(size_t);
                    void set_opacity(float);
                    void set_visible(bool);
                    void set_locked(bool);

                    void queue_render();

                private:
                    size_t _layer_i;

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
                    LayerRow(LayerView*, size_t layer_i);
                    operator Widget*();

                    void set_all_signals_blocked(bool);

                    void set_texture(const Texture*);
                    void set_opacity(float);
                    void set_visible(bool);
                    void set_locked(bool);
                    void set_blend_mode(BlendMode);
                    void set_name(const std::string&);
                    void set_preview_size(size_t);

                private:
                    LayerView* _owner;
                    size_t _layer_i;

                    Box _inner_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit);
                    Box _outer_box = Box(GTK_ORIENTATION_VERTICAL);

                    LayerPreview _layer_preview;
                    AspectFrame _layer_preview_aspect_frame = AspectFrame(active_state->get_layer_resolution().x / float(active_state->get_layer_resolution().y));
                    ListView _layer_preview_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

                    ToggleButton _is_visible_toggle_button;
                    ImageDisplay _is_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_visible.png");
                    ImageDisplay _is_not_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png");

                    ToggleButton _is_locked_toggle_button;
                    ImageDisplay _is_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_locked.png");
                    ImageDisplay _is_not_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png");

                    Entry _layer_name_entry;

                    LevelBar _opacity_level_bar = LevelBar(0, 1);
                    Overlay _opacity_level_bar_name_entry_overlay;

                    Scale _opacity_scale = Scale(0, 1, 0.01);
                    SpinButton _opacity_spin_button = SpinButton(0, 1, 0.001);
                    Label _opacity_header = Label("Layer Opacity");

                    Box _opacity_scale_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _opacity_box = Box(GTK_ORIENTATION_VERTICAL);

                    Popover _opacity_popover;
                    MenuButton _opacity_menu_button;

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

            // layout

            std::deque<LayerRow> _layer_rows;
            ListView _layer_rows_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_MULTIPLE);

            ScrolledWindow _layer_rows_scrolled_window;
            Box _layer_rows_scrolled_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _layer_rows_scrolled_window_spacer;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            Tooltip _tooltip;
    };

    namespace state
    {
        inline LayerView* layer_view = nullptr;
    }
}
