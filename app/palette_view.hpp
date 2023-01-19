//
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/app_component.hpp>
#include <app/palette.hpp>
#include <app/global_state.hpp>
#include <app/bubble_log_area.hpp>
#include <app/file_chooser_dialog.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(palette_view, load_default);
        DECLARE_GLOBAL_ACTION(palette_view, save);
        DECLARE_GLOBAL_ACTION(palette_view, save_as_default);
        DECLARE_GLOBAL_ACTION(palette_view, load);
        DECLARE_GLOBAL_ACTION(palette_view, save_as);

        DECLARE_GLOBAL_ACTION(palette_view, sort_by_default);
        DECLARE_GLOBAL_ACTION(palette_view, sort_by_hue);
        DECLARE_GLOBAL_ACTION(palette_view, sort_by_saturation);
        DECLARE_GLOBAL_ACTION(palette_view, sort_by_value);

        DECLARE_GLOBAL_ACTION(palette_view, select_color_0);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_1);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_2);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_3);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_4);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_5);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_6);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_7);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_8);
        DECLARE_GLOBAL_ACTION(palette_view, select_color_9);
    }

    class PaletteView : public AppComponent
    {
        public:
            PaletteView();
            operator Widget*() override;
            void update() override;

            void select(size_t i);

            void set_preview_size(size_t);
            size_t get_preview_size() const;

            void set_palette_locked(bool);
            bool get_palette_locked() const;

            void reload();

        protected:
            void load_from_file(const std::string& path);
            void load_as_debug();

            void save_to_file(const std::string& path);

        private:
            bool _palette_locked = state::settings_file->get_value_as<bool>("palette_view", "palette_locked");
            size_t _preview_size = state::settings_file->get_value_as<size_t>("palette_view", "color_preview_size");

            void update_from_palette();

            class ColorTile
            {
                public:
                    ColorTile(PaletteView* owner, HSVA color);
                    operator Widget*();

                    void set_color(HSVA color);
                    void set_size(size_t);
                    HSVA get_color();
                    void set_selected(bool);

                private:
                    PaletteView* _owner;
                    HSVA _color;

                    GLArea _color_area;
                    GLArea _selection_frame_area;

                    Shape* _color_shape = nullptr;
                    Shape* _frame_shape = nullptr;

                    static inline Texture* is_selected_overlay_texture = nullptr;
                    Shape* _is_selected_shape = nullptr;

                    static void on_color_area_realize(Widget* widget, ColorTile* instance);
                    static void on_selection_frame_realize(Widget* widget, ColorTile* instance);

                    AspectFrame _aspect_frame = AspectFrame(1, 1);
                    Overlay _overlay;
            };

            class PaletteControlBar
            {
                public:
                    PaletteControlBar(PaletteView* owner);
                    operator Widget*();
                    void update();

                private:
                    PaletteView* _owner;

                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    ImageDisplay _palette_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_locked.png");
                    ImageDisplay _palette_not_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_not_locked.png");
                    ToggleButton _palette_not_locked_button;
                    static void on_palette_not_locked_button_toggled(ToggleButton*, PaletteControlBar* instance);

                    Box _palette_locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _palette_locked_label = Label("Palette Editing Enabled: ");
                    SeparatorLine _palette_locked_spacer;
                    Switch _palette_locked_switch;
                    static void on_palette_locked_switch_state_set(Switch*, bool, PaletteControlBar* instance);

                    Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _preview_size_label = Label("Preview Size (px): ");
                    SeparatorLine _preview_size_spacer;
                    SpinButton _preview_size_scale = SpinButton(2, 512, 1);
                    static void on_preview_size_scale_value_changed(SpinButton*, PaletteControlBar* instance);

                    void update_palette_locked();
                    void update_preview_size_changed();

                    Label _menu_button_label = Label("Palette");
                    MenuButton _menu_button;
                    MenuModel _menu;
            };

            PaletteControlBar _palette_control_bar = PaletteControlBar(this);

            size_t _selected_i = 0;
            static void on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance);

            std::vector<ColorTile*> _color_tiles;
            GridView _color_tile_view = GridView(GTK_SELECTION_SINGLE);
            ScrolledWindow _scrolled_window;

            Box _palette_view_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            // actions

            OpenFileDialog _on_load_dialog = OpenFileDialog("Load Palette");
            void on_load_ok_pressed();

            SaveAsFileDialog _on_save_as_dialog = SaveAsFileDialog("Save Palette As");
            void on_save_as_ok_pressed();

            std::string _save_to_path = "";
            Tooltip _tooltip;
    };

    namespace state
    {
        static inline PaletteView* palette_view = nullptr;
    }
}
