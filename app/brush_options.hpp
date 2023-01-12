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
}

// ###

namespace mousetrap
{
    void BrushOptions::initialize_brushes()
    {
        for (auto* preview : _brush_previews)
            delete preview;

        _brush_preview_list.clear();
        _brush_previews.clear();

        for (auto* brush : state::brushes)
        {
            auto shape = brush->get_shape();
            if (shape == BrushShape::CUSTOM)
            {
                _brush_previews.emplace_back(new BrushPreview(brush->get_name(), brush->get_image()));
                continue;
            }

            // user shader instead of texture if algorithmic
            std::stringstream name;
            name << "brush_";
            for (auto c : brush_shape_to_string(shape))
                name << (char) std::tolower(c);

            _brush_previews.emplace_back(new BrushPreview(brush->get_name(), name.str()));
        }

        for (auto* preview : _brush_previews)
        {
            _brush_preview_list.push_back(preview->operator Widget*());
            preview->set_preview_size(_preview_size);
        }
    }

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

        _preview_size_spin_button.set_margin_start(state::margin_unit);
        _preview_size_spin_button.set_value(_preview_size);
        _preview_size_spin_button.connect_signal_value_changed(on_preview_size_spin_button_value_changed, this);
        _preview_size_box.push_back(&_preview_size_label);

        auto spacer = SeparatorLine();
        spacer.set_opacity(0);
        spacer.set_hexpand(true);
        _preview_size_box.push_back(&spacer);

        _preview_size_box.push_back(&_preview_size_spin_button);
        _preview_size_box.set_margin(state::margin_unit);

        auto settings_section = MenuModel();
        auto preview_size_submenu = MenuModel();
        preview_size_submenu.add_widget(&_preview_size_box);
        settings_section.add_submenu("Preview Size...", &preview_size_submenu);
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

        initialize_brushes();

        // layout

        _brush_preview_list.get_selection_model()->connect_signal_selection_changed(on_brush_preview_list_selection_changed, this);
        _brush_preview_window.set_child(&_brush_preview_list);
        _brush_preview_list_frame.set_child(&_brush_preview_window);

        _brush_preview_list_frame.set_expand(true);
        _main.push_back(&_brush_preview_list_frame);

        _brush_opacity_label_spacer.set_opacity(0);
        _brush_size_label_spacer.set_opacity(0);

        _brush_size_scale.set_hexpand(true);
        _brush_size_spin_button.set_hexpand(false);
        _brush_opacity_scale.set_hexpand(true);
        _brush_opacity_spin_button.set_hexpand(false);

        _menu_button.set_margin_bottom(state::margin_unit);

        _brush_opacity_label_box.set_margin_start(state::margin_unit);
        _brush_opacity_scale_box.set_margin_start(2 * state::margin_unit);
        _brush_opacity_scale_box.set_margin_end(state::margin_unit);

        _brush_size_label_box.set_margin_start(state::margin_unit);
        _brush_size_scale_box.set_margin_start(2 * state::margin_unit);
        _brush_size_scale_box.set_margin_end(state::margin_unit);

        _brush_preview_list_frame.set_margin_horizontal(state::margin_unit);
        _brush_preview_list_frame.set_margin_top(state::margin_unit);
    }

    void BrushOptions::on_preview_size_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->set_preview_size(scale->get_value());
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

    void BrushOptions::set_preview_size(size_t v)
    {
        _preview_size = v;
        for (auto* preview : _brush_previews)
            preview->set_preview_size(v);
    }

    BrushOptions::BrushPreview::BrushPreview(const std::string& name, const std::string& shader_id)
        : _name(name), _image(nullptr), _size(-1), _label(BrushOptions::make_index_label("&#8734;")), _shader_path(get_resource_path() + "shaders/" + shader_id + ".frag")
    {
        _area.connect_signal_realize(on_realize, this);
        _frame.set_child(&_area);
        _aspect_frame.set_child(&_frame);

        _label.set_align(GTK_ALIGN_END);

        _overlay.set_child(&_aspect_frame);
        _overlay.add_overlay(&_label);

        std::stringstream tooltip_text;
        tooltip_text << "<b>Brush \"" + _name + "\"</b>" << std::endl;
        tooltip_text << "Base Size: Any";

        operator Widget*()->set_tooltip_text(tooltip_text.str());
    }

    BrushOptions::BrushPreview::BrushPreview(const std::string& name, Image& image)
        : _name(name),
          _image(&image),
          _size(std::max(image.get_size().x, image.get_size().y)),
          _label(BrushOptions::make_index_label(std::to_string(_size)))
    {
        _area.connect_signal_realize(on_realize, this);
        _frame.set_child(&_area);
        _aspect_frame.set_child(&_frame);

        _label.set_align(GTK_ALIGN_END);

        _overlay.set_child(&_aspect_frame);
        _overlay.add_overlay(&_label);

        std::stringstream tooltip_text;
        tooltip_text << "<b>Brush \"" + _name + "\"</b>" << std::endl;
        tooltip_text << "Base Size: " << _size;

        operator Widget*()->set_tooltip_text(tooltip_text.str());
    }

    BrushOptions::BrushPreview::~BrushPreview()
    {
        delete _shader;
        delete _shape;
    }

    BrushOptions::BrushPreview::operator Widget*()
    {
        return &_overlay;
    }

    int BrushOptions::BrushPreview::get_base_size() const
    {
        return _size;
    }

    void BrushOptions::BrushPreview::set_preview_size(size_t x)
    {
        _area.set_size_request({x, x});
    }

    void BrushOptions::BrushPreview::on_realize(Widget* widget, BrushPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (instance->_image != nullptr)
        {
            instance->_texture = new Texture();
            instance->_texture->create_from_image(*instance->_image);
        }

        instance->_shape = new Shape();
        instance->_shape->as_rectangle(Vector2f(BrushOptions::brush_margin), Vector2f(1 - 2 * BrushOptions::brush_margin));
        instance->_background = new Shape();
        instance->_background->as_rectangle({0, 0}, {1, 1});
        instance->_background->set_color(HSVA(0, 0, BrushOptions::background_value, 1));

        area->clear_render_tasks();
        area->add_render_task(instance->_background);

        if (instance->_shader_path.empty())
        {
            instance->_shape->set_texture(instance->_texture);
            area->add_render_task(instance->_shape);
        }
        else
        {
            instance->_shader = new Shader();
            instance->_shader->create_from_file(instance->_shader_path, ShaderType::FRAGMENT);
            auto task = RenderTask(instance->_shape, instance->_shader, nullptr, BlendMode::NORMAL);
            task.register_float("_half_margin", &BrushOptions::brush_margin);
            area->add_render_task(task);
        }

        area->queue_render();
    }

    void BrushOptions::on_brush_preview_list_selection_changed(SelectionModel*, size_t first_item_position, size_t, BrushOptions* instance)
    {
        auto* preview = instance->_brush_previews.at(first_item_position);

        if (preview->get_base_size() > 0)
            instance->set_brush_size(preview->get_base_size());
    }
}

namespace mousetrap::state
{
    void reload_brushes()
    {
        state::brushes.clear();

        state::brushes.emplace_back(new Brush())->as_circle();
        state::brushes.emplace_back(new Brush())->as_square();
        state::brushes.emplace_back(new Brush())->as_ellipse_horizontal();
        state::brushes.emplace_back(new Brush())->as_ellipse_vertical();
        state::brushes.emplace_back(new Brush())->as_rectangle_horizontal();
        state::brushes.emplace_back(new Brush())->as_rectangle_vertical();

        auto files = get_all_files_in_directory(get_resource_path() + "brushes", false, false);
        std::sort(files.begin(), files.end(), [](FileDescriptor& a, FileDescriptor& b) -> bool {
            return a.get_name() < b.get_name();
        });

        for (auto& file : files)
        {
            auto* brush = new Brush();
            auto image = Image();
            if (image.create_from_file(file.get_path()))
            {
                brush->as_custom(image);
                state::brushes.emplace_back(brush);
            }
        }

        state::current_brush = state::brushes.at(0);
    }

}
