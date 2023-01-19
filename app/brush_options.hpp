//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/9/23
//
#pragma once

#include <mousetrap.hpp>
#include <app/brush.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/tooltip.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/app_signals.hpp>

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

    class BrushOptions : public AppComponent, public HasBrushSelectionUpdatedSignal
    {
        public:
            BrushOptions();

            void update();
            operator Widget*();

            size_t get_preview_size() const;
            void set_preview_size(size_t);

            void set_brush_size(size_t);
            size_t get_brush_size() const;
            void set_brush_opacity(float);
            float get_brush_opacity() const;

        private:
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
                    /// \brief algorithmic brushes
                    BrushPreview(const std::string& name, const std::string& shader_id);

                    /// \brief texture-based custom brushes
                    BrushPreview(const std::string& name, Image&);
                    ~BrushPreview();

                    void set_preview_size(size_t);
                    operator Widget*();

                    /// \returns -1 if algorithmic, max(texture->size) otherwise
                    int get_base_size() const;

                private:
                    int _size;
                    std::string _name;

                    Overlay _overlay;
                    Label _label;
                    Frame _frame;
                    AspectFrame _aspect_frame = AspectFrame(1);
                    GLArea _area;
                    static void on_realize(Widget* widget, BrushPreview* instance);

                    Shader* _shader;
                    std::string _shader_path = "";

                    Image* _image;
                    Texture* _texture;

                    Shape* _shape;
                    Shape* _background;
            };

            std::vector<BrushPreview*> _brush_previews;

            // Menu

            MenuModel _menu;
            MenuButton _menu_button;
            Label _menu_button_label = Label("Brushes");
            PopoverMenu _menu_button_popover = PopoverMenu(&_menu);

            size_t _preview_size = state::settings_file->get_value_as<size_t>("brush_options", "brush_preview_size");

            Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _preview_size_label = Label("Preview Size (px): ");
            SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);
            static void on_preview_size_spin_button_value_changed(SpinButton*, BrushOptions* instance);

            // Layout

            void initialize_brushes();

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            GridView _brush_preview_list = GridView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
            static void on_brush_preview_list_selection_changed(SelectionModel*, size_t first_item_position, size_t, BrushOptions* instance);

            ScrolledWindow _brush_preview_window;
            Frame _brush_preview_list_frame;

            Tooltip _tooltip;
    };

    namespace state
    {
        static inline BrushOptions* brush_options = nullptr;
    }
}
