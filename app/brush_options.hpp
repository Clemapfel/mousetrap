//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/9/23
//
#pragma once

#include <mousetrap.hpp>
#include <app/brush.hpp>
#include <app/app_component.hpp>
#include <app/project_state.hpp>
#include <app/tooltip.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/app_signals.hpp>
#include <app/file_chooser_dialog.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(brush_options, increase_brush_size);
        DECLARE_GLOBAL_ACTION(brush_options, decrease_brush_size);

        DECLARE_GLOBAL_ACTION(brush_options, increase_brush_opacity);
        DECLARE_GLOBAL_ACTION(brush_options, decrease_brush_opacity);

        DECLARE_GLOBAL_ACTION(brush_options, add_brush);
        DECLARE_GLOBAL_ACTION(brush_options, remove_brush);
        DECLARE_GLOBAL_ACTION(brush_options, load_default_brushes);

        DECLARE_GLOBAL_ACTION(brush_options, open_default_brush_directory);
    }

    class BrushOptions : public AppComponent,
        public signals::BrushSelectionChanged,
        public signals::BrushSetUpdated
    {
        public:
            BrushOptions();

            operator Widget*() override;

            size_t get_preview_size() const;
            void set_preview_size(size_t);

        protected:
            void on_brush_selection_changed() override;
            void on_brush_set_updated() override;

        private:
            void set_brush_size(size_t);
            size_t get_brush_size() const;
            void set_brush_opacity(float);
            float get_brush_opacity() const;

            // Scales

            size_t max_brush_size = state::settings_file->get_value_as<size_t>("brush_options", "maximum_brush_size");

            Label _brush_size_label = Label("Brush Size (px)");
            SeparatorLine _brush_size_label_spacer;
            Box _brush_size_label_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Scale _brush_size_scale = Scale(1, max_brush_size, 1);
            SpinButton _brush_size_spin_button = SpinButton(1, max_brush_size, 1);
            Box _brush_size_scale_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Label _brush_opacity_label = Label("Brush Opacity");
            SeparatorLine _brush_opacity_label_spacer;
            Box _brush_opacity_label_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Scale _brush_opacity_scale = Scale(0, 1, 0.0001);
            SpinButton _brush_opacity_spin_button = SpinButton(0, 1, 0.0001);
            Box _brush_opacity_scale_box = Box(GTK_ORIENTATION_HORIZONTAL);

            // Open Brush

            class AddBrushDialog
            {
                public:
                    AddBrushDialog();
                    void show();

                private:
                    OpenFileDialog _dialog;
            };

            AddBrushDialog _add_brush_dialog;

            // Previews

            static inline float background_value = 0.3;
            static inline float brush_margin = 0.15;

            static std::string make_index_label(const std::string& s)
            {
                return "<span font_scale=\"subscript\" bgcolor=\"black\" bgalpha=\"100%\" fgcolor=\"white\">" + s + "</span>";
            }

            class BrushPreview
            {
                public:
                    BrushPreview();
                    BrushPreview(const std::string& name, const Image&);
                    ~BrushPreview();

                    void set_preview_size(size_t);
                    operator Widget*();

                    void set_as_algorithmic(const std::string& name, const std::string& shader_id);
                    void set_as_custom(const std::string& name, const Image&);

                private:
                    int _size;
                    std::string _name;

                    Overlay _overlay;
                    Label _label;
                    Frame _frame;
                    AspectFrame _aspect_frame = AspectFrame(1);
                    GLArea _area;
                    static void on_realize(Widget* widget, BrushPreview* instance);

                    Shader* _shader = nullptr;
                    std::string _shader_path = "";
                    Texture* _texture = nullptr;

                    Shape* _shape = nullptr;
                    Shape* _background = nullptr;
            };

            std::deque<BrushPreview> _brush_previews;

            // Menu

            MenuModel _menu;
            MenuButton _menu_button;
            Label _menu_button_label = Label("Brushes");
            PopoverMenu _menu_button_popover = PopoverMenu(&_menu);

            size_t _preview_size = state::settings_file->get_value_as<size_t>("brush_options", "brush_preview_size");

            Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _preview_size_label = Label("Preview Size (px): ");
            SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);

            // Layout

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            GridView _brush_preview_list = GridView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);

            ScrolledWindow _brush_preview_window;
            Frame _brush_preview_list_frame;

            Tooltip _tooltip;
    };

    namespace state
    {
        inline BrushOptions* brush_options = nullptr;
    }
}
