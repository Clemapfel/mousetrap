#include <app/palette_view.hpp>
#include <app/color_picker.hpp>
#include <app/verbose_color_picker.hpp>
#include <app/color_preview.hpp>
#include <app/color_swapper.hpp>

namespace mousetrap
{
    // COLOR TILE

    PaletteView::ColorTile::ColorTile(PaletteView* owner, HSVA color)
            : _owner(owner), _color(color)
    {
        _color_area.connect_signal_realize(on_color_area_realize, this);
        _selection_frame_area.connect_signal_realize(on_selection_frame_realize, this);

        _color_area.set_size_request(Vector2f(owner->_preview_size));
        _selection_frame_area.set_size_request(Vector2f(owner->_preview_size));

        _overlay.set_child(&_color_area);
        _overlay.add_overlay(&_selection_frame_area);
        _aspect_frame.set_child(&_overlay);

        operator Widget*()->set_align(GTK_ALIGN_CENTER);
        operator Widget*()->set_cursor(GtkCursorType::POINTER);

        set_color(color);
        set_size(_owner->_preview_size);
    }

    void PaletteView::ColorTile::set_color(HSVA color)
    {
        _color = color;
        if (_color_shape != nullptr)
            _color_shape->set_color(color);

        auto round = [](float v) -> std::string {
            auto as_int = int(v * 100);
            if (v == 1)
                return "1.000";
            else
                return "0." + std::to_string(as_int);
        };

        std::stringstream tooltip;
        tooltip << "HSVA (" << round(color.h) << ", " << round(color.v) << ", " << round(color.a) << ")";
        _aspect_frame.set_tooltip_text(tooltip.str());
        _color_area.queue_render();
    }

    void PaletteView::ColorTile::set_size(size_t size)
    {
        _color_area.set_size_request({size, size});
        _selection_frame_area.set_size_request({size, size});
        _aspect_frame.set_size_request({size, size});
    }

    HSVA PaletteView::ColorTile::get_color()
    {
        return _color;
    }

    void PaletteView::ColorTile::set_selected(bool b)
    {
        if (_is_selected_shape != nullptr)
            _is_selected_shape->set_visible(b);

        _selection_frame_area.queue_render();
    }

    PaletteView::ColorTile::operator Widget*()
    {
        return &_aspect_frame;
    }

    void PaletteView::ColorTile::on_color_area_realize(Widget* widget, ColorTile* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_color_shape = new Shape();
        instance->_color_shape->as_rectangle({0, 0}, {1, 1});
        instance->_color_shape->set_color(instance->_color);

        instance->_frame_shape = new Shape();
        float eps = 0.001;
        instance->_frame_shape->as_wireframe({
                                                     {eps, eps},
                                                     {eps + (1 - eps), eps},
                                                     {(1 - eps), (1 - eps)},
                                                     {eps, (1 - eps)}
                                             });
        instance->_frame_shape->set_color(RGBA(0, 0, 0, 1));

        area->add_render_task(instance->_color_shape);
        area->add_render_task(instance->_frame_shape);

        area->queue_render();
    }

    void PaletteView::ColorTile::on_selection_frame_realize(Widget* widget, ColorTile* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (is_selected_overlay_texture == nullptr)
        {
            is_selected_overlay_texture = new Texture();
            is_selected_overlay_texture->create_from_file(get_resource_path() + "icons/" + "selected_indicator" + ".png");
        }

        instance->_is_selected_shape = new Shape();
        instance->_is_selected_shape->as_rectangle({0, 0}, {1, 1});
        instance->_is_selected_shape->set_texture(is_selected_overlay_texture);
        instance->_is_selected_shape->set_visible(false);

        area->add_render_task(instance->_is_selected_shape);
        area->queue_render();
    }

    // CONTROL BAR

    void PaletteView::PaletteControlBar::update_palette_locked()
    {
        if (_owner->_palette_locked)
        {
            _palette_not_locked_button.set_child(&_palette_locked_icon);
            _palette_not_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_not_active"));
        }
        else
        {
            _palette_not_locked_button.set_child(&_palette_not_locked_icon);
            _palette_not_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_active"));
        }

        _palette_not_locked_button.set_signal_toggled_blocked(true);
        _palette_not_locked_button.set_active(not _owner->_palette_locked);
        _palette_not_locked_button.set_signal_toggled_blocked(false);

        _palette_locked_switch.set_signal_state_set_blocked(true);
        _palette_locked_switch.set_active(_owner->_palette_locked);
        _palette_locked_switch.set_state(_owner->_palette_locked ? Switch::State::ON : Switch::State::OFF);
        _palette_locked_switch.set_signal_state_set_blocked(false);
    }

    void PaletteView::PaletteControlBar::update_preview_size_changed()
    {
        _preview_size_scale.set_signal_value_changed_blocked(true);
        _preview_size_scale.set_value(_owner->_preview_size);
        _preview_size_scale.set_signal_value_changed_blocked(false);
    }

    void PaletteView::PaletteControlBar::on_palette_not_locked_button_toggled(ToggleButton* button, PaletteControlBar* instance)
    {
        instance->_owner->set_palette_locked(not button->get_active());
    }

    void PaletteView::PaletteControlBar::on_palette_locked_switch_state_set(Switch* button, bool value, PaletteControlBar* instance)
    {
        instance->_owner->set_palette_locked(button->get_active());
    }

    void PaletteView::PaletteControlBar::on_preview_size_scale_value_changed(SpinButton* scale, PaletteControlBar* instance)
    {
        instance->_owner->set_preview_size( scale->get_value());
    }

    PaletteView::PaletteControlBar::operator Widget*()
    {
        return &_main;
    }

    PaletteView::PaletteControlBar::PaletteControlBar(PaletteView* owner)
            : _owner(owner)
    {
        _palette_not_locked_button.set_active(not owner->_palette_locked);
        _palette_not_locked_button.set_child(owner->_palette_locked ? &_palette_locked_icon : &_palette_not_locked_icon);
        _palette_not_locked_button.connect_signal_toggled(on_palette_not_locked_button_toggled, this);

        _palette_locked_icon.set_size_request(_palette_locked_icon.get_size());
        _palette_not_locked_icon.set_size_request(_palette_not_locked_icon.get_size());

        _preview_size_label.set_hexpand(false);
        _preview_size_label.set_halign(GTK_ALIGN_START);
        _preview_size_scale.set_hexpand(true);
        _preview_size_scale.connect_signal_value_changed(on_preview_size_scale_value_changed, this);

        _preview_size_box.push_back(&_preview_size_label);
        _preview_size_box.push_back(&_preview_size_spacer);
        _preview_size_box.push_back(&_preview_size_scale);

        _palette_locked_label.set_hexpand(false);
        _palette_locked_label.set_halign(GTK_ALIGN_START);
        _palette_locked_spacer.set_hexpand(true);
        _palette_locked_switch.set_size_request({state::margin_unit, state::margin_unit});
        _palette_locked_switch.set_halign(GTK_ALIGN_END);
        _palette_locked_switch.connect_signal_state_set(on_palette_locked_switch_state_set, this);

        _palette_locked_box.push_back(&_palette_locked_label);
        _palette_locked_box.push_back(&_palette_locked_spacer);
        _palette_locked_box.push_back(&_palette_locked_switch);

        _palette_locked_box.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "palette_editing_menu_item"));
        _preview_size_box.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "preview_size_menu_item"));

        update_palette_locked();
        update_preview_size_changed();

        for (auto* box : {&_palette_locked_box, &_preview_size_box})
            box->set_margin(state::margin_unit);

        for (auto* spacer : {&_palette_locked_spacer, &_preview_size_spacer})
        {
            spacer->set_opacity(0);
            spacer->set_size_request({state::margin_unit, 0});
        }

        auto settings_section = MenuModel();

        auto preview_size_submenu = MenuModel();
        preview_size_submenu.add_widget(&_preview_size_box);

        auto palette_locked_submenu = MenuModel();
        palette_locked_submenu.add_widget(&_palette_locked_box);

        settings_section.add_submenu("Preview Size...", &preview_size_submenu);
        settings_section.add_submenu("Editing...", &palette_locked_submenu);
        _menu.add_section("Settings", &settings_section);

        auto load_save_submenu = MenuModel();
        load_save_submenu.add_action("Load...", "palette_view.load");
        load_save_submenu.add_action("Load Default", "palette_view.load_default");
        load_save_submenu.add_action("Save", "palette_view.save");
        load_save_submenu.add_action("Save As...", "palette_view.save_as");
        load_save_submenu.add_action("Save As Default", "palette_view.save_as_default");
        _menu.add_section("Load / Save", &load_save_submenu);

        auto sort_by_section = MenuModel();
        auto sort_by_submenu = MenuModel();
        sort_by_submenu.add_action("None", "palette_view.sort_by_default");
        sort_by_submenu.add_action("Hue", "palette_view.sort_by_hue");
        sort_by_submenu.add_action("Saturation", "palette_view.sort_by_saturation");
        sort_by_submenu.add_action("Value", "palette_view.sort_by_value");
        sort_by_section.add_submenu("Sort By...", &sort_by_submenu);
        _menu.add_section("Sorting", &sort_by_section);

        auto* popover_menu = new PopoverMenu(&_menu);
        _menu_button.set_popover(popover_menu);
        _menu_button.set_child(&_menu_button_label);

        _menu_button_label.set_hexpand(true);
        _menu_button_label.set_halign(GTK_ALIGN_START);
        _menu_button.set_hexpand(true);
        _palette_not_locked_button.set_hexpand(false);

        //_menu_button.set_cursor(GtkCursorType::POINTER);
        //_palette_editing_active_toggle_button.set_cursor(GtkCursorType::POINTER);

        _main.push_back(&_menu_button);
        _main.push_back(&_palette_not_locked_button);
    }

    void PaletteView::PaletteControlBar::update()
    {
        update_palette_locked();
        update_preview_size_changed();
    }

    // PALETTE VIEW

    PaletteView::operator Widget*()
    {
        return &_main;
    }

    void PaletteView::on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance)
    {
        instance->_color_tiles.at(instance->_selected_i)->set_selected(false);
        instance->_color_tiles.at(child_i)->set_selected(true);
        instance->_selected_i = child_i;

        active_state->set_primary_color(instance->_color_tiles.at(child_i)->get_color());
        active_state->set_preview_colors(active_state->get_primary_color(), active_state->get_primary_color());
    }

    void PaletteView::load_as_debug()
    {
        std::vector<HSVA> colors;

        const size_t n_steps = 8;
        for (size_t i = 0; i < n_steps; ++i)
        {
            float h = i / float(n_steps);

            for (float s : {0.33f, 0.66f, 1.f})
                for (float v : {0.33f, 0.66f, 1.f})
                    colors.push_back(HSVA(h, s, v, 1));
        }

        for (size_t i = 0; i < n_steps; ++i)
            colors.push_back(HSVA(0, 0, i / float(n_steps), 1));

        active_state->set_palette(colors);
        update_from_palette();
    }

    void PaletteView::load_from_file(const std::string& path)
    {
        auto palette = Palette();
        if (not palette.load_from(path))
        {
            state::bubble_log->send_message("Unable to load palette at \"" + path + "\"");
            return;
        }

        active_state->set_palette(palette.get_colors());
        update_from_palette();
    }

    void PaletteView::save_to_file(const std::string& path)
    {
        const auto& palette = active_state->get_palette();
        if (palette.save_to(path))
            state::bubble_log->send_message("Saved current palette as \"" + path + "\"");
        else
            state::bubble_log->send_message("Unable to save palette as \"" + path + "\"", InfoMessageType::ERROR);
    }

    void PaletteView::on_load_ok_pressed()
    {
        auto path = _on_load_dialog.get_current_name();
        load_from_file(path);
        _on_load_dialog.close();
    }

    void PaletteView::on_save_as_ok_pressed()
    {
        auto path = _on_save_as_dialog.get_current_name();
        save_to_file(path);
        _on_save_as_dialog.close();
    }

    void PaletteView::update_from_palette()
    {
        _color_tile_view.clear();

        for (auto* tile : _color_tiles)
            tile->set_color(HSVA(0, 0, 0, 1));

        auto& palette = active_state->get_palette();
        auto sort_mode = active_state->get_palette_sort_mode();

        for (size_t i = 0; palette.get_n_colors() > _color_tiles.size() and i < palette.get_n_colors() - _color_tiles.size(); ++i)
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 1)));

        std::vector<HSVA> colors;
        if (sort_mode == PaletteSortMode::NONE)
            colors = palette.get_colors();
        else if (sort_mode == PaletteSortMode::BY_HUE)
            colors = palette.get_colors_by_hue();
        else if (sort_mode == PaletteSortMode::BY_VALUE)
            colors = palette.get_colors_by_value();
        else if (sort_mode == PaletteSortMode::BY_SATURATION)
            colors = palette.get_colors_by_saturation();

        for (size_t i = 0; i < colors.size(); ++i)
        {
            auto color = colors.at(i);
            color.a = 1;
            _color_tiles.at(i)->set_color(color);
            _color_tile_view.push_back(_color_tiles.at(i)->operator Widget*());
        }

        if (colors.size() == 0)
        {
            _color_tiles.at(0)->set_color(HSVA(0, 0, 0, 1));
            _color_tile_view.push_back(_color_tiles.at(0)->operator Widget*());
        }
    }

    PaletteView::PaletteView()
    {
        for (size_t i = 0; i < 256; ++i) // pre allocate to reduce load time
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 0)));

        for (auto* tile : _color_tiles)
            _color_tile_view.push_back(tile->operator Widget*());

        auto path = state::settings_file->get_value_as<std::string>("palette_view", "default_palette");
        if (path == "DEBUG")
            load_as_debug();
        else
            load_from_file(path);

        update_from_palette();

        _scrolled_window.set_child(&_color_tile_view);
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _palette_view_box.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _palette_view_box.set_hexpand(true);

        _color_tile_view.get_selection_model()->connect_signal_selection_changed(on_color_tile_view_selection_model_selection_changed, this);
        _color_tile_view.get_selection_model()->unselect_all();

        _color_tiles.at(_selected_i)->set_selected(true);

        _palette_control_bar.operator Widget *()->set_vexpand(false);
        _palette_view_box.set_vexpand(true);
        _palette_view_box.set_size_request({0, _color_tiles.at(0)->operator Widget *()->get_preferred_size().natural_size.y + 4});

        _main.push_back(_palette_control_bar);
        _main.push_back(&_palette_view_box);
        _main.set_vexpand(true);

        _palette_control_bar.update();

        FileFilter palette_filer = FileFilter("Mousetrap Palette File");
        palette_filer.add_allowed_suffix("palette");

        using namespace state::actions;

        // ACTION: load

        _on_load_dialog.get_file_chooser().add_filter(palette_filer, true);

        _on_load_dialog.set_on_accept_pressed([](OpenFileDialog*, PaletteView* instance){
            instance->on_load_ok_pressed();
            instance->_on_load_dialog.close();
        }, this);

        _on_load_dialog.set_on_cancel_pressed([](OpenFileDialog*, PaletteView* instance){
            instance->_on_load_dialog.close();
        }, this);

        palette_view_load.set_function([instance = this](){
            instance->_on_load_dialog.show();
        });

        // ACTION: save as
        _on_save_as_dialog.get_file_chooser().add_filter(palette_filer, true);

        _on_save_as_dialog.set_on_accept_pressed([](SaveAsFileDialog*, PaletteView* instance){
            instance->on_save_as_ok_pressed();
            instance->_on_save_as_dialog.close();
        }, this);

        _on_save_as_dialog.set_on_cancel_pressed([](SaveAsFileDialog*, PaletteView* instance){
            instance->_on_save_as_dialog.close();
        }, this);

        palette_view_save_as.set_function([instance = this](){
            instance->_on_save_as_dialog.get_name_entry().set_text("Untitled.palette");
            instance->_on_save_as_dialog.show();
        });

        // Action:: Save

        palette_view_save.set_function([instance = this](){
            if (instance->_save_to_path == "")
            {
                instance->_on_save_as_dialog.get_name_entry().set_text("Untitled.palette");
                instance->_on_save_as_dialog.show();
            }
            else
                instance->save_to_file(instance->_save_to_path);
        });

        // Action: Load Default
        palette_view_load_default.set_function([instance = this]()
         {
             instance->load_from_file(get_resource_path() + "default.palette");
         });

        // Action: Save As Default

        palette_view_save_as_default.set_function([instance = this]()
        {
            if (active_state->get_palette().save_to(get_resource_path() + "default.palette"))
                state::bubble_log->send_message("Current palette saved as default");
            else
                state::bubble_log->send_message("Unable to save current palette as default");
        });

        // Action:: Sort

        palette_view_sort_by_default.set_function([instance = this](){
            active_state->set_palette_sort_mode(PaletteSortMode::NONE);
            instance->update_from_palette();
        });

        palette_view_sort_by_hue.set_function([instance = this](){
            active_state->set_palette_sort_mode(PaletteSortMode::BY_HUE);
            instance->update_from_palette();
        });

        palette_view_sort_by_value.set_function([instance = this](){
            active_state->set_palette_sort_mode(PaletteSortMode::BY_VALUE);
            instance->update_from_palette();
        });

        palette_view_sort_by_saturation.set_function([instance = this](){
            active_state->set_palette_sort_mode(PaletteSortMode::BY_SATURATION);
            instance->update_from_palette();
        });

        // on startup: make first color current color
        operator Widget*()->add_tick_callback([](FrameClock, PaletteView* instance) -> bool
          {
              instance->select(0);
              return false;
          }, this);

        std::vector<Action*> select_color_actions = {
                &palette_view_select_color_0,
                &palette_view_select_color_1,
                &palette_view_select_color_2,
                &palette_view_select_color_3,
                &palette_view_select_color_4,
                &palette_view_select_color_5,
                &palette_view_select_color_6,
                &palette_view_select_color_7,
                &palette_view_select_color_8,
                &palette_view_select_color_9
        };

        for (size_t i = 0; i < select_color_actions.size(); ++i)
        {
            auto* action = select_color_actions.at(i);
            action->set_function([index = i, instance = this](){
                instance->select(index);
            });
        }

        for (auto* action : {
            &palette_view_load_default,
            &palette_view_save,
            &palette_view_save_as_default,
            &palette_view_load,
            &palette_view_save_as,
            &palette_view_sort_by_default,
            &palette_view_sort_by_hue,
            &palette_view_sort_by_saturation,
            &palette_view_sort_by_value,
            &palette_view_select_color_0,
            &palette_view_select_color_1,
            &palette_view_select_color_2,
            &palette_view_select_color_3,
            &palette_view_select_color_4,
            &palette_view_select_color_5,
            &palette_view_select_color_6,
            &palette_view_select_color_7,
            &palette_view_select_color_8,
            &palette_view_select_color_9
        })
            state::add_shortcut_action(*action);

        _tooltip.create_from("palette_view", state::tooltips_file, state::keybindings_file);
        operator Widget*()->set_tooltip_widget(_tooltip.operator Widget*());
    }

    void PaletteView::update()
    {
        auto color = active_state->get_primary_color();

        _palette_control_bar.update();

        if (_palette_locked)
        {
            _color_tile_view.get_selection_model()->unselect_all();

            for (size_t i = 0; i < _color_tiles.size(); ++i)
            {
                auto* tile = _color_tiles.at(i);
                bool should_select = tile->get_color() == color;
                tile->set_selected(should_select);

                if (should_select)
                    select(i);
            }

            return;
        }

        color.a = 1;
        _color_tiles.at(_selected_i)->set_color(color);
    }

    void PaletteView::reload()
    {
        update_from_palette();
    }

    void PaletteView::select(size_t i)
    {
        if (i > _color_tiles.size())
            i = _color_tiles.size() - 1;

        _color_tile_view.get_selection_model()->select(i);
        on_color_tile_view_selection_model_selection_changed(_color_tile_view.get_selection_model(), i, 1, this);
    }

    void PaletteView::set_preview_size(size_t x)
    {
        _preview_size = x;
        for (auto* tile : _color_tiles)
            tile->set_size(x);
    }

    size_t PaletteView::get_preview_size() const
    {
        return _preview_size;
    }

    void PaletteView::set_palette_locked(bool b)
    {
        _palette_locked = b;
        _palette_control_bar.update();
    }

    bool PaletteView::get_palette_locked() const
    {
        return _palette_locked;
    }
}