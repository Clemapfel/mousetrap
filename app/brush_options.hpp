//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/9/23
//
#pragma once

#include <mousetrap.hpp>
#include <app/brush.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    class BrushOptions : public AppComponent
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

            Label _brush_opacity_label = Label("Brush opacity (px)");
            SeparatorLine _brush_opacity_label_spacer;
            Box _brush_opacity_label_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Scale _brush_opacity_scale = Scale(0, 1, 0.0001);
            SpinButton _brush_opacity_spin_button = SpinButton(0, 1, 0.0001);
            Box _brush_opacity_scale_box = Box(GTK_ORIENTATION_HORIZONTAL);

            // Previews

            static inline float background_value = 0.5;
            static inline float brush_margin = 0.1;

            size_t _preview_size = state::settings_file->get_value_as<size_t>("brush_options", "brush_preview_size");
            static std::string make_index_label(const std::string& s)
            {
                return "<span font_scale=\"subscript\" bgcolor=\"black\" bgalpha=\"100%\" fgcolor=\"white\">" + s + "</span>";
            }

            class AlgorithmicBrushPreview
            {
                public:
                    AlgorithmicBrushPreview(const std::string& shader_id);
                    ~AlgorithmicBrushPreview();

                    void set_preview_size(size_t);
                    operator Widget*();

                private:
                    Overlay _overlay;
                    Label _label = Label(make_index_label("&#8734;"));
                    Frame _frame;
                    AspectFrame _aspect_frame = AspectFrame(1);
                    GLArea _area;
                    static void on_realize(Widget* widget, AlgorithmicBrushPreview* instance);

                    Shader* _shader;
                    std::string _shader_path;

                    Shape* _shape;
            };

            class CustomBrushPreview
            {

            };

            std::vector<AlgorithmicBrushPreview*> _algorithmic_brushes;

            // Actions

            ShortcutController _shortcut_controller = ShortcutController(state::app);

            Action _increase_brush_size_action = Action("brush_options.increase_brush_size");
            Action _decrease_brush_size_action = Action("brush_options.decrease_brush_size");

            Action _increase_brush_opacity_action = Action("brush_options.increase_brush_opacity");
            Action _decrease_brush_opacity_action = Action("brush_options.decrease_brush_opacity");

            // Menu

            MenuModel _menu;
            MenuButton _menu_button;
            Label _menu_button_label = Label("Brushes");
            PopoverMenu _menu_button_popover = PopoverMenu(&_menu);

            // Layout

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            GridView _brush_preview_list = GridView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
            SeparatorLine _brush_preview_box_spacer;
            Box _brush_preview_box = Box(GTK_ORIENTATION_VERTICAL);
            ScrolledWindow _brush_preview_window;
            Frame _brush_preview_list_frame;

            Tooltip _tooltip;
    };
}

// ###

namespace mousetrap
{
    BrushOptions::BrushOptions()
    {
        _brush_size_label_box.push_back(&_brush_size_label);
        _brush_size_label_box.push_back(&_brush_size_label_spacer);
        _main.push_back(&_brush_size_label_box);

        _brush_size_scale_box.push_back(&_brush_size_scale);
        _brush_size_scale_box.push_back(&_brush_size_spin_button);
        _main.push_back(&_brush_size_scale_box);

        _brush_opacity_label_box.push_back(&_brush_opacity_label);
        _brush_opacity_label_box.push_back(&_brush_opacity_label_spacer);
        _main.push_back(&_brush_opacity_label_box);

        _brush_opacity_scale_box.push_back(&_brush_opacity_scale);
        _brush_opacity_scale_box.push_back(&_brush_opacity_spin_button);
        _main.push_back(&_brush_opacity_scale_box);

        auto spin_button_width = std::max(
            _brush_size_spin_button.get_preferred_size().natural_size.x,
            _brush_opacity_spin_button.get_preferred_size().natural_size.x
        );

        _brush_size_spin_button.set_size_request({spin_button_width, 0});
        _brush_opacity_spin_button.set_size_request({spin_button_width, 0});

        _brush_size_scale.set_hexpand(true);
        _brush_size_spin_button.set_hexpand(false);
        _brush_opacity_scale.set_hexpand(true);
        _brush_opacity_spin_button.set_hexpand(false);

        _brush_opacity_spin_button.set_digits(3);

        _brush_size_scale.connect_signal_value_changed([](Scale* scale, BrushOptions* instance) -> void {
            instance->set_brush_size(scale->get_value());
        }, this);

        _brush_size_spin_button.connect_signal_value_changed([](SpinButton* scale, BrushOptions* instance) -> void {
            instance->set_brush_size(scale->get_value());
        }, this);

        _brush_opacity_scale.connect_signal_value_changed([](Scale* scale, BrushOptions* instance) -> void {
            instance->set_brush_opacity(scale->get_value());
        }, this);

        _brush_opacity_spin_button.connect_signal_value_changed([](SpinButton* scale, BrushOptions* instance) -> void {
            instance->set_brush_opacity(scale->get_value());
        }, this);

        set_brush_opacity(state::brush_opacity);
        set_brush_size(state::brush_size);

        // Actions

        _increase_brush_opacity_action.set_do_function([](BrushOptions* instance){
            instance->set_brush_opacity(instance->get_brush_opacity() + 0.1f);
        }, this);
        _increase_brush_opacity_action.add_shortcut(state::keybindings_file->get_value("brush_options", "increase_brush_opacity"));
        state::app->add_action(_increase_brush_opacity_action);
        _shortcut_controller.add_action(_increase_brush_opacity_action.get_id());

        _decrease_brush_opacity_action.set_do_function([](BrushOptions* instance){
            instance->set_brush_opacity(instance->get_brush_opacity() - 0.1f);
        }, this);
        _decrease_brush_opacity_action.add_shortcut(state::keybindings_file->get_value("brush_options", "decrease_brush_opacity"));
        state::app->add_action(_decrease_brush_opacity_action);
        _shortcut_controller.add_action(_decrease_brush_opacity_action.get_id());

        _increase_brush_size_action.set_do_function([](BrushOptions* instance){
            instance->set_brush_opacity(instance->get_brush_opacity() + 1);
        }, this);
        _increase_brush_size_action.add_shortcut(state::keybindings_file->get_value("brush_options", "increase_brush_size"));
        state::app->add_action(_increase_brush_size_action);
        _shortcut_controller.add_action(_increase_brush_size_action.get_id());

        _decrease_brush_size_action.set_do_function([](BrushOptions* instance){
            instance->set_brush_opacity(instance->get_brush_opacity() - 1);
        }, this);
        _decrease_brush_size_action.add_shortcut(state::keybindings_file->get_value("brush_options", "decrease_brush_size"));
        state::app->add_action(_decrease_brush_size_action);
        _shortcut_controller.add_action(_decrease_brush_size_action.get_id());

        // menu

        auto settings_section = MenuModel();
        settings_section.add_action("Dummy", "app.dummy_action");

        _menu.add_section("Settings", &settings_section);

        _menu_button.set_child(&_menu_button_label);
        _menu_button_popover.refresh_widgets();
        _menu_button.set_popover(&_menu_button_popover);

        _main.push_front(&_menu_button);

        // tooltips

        auto brush_opacity_tooltip = state::tooltips_file->get_value("brush_options", "brush_opacity");
        _brush_opacity_label_box.set_tooltip_text(brush_opacity_tooltip);
        _brush_opacity_scale_box.set_tooltip_text(brush_opacity_tooltip);

        auto brush_size_tooltip = state::tooltips_file->get_value("brush_options", "brush_size");
        _brush_size_label_box.set_tooltip_text(brush_size_tooltip);
        _brush_size_scale_box.set_tooltip_text(brush_size_tooltip);

        _tooltip.create_from("brush_options", state::tooltips_file, state::keybindings_file);
        _main.set_tooltip_widget(_tooltip);

        // brushes

        for (auto id : {
            "brush_circle",
            "brush_square",
            "brush_ellipse_horizontal",
            "brush_ellipse_vertical",
            "brush_line_horizontal",
            "brush_line_vertical"
        })
            _algorithmic_brushes.emplace_back(new AlgorithmicBrushPreview(id));

        for (auto* preview : _algorithmic_brushes)
        {
            _brush_preview_list.push_back(preview->operator Widget*());
            preview->set_preview_size(_preview_size);
        }

        _brush_preview_box.push_back(&_brush_preview_list);
        _brush_preview_box.push_back(&_brush_preview_box_spacer);
        _brush_preview_window.set_child(&_brush_preview_box);
        _brush_preview_list_frame.set_child(&_brush_preview_window);

        _brush_preview_list_frame.set_expand(true);

        _main.push_back(&_brush_preview_list_frame);
    }

    void BrushOptions::set_brush_opacity(float x)
    {
        x = glm::clamp<float>(x, 0, 1);

        state::brush_opacity = x;

        _brush_opacity_scale.set_signal_value_changed_blocked(true);
        _brush_opacity_scale.set_value(x);
        _brush_opacity_scale.set_signal_value_changed_blocked(false);

        _brush_opacity_spin_button.set_signal_value_changed_blocked(true);
        _brush_opacity_spin_button.set_value(x);
        _brush_opacity_spin_button.set_signal_value_changed_blocked(false);
    }

    float BrushOptions::get_brush_opacity() const
    {
        return state::brush_opacity;
    }

    void BrushOptions::set_brush_size(size_t x)
    {
        x = glm::clamp<size_t>(x, 1, max_brush_size);

        state::brush_size = x;

        _brush_size_scale.set_signal_value_changed_blocked(true);
        _brush_size_scale.set_value(x);
        _brush_size_scale.set_signal_value_changed_blocked(false);

        _brush_size_spin_button.set_signal_value_changed_blocked(true);
        _brush_size_spin_button.set_value(x);
        _brush_size_spin_button.set_signal_value_changed_blocked(false);
    }

    size_t BrushOptions::get_brush_size() const
    {
        return state::brush_size;
    }

    BrushOptions::operator Widget*()
    {
        return &_main;
    }

    void BrushOptions::update()
    {

    }

    size_t BrushOptions::get_preview_size() const
    {
        return 50;
    }

    void BrushOptions::set_preview_size(size_t)
    {}

    BrushOptions::AlgorithmicBrushPreview::AlgorithmicBrushPreview(const std::string& shader_id)
        : _shader_path(get_resource_path() + "shaders/" + shader_id + ".frag")
    {
        _area.connect_signal_realize(on_realize, this);
        _frame.set_child(&_area);
        _aspect_frame.set_child(&_frame);

        _label.set_align(GTK_ALIGN_END);

        _overlay.set_child(&_aspect_frame);
        _overlay.add_overlay(&_label);
    }

    BrushOptions::AlgorithmicBrushPreview::~AlgorithmicBrushPreview()
    {
        delete _shader;
        delete _shape;
    }

    BrushOptions::AlgorithmicBrushPreview::operator Widget*()
    {
        return &_overlay;
    }

    void BrushOptions::AlgorithmicBrushPreview::set_preview_size(size_t x)
    {
        _area.set_size_request({x, x});
    }

    void BrushOptions::AlgorithmicBrushPreview::on_realize(Widget* widget, AlgorithmicBrushPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_shader = new Shader();
        instance->_shader->create_from_file(instance->_shader_path, ShaderType::FRAGMENT);
        instance->_shape = new Shape();
        instance->_shape->as_rectangle({0, 0}, {1, 1});

        auto task = RenderTask(instance->_shape, instance->_shader);
        task.register_float("_background_value", &BrushOptions::background_value);
        task.register_float("_margin", &BrushOptions::brush_margin);
        area->clear_render_tasks();
        area->add_render_task(task);
    }
}

namespace mousetrap::state
{
    void reload_brushes()
    {
        state::brushes.clear();

        auto files = get_all_files_in_directory(get_resource_path() + "brushes", false, false);
        std::sort(files.begin(), files.end(), [](FileDescriptor& a, FileDescriptor& b) -> bool {
            return a.get_name() < b.get_name();
        });

        for (auto& file : files)
        {
            auto* brush = new Brush();
            if (brush->create_from_file(file.get_path()))
                state::brushes.emplace_back(brush);
        }

        if (state::brushes.empty())
        {
            auto default_brush_image = Image();
            default_brush_image.create(1, 1, RGBA(1, 1, 1, 1));
            state::brushes.emplace_back(new Brush())->create_from_image(default_brush_image, "default");
        }

        state::current_brush = state::brushes.at(0);

        //if (state::brush_options != nullptr)
          //  ((BrushOptions*) state::brush_options)->reload_default_brushes();
    }

}
