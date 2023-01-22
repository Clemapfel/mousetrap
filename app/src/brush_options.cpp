//
// Created by clem on 1/22/23.
//

#include <app/brush_options.hpp>
#include <app/bubble_log_area.hpp>

namespace mousetrap
{
    void BrushOptions::BrushPreview::set_as_algorithmic(const std::string& name, const std::string& shader_id)
    {
        _size = -1;
        _name = name;
        _label.set_text(BrushOptions::make_index_label("&#8734;"));
        _shader_path = get_resource_path() + "shaders/" + shader_id + ".frag";

        delete _texture;
        _texture = nullptr;

        std::stringstream tooltip_text;
        tooltip_text << "<b>Brush \"" + _name + "\"</b>" << std::endl;
        tooltip_text << "Base Size: Any";
        operator Widget*()->set_tooltip_text(tooltip_text.str());

        if (_area.get_is_realized())
            on_realize(&_area, this);
    }

    void BrushOptions::BrushPreview::set_as_custom(const std::string& name, const Image& image)
    {
        _size = std::max(image.get_size().x, image.get_size().y);
        _name = name;
        _label.set_text(BrushOptions::make_index_label(std::to_string(_size)));
        _shader_path = "";

        if (_texture == nullptr)
            _texture = new Texture();

        _texture->create_from_image(image);

        std::stringstream tooltip_text;
        tooltip_text << "<b>Brush \"" + _name + "\"</b>" << std::endl;
        tooltip_text << "Base Size: " << _size;
        operator Widget*()->set_tooltip_text(tooltip_text.str());

        if (_area.get_is_realized())
            on_realize(&_area, this);
    }

    BrushOptions::BrushPreview::BrushPreview()
    {
        _area.connect_signal_realize(on_realize, this);
        _frame.set_child(&_area);
        _aspect_frame.set_child(&_frame);

        _label.set_align(GTK_ALIGN_END);

        _overlay.set_child(&_aspect_frame);
        _overlay.add_overlay(&_label);
    }

    BrushOptions::BrushPreview::~BrushPreview()
    {
        delete _shader;
        delete _shape;
        delete _texture;
    }

    BrushOptions::BrushPreview::operator Widget*()
    {
        return &_overlay;
    }

    void BrushOptions::BrushPreview::set_preview_size(size_t x)
    {
        _area.set_size_request({x, x});
    }

    void BrushOptions::BrushPreview::on_realize(Widget* widget, BrushPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (instance->_shape == nullptr)
            instance->_shape = new Shape();

        instance->_shape->as_rectangle(Vector2f(BrushOptions::brush_margin), Vector2f(1 - 2 * BrushOptions::brush_margin));

        if (instance->_background == nullptr)
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
            if (instance->_shader == nullptr)
                instance->_shader = new Shader();

            instance->_shader->create_from_file(instance->_shader_path, ShaderType::FRAGMENT);
            auto task = RenderTask(instance->_shape, instance->_shader, nullptr, BlendMode::NORMAL);
            task.register_float("_half_margin", &BrushOptions::brush_margin);
            area->add_render_task(task);
        }

        area->queue_render();
    }

    BrushOptions::AddBrushDialog::AddBrushDialog()
        : _dialog("Add Brush...")
    {
        _dialog.set_on_accept_pressed([](FileChooserDialog<FileChooserDialogMode::OPEN>* instance)
        {
            auto path = instance->get_current_name();
            auto brush = Brush();
            auto image = Image();

            if (image.create_from_file(path))
            {
                auto max_brush_size = state::settings_file->get_value_as<size_t>("brush_options", "maximum_brush_size");
                if (image.get_size().x > max_brush_size or image.get_size().y > max_brush_size)
                    state::bubble_log->send_message("Unable to load brush from file at `" + path + "`: Image width or height exceeds 256px", InfoMessageType::ERROR);
                else
                {
                    brush.as_custom(image);
                    state::bubble_log->send_message("Added new Custom Brush `" + brush.get_name() + "`");
                }
            }
            else
                state::bubble_log->send_message("Unable to load brush from file at `" + path + "`: File is not an image or has unsupported format", InfoMessageType::ERROR);

            instance->close();
        });

        _dialog.set_on_cancel_pressed([](FileChooserDialog<FileChooserDialogMode::OPEN>* instance)
        {
            instance->close();
        });

        auto filter = FileFilter("image files");
        filter.add_allow_all_supported_image_formats();
        _dialog.get_file_chooser().add_filter(filter);
    }

    void BrushOptions::AddBrushDialog::show()
    {
        _dialog.show();
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
            active_state->set_brush_size(scale->get_value());
        }, this);

        _brush_size_spin_button.connect_signal_value_changed([](SpinButton* scale, BrushOptions* instance) -> void {
            active_state->set_brush_size(scale->get_value());
        }, this);

        _brush_opacity_scale.connect_signal_value_changed([](Scale* scale, BrushOptions* instance) -> void {
            active_state->set_brush_opacity(scale->get_value());
        }, this);

        _brush_opacity_spin_button.connect_signal_value_changed([](SpinButton* scale, BrushOptions* instance) -> void {
            active_state->set_brush_opacity(scale->get_value());
        }, this);

        set_brush_opacity(active_state->get_brush_opacity());
        set_brush_size(active_state->get_brush_size());

        // actions

        using namespace state::actions;

        brush_options_increase_brush_size.set_function([](){
            active_state->set_brush_size(active_state->get_brush_size() + 1);
        });

        brush_options_decrease_brush_size.set_function([](){
            active_state->set_brush_size(active_state->get_brush_size() - 1);
        });

        brush_options_increase_brush_opacity.set_function([](){
            active_state->set_brush_opacity(active_state->get_brush_opacity() + 0.1);
        });

        brush_options_decrease_brush_opacity.set_function([](){
            active_state->set_brush_opacity(active_state->get_brush_opacity() - 0.1);
        });

        brush_options_add_brush.set_function([](){
           state::brush_options->_add_brush_dialog.show();
        });

        brush_options_remove_brush.set_function([](){
           active_state->remove_brush(active_state->get_current_brush_index());
        });

        brush_options_load_default_brushes.set_function([]() {
           active_state->load_default_brushes();
        });

        for (auto* action : {
            &brush_options_increase_brush_size,
            &brush_options_decrease_brush_size,
            &brush_options_increase_brush_opacity,
            &brush_options_decrease_brush_opacity,
            &brush_options_add_brush,
            &brush_options_remove_brush,
            &brush_options_load_default_brushes,
            &brush_options_open_default_brush_directory
        })
            state::add_shortcut_action(*action);

        // menu

        _preview_size_spin_button.set_margin_start(state::margin_unit);
        _preview_size_spin_button.set_value(_preview_size);
        _preview_size_spin_button.connect_signal_value_changed([](SpinButton* scale, BrushOptions* instance){
            instance->set_preview_size(scale->get_value());
        }, this);
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

        auto brush_section = MenuModel();
        brush_section.add_action("Add Brush...", brush_options_add_brush.get_id());
        brush_section.add_action("Remove Current Brush", brush_options_remove_brush.get_id());
        brush_section.add_action("Load Default Brushes", brush_options_load_default_brushes.get_id());
        _menu.add_section("Manage Brushes", &brush_section);

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

        // layout

        _brush_preview_list.get_selection_model()->connect_signal_selection_changed([](SelectionModel*, size_t i, size_t, BrushOptions* instance) {
            active_state->set_current_brush(i);
        }, this);
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

        on_brush_set_updated();
        on_brush_selection_changed();
    }

    BrushOptions::operator Widget*()
    {
        return &_main;
    }

    void BrushOptions::set_brush_size(size_t x)
    {
        _brush_size_scale.set_signal_value_changed_blocked(true);
        _brush_size_scale.set_value(x);
        _brush_size_scale.set_signal_value_changed_blocked(false);

        _brush_size_spin_button.set_signal_value_changed_blocked(true);
        _brush_size_spin_button.set_value(x);
        _brush_size_spin_button.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::set_brush_opacity(float x)
    {
        _brush_opacity_scale.set_signal_value_changed_blocked(true);
        _brush_opacity_scale.set_value(x);
        _brush_opacity_scale.set_signal_value_changed_blocked(false);

        _brush_opacity_spin_button.set_signal_value_changed_blocked(true);
        _brush_opacity_spin_button.set_value(x);
        _brush_opacity_spin_button.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::set_preview_size(size_t v)
    {
        _preview_size = v;
        for (auto& preview : _brush_previews)
            preview.set_preview_size(v);
    }

    void BrushOptions::on_brush_selection_changed()
    {
        set_brush_size(active_state->get_brush_size());
        set_brush_opacity(active_state->get_brush_opacity());

        _brush_preview_list.get_selection_model()->set_signal_selection_changed_blocked(true);
        _brush_preview_list.get_selection_model()->select(active_state->get_current_brush_index());
        _brush_preview_list.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    void BrushOptions::on_brush_set_updated()
    {
        auto& brushes = active_state->get_brushes();

        while (_brush_previews.size() < brushes.size()) // overfill will not be pushed to grid view
            _brush_previews.emplace_back();

        _brush_preview_list.clear();

        for (size_t i = 0; i < brushes.size(); ++i)
        {
            const auto& brush = brushes.at(i);
            auto& preview = _brush_previews.at(i);

            if (brush.get_shape() == BrushShape::CUSTOM)
                preview.set_as_custom(brush.get_name(), brush.get_image());
            else
            {
                std::stringstream shader_name;
                shader_name << "brush_";
                for (auto c:brush_shape_to_string(brush.get_shape()))
                    shader_name << (char) std::tolower(c);

                preview.set_as_algorithmic(brush.get_name(), shader_name.str());
            }

            _brush_preview_list.push_back(preview);
            preview.set_preview_size(_preview_size);
        }
    }
}