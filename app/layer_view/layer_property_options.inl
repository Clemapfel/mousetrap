// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void LayerView::LayerPropertyOptions::update()
    {
        auto& layer = *state::layers.at(_layer);

        _visible_toggle_button.set_all_signals_blocked(true);
        _visible_toggle_button.set_active(not layer.is_visible);
        _visible_toggle_button.set_child(
                layer.is_visible ? &_visible_toggle_button_icon_visible : &_visible_toggle_button_icon_not_visible);
        _visible_toggle_button.set_all_signals_blocked(false);

        _visible_check_button.set_all_signals_blocked(true);
        _visible_check_button.set_is_checked(layer.is_visible);
        _visible_check_button.set_all_signals_blocked(false);

        _menu_button.set_opacity(layer.is_visible ? 1 : 0.3);

        _locked_toggle_button.set_all_signals_blocked(true);
        _locked_toggle_button.set_active(layer.is_locked);
        _locked_toggle_button.set_child(
                layer.is_locked ? &_locked_toggle_button_icon_locked : &_locked_toggle_button_icon_not_locked);
        _locked_toggle_button.set_all_signals_blocked(false);

        _locked_check_button.set_all_signals_blocked(true);
        _locked_check_button.set_is_checked(layer.is_locked);
        _locked_check_button.set_all_signals_blocked(false);

        _opacity_scale.set_all_signals_blocked(true);
        _opacity_scale.set_value(layer.opacity);
        _opacity_scale.set_all_signals_blocked(false);

        _blend_mode_dropdown.set_list_item_activation_blocked(true);
        size_t i = 0;
        for (; i < _blend_modes_in_order.size(); ++i)
            if (_blend_modes_in_order.at(i) == layer.blend_mode)
                break;

        _blend_mode_dropdown.set_selected(i);
        _blend_mode_dropdown.set_list_item_activation_blocked(false);

        _name_entry.set_all_signals_blocked(true);
        _name_entry.set_text(layer.name);
        _menu_button_title_label.set_text(layer.name);
        _name_entry.set_all_signals_blocked(false);
    }

    LayerView::LayerPropertyOptions::LayerPropertyOptions(size_t layer_i, LayerView* owner)
            : _layer(layer_i), _owner(owner)
    {
        auto icon_size = _locked_toggle_button_icon_locked.get_size();

        for (auto* icon: {&_locked_toggle_button_icon_locked, &_locked_toggle_button_icon_not_locked,
                          &_visible_toggle_button_icon_visible, &_visible_toggle_button_icon_not_visible})
        {
            icon->set_size_request(icon_size);
            icon->set_expand(false);
        }

        for (auto* button: {&_visible_toggle_button, &_locked_toggle_button})
        {
            button->set_has_frame(false);
            button->set_size_request(icon_size);
            button->set_expand(false);
            _visible_locked_indicator_box.push_back(button);
        }

        _locked_toggle_button.connect_signal_toggled(on_locked_toggle_button_toggled, this);
        _visible_toggle_button.connect_signal_toggled(on_visible_toggle_button_toggled, this);

        _name_box.push_back(&_name_label);
        _name_box.push_back(&_name_separator);
        _name_box.push_back(&_name_entry);
        _name_entry.set_text(state::layers.at(_layer)->name);
        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_hexpand(true);
        _name_entry.connect_signal_activate(on_name_entry_activate, this);

        auto* entry_natural = gtk_requisition_new();
        auto* button_natural = gtk_requisition_new();

        gtk_widget_get_preferred_size(_name_entry.operator GtkWidget*(), nullptr, entry_natural);
        gtk_widget_get_preferred_size(_visible_check_button.operator GtkWidget*(), nullptr, button_natural);

        float right_margin = entry_natural->width * 0.5 - button_natural->width * 0.5;

        gtk_requisition_free(entry_natural);
        gtk_requisition_free(button_natural);

        _visible_box.push_back(&_visible_label);
        _visible_box.push_back(&_visible_separator);
        _visible_box.push_back(&_visible_check_button);
        _visible_check_button.set_halign(GTK_ALIGN_END);
        _visible_check_button.set_hexpand(true);
        _visible_check_button.set_margin_end(right_margin);
        _visible_check_button.connect_signal_toggled(on_visible_check_button_toggled, this);

        _locked_box.push_back(&_locked_label);
        _locked_box.push_back(&_locked_separator);
        _locked_box.push_back(&_locked_check_button);
        _locked_check_button.set_halign(GTK_ALIGN_END);
        _locked_check_button.set_hexpand(true);
        _locked_check_button.set_margin_end(right_margin);
        _locked_check_button.connect_signal_toggled(on_locked_check_button_toggled, this);

        _opacity_box.push_back(&_opacity_label);
        _opacity_box.push_back(&_opacity_separator);
        _opacity_box.push_back(&_opacity_scale);
        _opacity_scale.set_hexpand(true);
        _opacity_scale.set_value(state::layers.at(_layer)->opacity);
        _opacity_scale.connect_signal_value_changed(on_opacity_scale_value_changed, this);

        static auto generate_tooltip_text = [](std::string title, std::string description)
        {
            std::stringstream str;

            str << "<b>" << title << "</b>";

            if (not description.empty())
                str << "\n\n" << "<span foreground=\"#BBBBBB\">" << description << "</span>";

            return str.str();
        };

        _blend_mode_dropdown.set_list_item_activation_blocked(true);
        auto add_dropdown_item = [&](BlendMode mode) -> void {

            std::string label;
            std::string list_item;
            std::string label_tooltip;
            std::string list_tooltip;

            if (mode == BlendMode::NORMAL)
            {
                list_item = "Normal";
                label = "Normal";
                label_tooltip = "Blend Mode: Alpha";
                list_tooltip = generate_tooltip_text("Alpha Blending",
                                                     "<tt>color = mix(source.rgb, destination.rgb, source.a)</tt>");
            }
            else if (mode == BlendMode::ADD)
            {
                list_item = "Add";
                label = "Add";
                label_tooltip = "Blend Mode: Additive";
                list_tooltip = generate_tooltip_text("Additive Blending",
                                                     "<tt>color = source.rgba + destination.rgba</tt>");
            }
            else if (mode == BlendMode::SUBTRACT)
            {
                list_item = "Subtract";
                label = "Subtract";
                label_tooltip = "Blend Mode: Subtractive";
                list_tooltip = generate_tooltip_text("Subtractive Blending",
                                                     "<tt>color = source.rgba - destination.rgba</tt>");
            }
            else if (mode == BlendMode::REVERSE_SUBTRACT)
            {
                list_item = "Reverse Subtract";
                label = "Reverse Subtract";
                label_tooltip = "Blend Mode: Subtractive Reversed";
                list_tooltip = generate_tooltip_text("Reverse Subtractive Blending",
                                                     "<tt>color = destination.rgba - source.rgba</tt>");
            }
            else if (mode == BlendMode::MULTIPLY)
            {
                list_item = "Multiply";
                label = "Multiply";
                label_tooltip = "Blend Mode: Multiplicative";
                list_tooltip = generate_tooltip_text("Multiplicative Blending",
                                                     "<tt>color = destination.rgba * source.rgba</tt>");
            }
            else if (mode == BlendMode::MIN)
            {
                list_item = "Minimum";
                label = "Min";
                label_tooltip = "Blend Mode: Minimum";
                list_tooltip = generate_tooltip_text("Minimum Blending",
                                                     "<tt>color = min(destination.rgba, source.rgba)</tt>");
            }
            else if (mode == BlendMode::MAX)
            {
                list_item = "Maximum";
                label = "Max";
                label_tooltip = "Blend Mode: Maximum";
                list_tooltip = generate_tooltip_text("Maximum Blending",
                                                     "<tt>color = max(destination.rgba, source.rgba)</tt>");
            }

            _blend_mode_dropdown_list_items.emplace_back(list_item);
            _blend_mode_dropdown_list_items.back().set_tooltip_text(list_tooltip);
            _blend_mode_dropdown_list_items.back().set_halign(GTK_ALIGN_START);

            _blend_mode_dropdown_label_items.emplace_back(label);
            _blend_mode_dropdown_label_items.back().set_tooltip_text(label_tooltip);

            _blend_mode_dropdown.push_back(
                    &_blend_mode_dropdown_list_items.back(),
                    &_blend_mode_dropdown_label_items.back(),
                    on_blend_mode_select,
                    new on_blend_mode_select_data{mode, this}
            );
        };

        _blend_mode_dropdown_label_items.reserve(_blend_modes_in_order.size());
        _blend_mode_dropdown_list_items.reserve(_blend_modes_in_order.size());

        for (auto mode: _blend_modes_in_order)
            add_dropdown_item(mode);

        _blend_mode_box.push_back(&_blend_mode_label);
        _blend_mode_box.push_back(&_blend_mode_separator);
        _blend_mode_box.push_back(&_blend_mode_dropdown);
        _blend_mode_dropdown.set_hexpand(true);
        _blend_mode_dropdown.set_halign(GTK_ALIGN_CENTER);

        _blend_mode_dropdown.set_list_item_activation_blocked(false);

        for (auto* box: {&_name_box, &_opacity_box, &_visible_box, &_locked_box, &_blend_mode_box})
            _menu_button_popover_box.push_back(box);

        _menu_button_popover_box.set_homogeneous(true);
        _menu_button_popover.set_child(&_menu_button_popover_box);
        _menu_button.set_popover(&_menu_button_popover);
        _menu_button.set_has_frame(false);

        _menu_button_title_label.set_text(state::layers.at(_layer)->name);
        _menu_button_title_label.set_halign(GTK_ALIGN_START);
        _menu_button_title_label.set_hexpand(false);
        _menu_button_title_label_viewport.set_child(&_menu_button_title_label);
        _menu_button_title_label_viewport.set_size_request({10 * state::margin_unit, 0});
        _menu_button_title_label_viewport.set_has_frame(false);
        _menu_button_title_label_viewport.set_propagate_natural_height(true);
        _menu_button_title_label_viewport.set_policy(GTK_POLICY_NEVER, GTK_POLICY_NEVER);
        _menu_button_title_label_viewport.set_can_respond_to_input(false);
        _menu_button.set_child(&_menu_button_title_label_viewport);
        _menu_button.set_hexpand(false);

        _main.push_back(&_visible_locked_indicator_box);
        _main.push_back(&_menu_button);

        for (auto* widget: std::vector < Widget * > {
                &_locked_toggle_button,
                &_visible_toggle_button,
                &_menu_button,
                &_visible_check_button,
                &_locked_check_button,
                &_opacity_scale,
                &_blend_mode_box
        })
            widget->set_cursor(GtkCursorType::POINTER);

        for (auto& label: _blend_mode_dropdown_label_items)
            label.set_cursor(GtkCursorType::POINTER);

        for (auto& sep: {&_name_separator, &_visible_separator, &_locked_separator, &_opacity_separator,
                         &_blend_mode_separator})
            sep->set_opacity(0);

        update();
    }

    LayerView::LayerPropertyOptions::operator Widget*()
    {
        return &_main;
    }

    void LayerView::LayerPropertyOptions::on_locked_toggle_button_toggled(ToggleButton* button,
                                                                          LayerPropertyOptions* instance)
    {}

    void LayerView::LayerPropertyOptions::on_visible_toggle_button_toggled(ToggleButton* button,
                                                                           LayerPropertyOptions* instance)
    {}

    void LayerView::LayerPropertyOptions::on_name_entry_activate(Entry* entry, LayerPropertyOptions* instance)
    {}

    void
    LayerView::LayerPropertyOptions::on_locked_check_button_toggled(CheckButton* button, LayerPropertyOptions* instance)
    {}

    void LayerView::LayerPropertyOptions::on_visible_check_button_toggled(CheckButton* button,
                                                                          LayerPropertyOptions* instance)
    {}

    void LayerView::LayerPropertyOptions::on_opacity_scale_value_changed(Scale* scale, LayerPropertyOptions* instance)
    {}

    void LayerView::LayerPropertyOptions::on_blend_mode_select(on_blend_mode_select_data* data)
    {}
}