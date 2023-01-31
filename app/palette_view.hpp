//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/21/23
//

#pragma once

#include <app/app_component.hpp>
#include <app/palette.hpp>
#include <app/project_state.hpp>
#include <app/tooltip.hpp>
#include <app/app_signals.hpp>
#include <app/file_chooser_dialog.hpp>

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

        DECLARE_GLOBAL_ACTION(palette_view, toggle_palette_locked);
    }

    class PaletteView : public AppComponent,
            public signals::ColorSelectionChanged,
            public signals::PaletteUpdated,
            public signals::PaletteSortModeChanged,
            public signals::PaletteEditingToggled
    {
        public:
            PaletteView();
            ~PaletteView();
            operator Widget*() override;

            void set_preview_size(size_t);
            size_t get_preview_size() const;

        private:
            void on_color_selection_changed();
            void on_palette_updated();
            void on_palette_sort_mode_changed();
            void on_palette_editing_toggled();

            size_t _preview_size = state::settings_file->get_value_as<size_t>("palette_view", "color_preview_size");

            class ColorTile
            {
                public:
                    ColorTile(PaletteView* owner, HSVA color);
                    ~ColorTile();
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

                    bool _selected = false;
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

                    void set_palette_editing_enabled(bool);

                private:
                    PaletteView* _owner;

                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    ImageDisplay _palette_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_locked.png");
                    ImageDisplay _palette_not_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_not_locked.png");
                    ToggleButton _palette_locked_button;
                    static void on_palette_locked_button_toggled(ToggleButton*, PaletteControlBar* instance);

                    Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _preview_size_label = Label("Preview Size (px): ");
                    SeparatorLine _preview_size_spacer;
                    SpinButton _preview_size_scale = SpinButton(2, 512, 1);
                    static void on_preview_size_scale_value_changed(SpinButton*, PaletteControlBar* instance);

                    Label _menu_button_label = Label("Palette");
                    MenuButton _menu_button;
                    MenuModel _menu;
            };

            PaletteControlBar _palette_control_bar = PaletteControlBar(this);

            class PaletteFileSelectOpen
            {
                public:
                    PaletteFileSelectOpen(PaletteView*);
                    void show();

                private:
                    PaletteView* _owner;
                    OpenFileDialog _dialog;
            };
            PaletteFileSelectOpen _palette_file_select_open = PaletteFileSelectOpen(this);

            class PaletteFileSelectSave
            {
                public:
                    PaletteFileSelectSave(PaletteView*);
                    void show();

                private:
                    PaletteView* _owner;
                    SaveAsFileDialog _dialog;
            };

            std::string _palette_file_save_path = "";
            void save_palette_to_file(const std::string& path);
            PaletteFileSelectSave _palette_file_select_save = PaletteFileSelectSave(this);

            std::vector<ColorTile*> _color_tiles;
            GridView _color_tile_view = GridView(GTK_SELECTION_SINGLE);
            static void on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance);
            ScrolledWindow _scrolled_window;

            size_t _selected_i = 0;
            Box _palette_view_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            static bool colors_equal(HSVA a, HSVA b);

            Tooltip _tooltip;
    };

    namespace state
    {
        inline PaletteView* palette_view = nullptr;
    }

    namespace state::actions::detail
    {
        inline std::vector<std::vector<HSVA>> palette_view_load_undo_backup = {};
        inline std::vector<HSVA> palette_view_load_redo_palette = {};
    }
}
