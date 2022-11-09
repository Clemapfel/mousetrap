// 
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/app_component.hpp>
#include <app/palette.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    enum class PaletteSortMode
    {
        NONE,
        BY_HUE,
        BY_VALUE,
        BY_SATURATION
    };

    class PaletteView : public AppComponent
    {
        public:
            PaletteView();
            operator Widget*() override;
            void update() override;

        protected:
            void load_from_file(const std::string& path);
            void load_as_debug();

            void save_to_file(const std::string& path);

        private:
            bool palette_locked = state::settings_file->get_value_as<bool>("palette_view", "palette_locked");
            float tile_size = state::settings_file->get_value_as<float>("palette_view", "tile_size");

            Palette palette;
            PaletteSortMode sort_mode = PaletteSortMode::NONE;
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

                    Box _tile_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _tile_size_label = Label("Tile Size: ");
                    SeparatorLine _tile_size_spacer;
                    SpinButton _tile_size_scale = SpinButton(2, 512, 1);
                    static void on_tile_size_scale_value_changed(SpinButton*, PaletteControlBar* instance);

                    void update_palette_locked();
                    void update_tile_size_changed();

                    Label _menu_button_label = Label("Palette");
                    MenuButton _menu_button;
                    MenuModel _menu;
            };

            PaletteControlBar _palette_control_bar = PaletteControlBar(this);

            size_t _selected_i = 0;
            static void on_flowbox_child_selected(FlowBox*, size_t child_i, PaletteView* instance);

            std::vector<ColorTile*> _color_tiles;
            FlowBox _flow_box = FlowBox(GTK_ORIENTATION_HORIZONTAL);
            ScrolledWindow _scrolled_window;

            Box _palette_view_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            // actions

            Action _on_load_action = Action("palette_view.load");
            Window _on_load_file_chooser_dialog_window;
            Dialog _on_load_file_chooser_dialog = Dialog(&_on_load_file_chooser_dialog_window, "Load Palette...");        
            Box _on_load_file_chooser_dialog_box = Box(GTK_ORIENTATION_VERTICAL);
            FileChooser _on_load_file_chooser = FileChooser(FileChooserAction::OPEN);
            void on_load_ok_pressed();

            Action _on_save_as_action = Action("palette_view.save_as");
            Window _on_save_as_file_chooser_dialog_window;
            Dialog _on_save_as_file_chooser_dialog = Dialog(&_on_load_file_chooser_dialog_window, "Load Palette...");
            Box _on_save_as_file_chooser_dialog_box = Box(GTK_ORIENTATION_VERTICAL);
            FileChooser _on_save_as_file_chooser = FileChooser(FileChooserAction::SAVE);
            void on_save_as_ok_pressed();

            Action _on_load_default_action = Action("palette_view.load_default");
            Action _on_save_action = Action("palette_view.save");
            Action _on_save_as_default_action = Action("palette_view.save_as_default");

            Action _on_sort_by_default_action = Action("palette_view.sort_by_default");
            Action _on_sort_by_hue_action = Action("palette_view.sort_by_hue");
            Action _on_sort_by_saturation_action = Action("palette_view.sort_by_saturation");
            Action _on_sort_by_value_action = Action("palette_view.sort_by_value");
    };
}

//

namespace mousetrap
{
    // COLOR TILE

    PaletteView::ColorTile::ColorTile(PaletteView* owner, HSVA color)
            : _owner(owner), _color(color)
    {
        _color_area.connect_signal_realize(on_color_area_realize, this);
        _selection_frame_area.connect_signal_realize(on_selection_frame_realize, this);

        _color_area.set_size_request(Vector2f(owner->tile_size));
        _selection_frame_area.set_size_request(Vector2f(owner->tile_size));

        _overlay.set_child(&_color_area);
        _overlay.add_overlay(&_selection_frame_area);
        _aspect_frame.set_child(&_overlay);

        operator Widget*()->set_align(GTK_ALIGN_CENTER);
        operator Widget*()->set_cursor(GtkCursorType::POINTER);

        set_color(color);
        set_size(_owner->tile_size);
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
        if (_owner->palette_locked)
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
        _palette_not_locked_button.set_active(not _owner->palette_locked);
        _palette_not_locked_button.set_signal_toggled_blocked(false);

        _palette_locked_switch.set_signal_state_set_blocked(true);
        _palette_locked_switch.set_active(_owner->palette_locked);
        _palette_locked_switch.set_state(_owner->palette_locked ? Switch::State::ON : Switch::State::OFF);
        _palette_locked_switch.set_signal_state_set_blocked(false);
    }

    void PaletteView::PaletteControlBar::update_tile_size_changed()
    {
        _tile_size_scale.set_signal_value_changed_blocked(true);
        _tile_size_scale.set_value(_owner->tile_size);
        _tile_size_scale.set_signal_value_changed_blocked(false);
    }

    void PaletteView::PaletteControlBar::on_palette_not_locked_button_toggled(ToggleButton* button, PaletteControlBar* instance)
    {
        instance->_owner->palette_locked = not button->get_active();
        instance->update_palette_locked();
    }

    void PaletteView::PaletteControlBar::on_palette_locked_switch_state_set(Switch* button, bool value, PaletteControlBar* instance)
    {
        instance->_owner->palette_locked = button->get_active();
        instance->update_palette_locked();
    }

    void PaletteView::PaletteControlBar::on_tile_size_scale_value_changed(SpinButton* scale, PaletteControlBar* instance)
    {
        int value = scale->get_value();
        for (auto* tile : instance->_owner->_color_tiles)
            tile->set_size(value);
    }

    PaletteView::PaletteControlBar::operator Widget*()
    {
        return &_main;
    }

    PaletteView::PaletteControlBar::PaletteControlBar(PaletteView* owner)
            : _owner(owner)
    {
        _palette_not_locked_button.set_active(not owner->palette_locked);
        _palette_not_locked_button.set_child(owner->palette_locked ? &_palette_locked_icon : &_palette_not_locked_icon);
        _palette_not_locked_button.connect_signal_toggled(on_palette_not_locked_button_toggled, this);

        _palette_locked_icon.set_size_request(_palette_locked_icon.get_size());
        _palette_not_locked_icon.set_size_request(_palette_not_locked_icon.get_size());

        _tile_size_label.set_hexpand(false);
        _tile_size_label.set_halign(GTK_ALIGN_START);
        _tile_size_scale.set_hexpand(true);
        _tile_size_scale.connect_signal_value_changed(on_tile_size_scale_value_changed, this);

        _tile_size_box.push_back(&_tile_size_label);
        _tile_size_box.push_back(&_tile_size_spacer);
        _tile_size_box.push_back(&_tile_size_scale);

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
        _tile_size_box.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "tile_size_menu_item"));
        _menu_button.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "main"));

        update_palette_locked();
        update_tile_size_changed();

        for (auto* box : {&_palette_locked_box, &_tile_size_box})
            box->set_margin(state::margin_unit);

        for (auto* spacer : {&_palette_locked_spacer, &_tile_size_spacer})
        {
            spacer->set_opacity(0);
            spacer->set_size_request({state::margin_unit, 0});
        }

        auto* settings_section = new MenuModel();
        
        auto* tile_size_submenu = new MenuModel();
        tile_size_submenu->add_widget(&_tile_size_box);
        
        auto* palette_locked_submenu = new MenuModel();
        palette_locked_submenu->add_widget(&_palette_locked_box);
        
        settings_section->add_submenu("Tile Size...", tile_size_submenu);
        settings_section->add_submenu("Editing...", palette_locked_submenu);
        _menu.add_section("Setting", settings_section);
       
        auto* load_save_submenu = new MenuModel();
        load_save_submenu->add_action("Load...", "palette_view.load");
        load_save_submenu->add_action("Load Default", "palette_view.load_default");
        load_save_submenu->add_action("Save", "palette_view.save");
        load_save_submenu->add_action("Save As...", "palette_view.save_as");
        load_save_submenu->add_action("Save As Default", "palette_view.save_as_default");
        _menu.add_section("Load / Save", load_save_submenu);
        
        auto* sort_by_section = new MenuModel();
        auto* sort_by_submenu = new MenuModel();
        sort_by_submenu->add_action("None", "palette_view.sort_by_default");
        sort_by_submenu->add_action("Hue", "palette_view.sort_by_hue");
        sort_by_submenu->add_action("Saturation", "palette_view.sort_by_saturation");
        sort_by_submenu->add_action("Value", "palette_view.sort_by_value");
        sort_by_section->add_submenu("Sort By...", sort_by_submenu);
        _menu.add_section("Sorting", sort_by_section);

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
        update_tile_size_changed();
    }

    // PALETTE VIEW

    PaletteView::operator Widget*()
    {
        return &_main;
    }

    void PaletteView::on_flowbox_child_selected(FlowBox* flowbox, size_t child_i, PaletteView* instance)
    {
        instance->_color_tiles.at(instance->_selected_i)->set_selected(false);
        instance->_color_tiles.at(child_i)->set_selected(true);
        instance->_selected_i = child_i;

        state::primary_color = instance->_color_tiles.at(child_i)->get_color();
        state::preview_color_current = state::primary_color;
        state::preview_color_previous = state::primary_color;

        state::update_color_picker();
        state::update_verbose_color_picker();
        state::update_color_swapper();
        state::update_color_preview();
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

        palette = Palette(colors);
    }

    void PaletteView::load_from_file(const std::string& path)
    {
        palette.load_from(path);
        update_from_palette();
    }

    void PaletteView::save_to_file(const std::string& path)
    {}

    void PaletteView::on_load_ok_pressed()
    {
        auto selected = _on_load_file_chooser.get_selected();
        if (selected.empty())
            return;

        load_from_file(selected.at(0).get_path());
        _on_load_file_chooser_dialog.close();
    }

    void PaletteView::on_save_as_ok_pressed()
    {
        auto selected = _on_save_as_file_chooser.get_selected();
        if (selected.empty())
            return;

        save_to_file(selected.at(0).get_path());
        _on_save_as_file_chooser_dialog.close();
    }

    void PaletteView::update_from_palette()
    {
        auto clock = Clock();
        clock.restart();

        _flow_box.set_visible(false);

        for (auto* tile : _color_tiles)
        {
            _flow_box.remove(*tile);
            delete tile;
        }

        _color_tiles.clear();

        if (sort_mode == PaletteSortMode::NONE)
        {
            for (auto& color : palette.get_colors())
                _color_tiles.emplace_back(new ColorTile(this, color));
        }
        else if (sort_mode == PaletteSortMode::BY_HUE)
        {
            for (auto& color : palette.get_colors_by_hue())
                _color_tiles.emplace_back(new ColorTile(this, color));
        }
        else if (sort_mode == PaletteSortMode::BY_VALUE)
        {
            for (auto& color : palette.get_colors_by_value())
                _color_tiles.emplace_back(new ColorTile(this, color));
        }
        else if (sort_mode == PaletteSortMode::BY_SATURATION)
        {
            for (auto& color : palette.get_colors_by_saturation())
                _color_tiles.emplace_back(new ColorTile(this, color));
        }

        if (_color_tiles.empty())
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 1)));

        for (auto* tile : _color_tiles)
            _flow_box.push_back(tile->operator Widget*());

        _flow_box.set_visible(true);

        std::cout << clock.elapsed().as_seconds() << std::endl;
    }

    PaletteView::PaletteView()
    {
        for (auto* tile : _color_tiles)
            _flow_box.push_back(tile->operator Widget*());

        auto path = state::settings_file->get_value_as<std::string>("palette_view", "default_palette");

        if (path == "DEBUG")
            load_as_debug();
        else
            load_from_file(path);

        update_from_palette();

        _scrolled_window.set_child(&_flow_box);
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _palette_view_box.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _palette_view_box.set_hexpand(true);

        _flow_box.set_selection_mode(GTK_SELECTION_SINGLE);
        _flow_box.connect_signal_child_activated(on_flowbox_child_selected, this);

        _flow_box.unselect_all();
        _color_tiles.at(_selected_i)->set_selected(true);

        _palette_control_bar.operator Widget *()->set_vexpand(false);
        _palette_view_box.set_vexpand(true);
        _palette_view_box.set_size_request({0, _color_tiles.at(0)->operator Widget *()->get_preferred_size().natural_size.y + 4});

        _main.push_back(_palette_control_bar);
        _main.push_back(&_palette_view_box);
        _main.set_vexpand(true);

        _palette_control_bar.update();

        // ACTION: load

        auto on_load_filter = FileFilter("Mousetrap Palette File");
        on_load_filter.add_allowed_suffix(".palette");
        _on_load_file_chooser.add_filter(on_load_filter);
        _on_load_file_chooser.set_can_select_multiple(false);
        _on_load_file_chooser.set_expand(true);

        _on_load_file_chooser_dialog_box.push_back(&_on_load_file_chooser);
        _on_load_file_chooser_dialog.get_content_area().push_back(&_on_load_file_chooser_dialog_box);

        _on_load_file_chooser_dialog.add_action_button("OK", [](PaletteView* instance) {
            instance->on_load_ok_pressed();
        }, this);

        _on_load_file_chooser_dialog.add_action_button("Cancel", [](PaletteView* instance){
            instance->_on_load_file_chooser_dialog.close();
        }, this);
        
        _on_load_action.set_do_function([](PaletteView* instance){
            instance->_on_load_file_chooser_dialog.show();
        }, this);

        state::app->add_action(_on_load_action);

        // ACTION: save as

        _on_save_as_file_chooser.add_filter(on_load_filter);
        _on_save_as_file_chooser.set_can_select_multiple(false);
        _on_save_as_file_chooser.set_expand(true);

        _on_save_as_file_chooser_dialog_box.push_back(&_on_save_as_file_chooser);
        _on_save_as_file_chooser_dialog.get_content_area().push_back(&_on_save_as_file_chooser_dialog_box);

        _on_save_as_file_chooser_dialog.add_action_button("OK", [](PaletteView* instance) {
            instance->on_save_as_ok_pressed();
        }, this);

        _on_save_as_file_chooser_dialog.add_action_button("Cancel", [](PaletteView* instance){
            instance->_on_save_as_file_chooser_dialog.close();
        }, this);

        _on_save_as_action.set_do_function([](PaletteView* instance){
            instance->_on_save_as_file_chooser_dialog.show();
        }, this);

        state::app->add_action(_on_save_as_action);

        // Action:: Save

        _on_save_action.set_do_function([](PaletteView* instance){
            std::cout << "[TODO] Saving palette..." << std::endl;
        }, this);

        state::app->add_action(_on_save_action);

        // Action: Load Default

        _on_load_default_action.set_do_function([](PaletteView* instance) {
            std::cout << "[TODO] Loading default palette..." << std::endl;
        }, this);

        _on_load_default_action.set_do_function([](PaletteView* instance) {
            std::cout << "[TODO] Undoing loading default palette..." << std::endl;
        }, this);

        state::app->add_action(_on_load_default_action);

        // Action: Save As Default

        _on_save_as_default_action.set_do_function([](PaletteView* isntance){
            std::cout << "[TODO] Saving as default palette..." << std::endl;
        }, this);

        _on_save_as_default_action.set_undo_function([](PaletteView* isntance){
            std::cout << "[TODO] Undoing saving as default palette..." << std::endl;
        }, this);

        state::app->add_action(_on_save_as_default_action);

        // Action:: Sort

        _on_sort_by_default_action.set_do_function([](PaletteView* instance){
            instance->sort_mode = PaletteSortMode::NONE;
            instance->update_from_palette();
        }, this);

        _on_sort_by_hue_action.set_do_function([](PaletteView* instance){
            instance->sort_mode = PaletteSortMode::BY_HUE;
            instance->update_from_palette();
        }, this);

        _on_sort_by_value_action.set_do_function([](PaletteView* instance){
            instance->sort_mode = PaletteSortMode::BY_VALUE;
            instance->update_from_palette();
        }, this);

        _on_sort_by_saturation_action.set_do_function([](PaletteView* instance){
            instance->sort_mode = PaletteSortMode::BY_SATURATION;
            instance->update_from_palette();
        }, this);

        for (auto* action : {&_on_sort_by_default_action, &_on_sort_by_hue_action, &_on_sort_by_value_action, &_on_sort_by_saturation_action})
            state::app->add_action(*action);
    }

    void PaletteView::update()
    {
        auto color = state::primary_color;

        _palette_control_bar.update();

        if (palette_locked)
        {
            _flow_box.unselect_all();
            bool once = true;

            for (size_t i = 0; i < _color_tiles.size(); ++i)
            {
                auto* tile = _color_tiles.at(i);
                bool should_select = tile->get_color() == color;
                tile->set_selected(should_select);

                if (should_select)
                    _flow_box.select(i);
            }

            return;
        }

        color.a = 1;
        _color_tiles.at(_selected_i)->set_color(color);
    }
}