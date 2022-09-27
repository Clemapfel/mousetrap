// 
// Copyright 2022 Clemens Cords
// Created on 9/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/image_display.hpp>
#include <include/toggle_button.hpp>
#include <include/entry.hpp>
#include <include/scale.hpp>
#include <include/check_button.hpp>
#include <include/get_resource_path.hpp>
#include <include/box.hpp>
#include <include/list_view.hpp>
#include <include/reorderable_list.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class LayerView : public AppComponent
    {
        public:
            LayerView();

            operator Widget*() override;
            void update() override {};

        private:
            struct LayerRow
            {
                LayerRow();

                ImageDisplay _layer_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_locked.png", state::icon_scale);
                ImageDisplay _layer_not_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png", state::icon_scale);
                ToggleButton _layer_locked_button;
                static void on_layer_locked_button_toggled(ToggleButton*, LayerRow*);

                ImageDisplay _layer_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_visible.png", state::icon_scale);
                ImageDisplay _layer_not_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png", state::icon_scale);
                ToggleButton _layer_visible_button;
                static void on_layer_visible_button_toggled(ToggleButton*, LayerRow*);

                Box _visible_locked_indicator_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit * 0.5);

                Label _menu_button_title_label = Label("");
                MenuButton _menu_button;
                Popover _menu_button_popover;

                Label _name_label = Label("Name: ");
                SeparatorLine _name_separator;
                Entry _name_entry;
                Box _name_box = Box(GTK_ORIENTATION_HORIZONTAL);

                Label _visible_label = Label("Visible:");
                SeparatorLine _visible_separator;
                CheckButton _visible_check_button;
                Box _visible_box = Box(GTK_ORIENTATION_HORIZONTAL);

                Label _locked_label = Label("Locked: ");
                SeparatorLine _locked_separator;
                CheckButton _locked_check_button;
                Box _locked_box = Box(GTK_ORIENTATION_HORIZONTAL);

                Label _opacity_label = Label("Opacity: ");
                SeparatorLine _opacity_separator;
                Scale _opacity_scale = Scale(0, 1, 0.01);
                Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);

                Label _blend_mode_label = Label("Blend: ");
                SeparatorLine _blend_mode_separator;
                Label _blend_mode_dropdown = Label("TODO");
                Box _blend_mode_box = Box(GTK_ORIENTATION_HORIZONTAL);

                Box _menu_button_popover_box = Box (GTK_ORIENTATION_VERTICAL);

                ListView _main = ListView(GTK_ORIENTATION_HORIZONTAL);

            };

            std::vector<LayerRow*> _rows;
            ReorderableListView _main = ReorderableListView(GTK_ORIENTATION_VERTICAL);
    };
}

namespace mousetrap
{
    void LayerView::LayerRow::on_layer_locked_button_toggled(ToggleButton* button, LayerRow* instance)
    {
        if (button->get_active())
            button->set_child(&instance->_layer_locked_icon);
        else
            button->set_child(&instance->_layer_not_locked_icon);
    }

    void LayerView::LayerRow::on_layer_visible_button_toggled(ToggleButton* button, LayerRow* instance)
    {
        if (button->get_active())
            button->set_child(&instance->_layer_visible_icon);
        else
            button->set_child(&instance->_layer_not_visible_icon);
    }

    LayerView::LayerRow::LayerRow()
    {
        auto icon_size = _layer_locked_icon.get_size();

        for (auto* icon : {&_layer_locked_icon, &_layer_not_locked_icon, &_layer_visible_icon, &_layer_not_visible_icon})
        {
            icon->set_size_request(icon_size);
            icon->set_expand(false);
        }

        for (auto* button : { &_layer_visible_button, &_layer_locked_button})
        {
            button->set_has_frame(false);
            button->set_size_request(icon_size);
            button->set_expand(false);
            _visible_locked_indicator_box.push_back(button);
        }

        _layer_locked_button.connect_signal_toggled(on_layer_locked_button_toggled, this);
        _layer_visible_button.connect_signal_toggled(on_layer_visible_button_toggled, this);

        _layer_locked_button.set_active(true);
        _layer_locked_button.set_active(false);
        _layer_visible_button.set_active(true);

        _name_box.push_back(&_name_label);
        _name_box.push_back(&_name_separator);
        _name_box.push_back(&_name_entry);
        _name_entry.set_n_chars(std::string("Layer_#999").size());
        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_hexpand(false);

        auto* entry_natural = gtk_requisition_new();
        auto* button_natural = gtk_requisition_new();

        gtk_widget_get_preferred_size(_name_entry.operator GtkWidget *(), nullptr, entry_natural);
        gtk_widget_get_preferred_size(_visible_check_button.operator GtkWidget *(), nullptr, button_natural);

        float right_margin = entry_natural->width * 0.5 - button_natural->width * 0.5;

        gtk_requisition_free(entry_natural);
        gtk_requisition_free(button_natural);

        _visible_box.push_back(&_visible_label);
        _visible_box.push_back(&_visible_separator);
        _visible_box.push_back(&_visible_check_button);
        _visible_check_button.set_halign(GTK_ALIGN_END);
        _visible_check_button.set_hexpand(true);
        _visible_check_button.set_margin_end(right_margin);

        _locked_box.push_back(&_locked_label);
        _locked_box.push_back(&_locked_separator);
        _locked_box.push_back(&_locked_check_button);
        _locked_check_button.set_halign(GTK_ALIGN_END);
        _locked_check_button.set_hexpand(true);
        _locked_check_button.set_margin_end(right_margin);

        _opacity_box.push_back(&_opacity_label);
        _opacity_box.push_back(&_opacity_separator);
        _opacity_box.push_back(&_opacity_scale);
        _opacity_scale.set_hexpand(true);

        _blend_mode_box.push_back(&_blend_mode_label);
        _blend_mode_box.push_back(&_blend_mode_separator);
        _blend_mode_box.push_back(&_blend_mode_dropdown);
        _blend_mode_dropdown.set_hexpand(true);
        _blend_mode_dropdown.set_halign(GTK_ALIGN_CENTER);

        for (auto* box : {&_name_box, &_visible_box, &_locked_box, &_opacity_box, &_blend_mode_box})
           _menu_button_popover_box.push_back(box);

        _menu_button_popover_box.set_homogeneous(true);
        _menu_button_popover.set_child(&_menu_button_popover_box);
        _menu_button.set_popover(&_menu_button_popover);

        _main.push_back(&_visible_locked_indicator_box);
        _main.push_back(&_menu_button);
    }

    LayerView::LayerView()
    {
        for (size_t i = 0; i < 1; ++i)
        {
            _rows.emplace_back(new LayerRow());
            _main.push_back(&_rows.back()->_main);
        }
    }

    LayerView::operator Widget*()
    {
        return &_main;
    }
}