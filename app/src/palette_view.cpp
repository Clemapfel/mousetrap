//
// Created by clem on 1/21/23.
//

#include <app/palette_view.hpp>

namespace mousetrap
{
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

    PaletteView::ColorTile::~ColorTile()
    {
        delete _color_shape;
        delete _frame_shape;
        delete _is_selected_shape;
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

    PaletteView::PaletteControlBar::PaletteControlBar(PaletteView* owner)
            : _owner(owner)
    {
        set_palette_editing_enabled(active_state->get_palette_editing_enabled());
        _palette_locked_button.connect_signal_toggled(on_palette_locked_button_toggled, this);

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

        using namespace state::actions;

        auto load_save_submenu = MenuModel();
        load_save_submenu.add_action("Load...", palette_view_load.get_id());
        load_save_submenu.add_action("Load Default", palette_view_load_default.get_id());
        load_save_submenu.add_action("Save", palette_view_save.get_id());
        load_save_submenu.add_action("Save As...", palette_view_save_as.get_id());
        load_save_submenu.add_action("Save As Default", palette_view_save_as_default.get_id());
        _menu.add_section("Load / Save", &load_save_submenu);

        auto sort_by_section = MenuModel();
        auto sort_by_submenu = MenuModel();
        sort_by_submenu.add_action("None", palette_view_sort_by_default.get_id());
        sort_by_submenu.add_action("Hue", palette_view_sort_by_hue.get_id());
        sort_by_submenu.add_action("Saturation", palette_view_sort_by_saturation.get_id());
        sort_by_submenu.add_action("Value", palette_view_sort_by_value.get_id());
        sort_by_section.add_submenu("Sort By...", &sort_by_submenu);
        _menu.add_section("Sorting", &sort_by_section);

        auto* popover_menu = new PopoverMenu(&_menu);
        _menu_button.set_popover(popover_menu);
        _menu_button.set_child(&_menu_button_label);

        _menu_button_label.set_hexpand(true);
        _menu_button_label.set_halign(GTK_ALIGN_START);
        _menu_button.set_hexpand(true);
        _palette_locked_button.set_hexpand(false);

        _main.push_back(&_menu_button);
        _main.push_back(&_palette_locked_button);
    }

    void PaletteView::PaletteControlBar::set_palette_editing_enabled(bool is_editing_enabled)
    {
        auto is_locked = not is_editing_enabled;

        if (is_locked)
        {
            _palette_locked_button.set_child(&_palette_locked_icon);
            _palette_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_not_active"));
        }
        else
        {
            _palette_locked_button.set_child(&_palette_not_locked_icon);
            _palette_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_active"));
        }

        _palette_locked_button.set_signal_toggled_blocked(true);
        _palette_locked_button.set_active(is_locked);
        _palette_locked_button.set_signal_toggled_blocked(false);

        _palette_locked_switch.set_signal_state_set_blocked(true);
        _palette_locked_switch.set_active(is_locked);
        _palette_locked_switch.set_state(is_locked ? Switch::State::ON : Switch::State::OFF);
        _palette_locked_switch.set_signal_state_set_blocked(false);
    }

    void PaletteView::PaletteControlBar::on_palette_locked_button_toggled(ToggleButton* button, PaletteControlBar* instance)
    {
        active_state->set_palette_editing_enabled(not button->get_active());
    }

    void PaletteView::PaletteControlBar::on_palette_locked_switch_state_set(Switch* button, bool value, PaletteControlBar* instance)
    {
        active_state->set_palette_editing_enabled(not button->get_active());
    }

    void PaletteView::PaletteControlBar::on_preview_size_scale_value_changed(SpinButton* scale, PaletteControlBar* instance)
    {
        instance->_owner->set_preview_size(scale->get_value());
    }

    PaletteView::PaletteControlBar::operator Widget*()
    {
        return &_main;
    }

    void PaletteView::on_palette_updated()
    {
        auto sort_mode = active_state->get_palette_sort_mode();

        std::vector<HSVA> colors = {};

        if (sort_mode == PaletteSortMode::NONE)
            colors = active_state->get_palette().get_colors();
        else if (sort_mode == PaletteSortMode::BY_HUE)
            colors = active_state->get_palette().get_colors_by_hue();
        else if (sort_mode == PaletteSortMode::BY_SATURATION)
            colors = active_state->get_palette().get_colors_by_saturation();
        else if (sort_mode == PaletteSortMode::BY_VALUE)
            colors = active_state->get_palette().get_colors_by_value();

        while (_color_tiles.size() < colors.size())
        {
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 0)));
        }

        for (size_t i = 0; i < _color_tiles.size(); ++i)
        {
            auto* tile = _color_tiles.at(i);

            if (i >= colors.size())
            {
                tile->operator Widget*()->set_visible(false);
                continue;
            }

            tile->operator Widget*()->set_visible(true);
            tile->set_color(colors.at(i));
            tile->set_selected(colors.at(i) == active_state->get_primary_color());
            _color_tile_view.push_back(tile->operator Widget*());
        }
    }

    void PaletteView::on_color_selection_changed()
    {
        if (active_state->get_palette_editing_enabled())
        {
            _color_tile_view.get_selection_model()->select(_selected_i);

            for (auto* tile : _color_tiles)
                tile->set_selected(false);

            _color_tiles.at(_selected_i)->set_color(active_state->get_primary_color());
            _color_tiles.at(_selected_i)->set_selected(true);
            return;
        }

        bool matched_once = false;
        for (size_t i = 0; i < _color_tiles.size(); ++i)
        {
            auto* tile = _color_tiles.at(i);
            if (not tile->operator Widget*()->get_visible())
                break;

            auto a = rgba_to_html_code(tile->get_color());
            auto b = rgba_to_html_code( active_state->get_primary_color());
            auto matched = a == b;

            tile->set_selected(matched);

            if (matched)
            {
                _selected_i = i;
                matched_once = true;
            }
        }

        if (matched_once)
            _color_tile_view.get_selection_model()->select(_selected_i);
        else
            _color_tile_view.get_selection_model()->unselect_all();
    }

    void PaletteView::on_palette_sort_mode_changed()
    {
        on_palette_updated();
    }

    void PaletteView::on_palette_editing_toggled()
    {
        _palette_control_bar.set_palette_editing_enabled(active_state->get_palette_editing_enabled());
    }

    PaletteView::PaletteView()
    {
        for (size_t i = 0; i < 256; ++i) // pre allocate to reduce load time
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 0)));

        on_palette_updated();

        _scrolled_window.set_child(&_color_tile_view);
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _palette_view_box.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _palette_view_box.set_hexpand(true);

        _color_tile_view.get_selection_model()->connect_signal_selection_changed(on_color_tile_view_selection_model_selection_changed, this);
        _color_tile_view.get_selection_model()->unselect_all();

        _palette_control_bar.operator Widget *()->set_vexpand(false);
        _palette_view_box.set_vexpand(true);
        _palette_view_box.set_size_request({0, _color_tiles.at(0)->operator Widget *()->get_preferred_size().natural_size.y + 4});

        _main.push_back(_palette_control_bar);
        _main.push_back(&_palette_view_box);
        _main.set_vexpand(true);

        on_color_selection_changed();
    }

    PaletteView::~PaletteView()
    {
        for (auto* tile : _color_tiles)
            delete tile;
    }

    PaletteView::operator Widget*()
    {
        return &_main;
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

    void PaletteView::on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance)
    {
        instance->_selected_i = child_i;
        auto color = instance->_color_tiles.at(child_i)->get_color();
        active_state->set_primary_color(color);
        active_state->set_preview_colors(color, color);
    }
}