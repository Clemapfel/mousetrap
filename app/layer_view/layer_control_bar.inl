// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

#include <include/dialog.hpp>

namespace mousetrap
{
    void LayerView::LayerControlBar::on_layer_move_up_button_clicked(Button*, LayerControlBar* instance)
    {
        if (instance->_owner->_selected_layer == 0)
            return;

        size_t current_pos = instance->_owner->_selected_layer;
        size_t new_pos = instance->_owner->_selected_layer - 1;

        auto* current = state::layers.at(current_pos);
        auto* above = state::layers.at(new_pos);

        state::layers.at(current_pos) = above;
        state::layers.at(new_pos) = current;

        instance->_owner->_layer_rows.at(current_pos).update();
        instance->_owner->_layer_rows.at(new_pos).update();

        instance->_owner->set_selection(new_pos, instance->_owner->_selected_frame);
    }

    void LayerView::LayerControlBar::on_layer_move_down_button_clicked(Button*, LayerControlBar* instance)
    {
        if (instance->_owner->_selected_layer == state::n_frames - 1)
            return;

        size_t current_pos = instance->_owner->_selected_layer;
        size_t new_pos = instance->_owner->_selected_layer + 1;

        auto* current = state::layers.at(current_pos);
        auto* above = state::layers.at(new_pos);

        state::layers.at(current_pos) = above;
        state::layers.at(new_pos) = current;

        instance->_owner->_layer_rows.at(current_pos).update();
        instance->_owner->_layer_rows.at(new_pos).update();

        instance->_owner->set_selection(new_pos, instance->_owner->_selected_frame);
    }

    void LayerView::LayerControlBar::on_layer_create_button_clicked(Button*, LayerControlBar* instance)
    {
        auto* dialog = new Dialog(state::main_window, "Choose Layer Name");
        auto* name_entry = new Entry();
        name_entry->set_text("New Layer #" + std::to_string(instance->_owner->_selected_layer));

        static auto on_ok = [dialog, name_entry, instance](void*)
        {
            Layer* layer = *state::layers.insert(state::layers.begin() + instance->_owner->_selected_layer, new Layer{name_entry->get_text()});

            for (size_t i = 0; i < state::n_frames; ++i)
            {
                layer->frames.emplace_back();
                layer->frames.back().image = new Image();
                layer->frames.back().texture = new Texture();
                layer->frames.back().image->create(state::layer_resolution.x, state::layer_resolution.y, RGBA(1, 1, 1, 0));
                layer->frames.back().texture->create_from_image(*layer->frames.back().image);
            }

            instance->_owner->_layer_rows.emplace_back(instance->_owner->_selected_layer + 1, instance->_owner);
            instance->_owner->_layer_row_list_view.push_back(instance->_owner->_layer_rows.back());

            for (auto& row : instance->_owner->_layer_rows)
                row.update();

            dialog->close();

            delete dialog;
            delete name_entry;
        };

        auto on_cancel = [dialog, name_entry](void*)
        {
            dialog->close();

            delete dialog;
            delete name_entry;
        };

        dialog->add_action_button("ok", on_ok, nullptr);
        dialog->add_action_button("cancel", on_cancel, nullptr);

        auto& content = dialog->get_content_area();
        content.push_back(name_entry);
        dialog->show();
    }

    void LayerView::LayerControlBar::on_layer_delete_button_clicked(Button*, LayerControlBar* instance)
    {
        size_t selected_i = instance->_owner->_selected_layer;

        if (selected_i == state::layers.size() - 1)
            instance->_owner->set_selection(selected_i - 1, instance->_owner->_selected_frame);

        auto* layer = state::layers.at(selected_i);
        state::layers.erase(state::layers.begin() + selected_i);

        instance->_owner->_layer_row_list_view.clear();
        instance->_owner->_layer_rows.erase(instance->_owner->_layer_rows.end() - 1);

        for (size_t i = 0; i < instance->_owner->_layer_rows.size(); ++i)
        {
            auto& row = instance->_owner->_layer_rows.at(i);
            row.update();
        }

        for (auto& row : instance->_owner->_layer_rows)
            instance->_owner->_layer_row_list_view.push_back(row);

        for (auto& frame : layer->frames)
        {
            delete frame.image;
            delete frame.texture;
        }
    }

    void LayerView::LayerControlBar::on_layer_duplicate_button_clicked(Button*, LayerControlBar* instance)
    {
        auto* current = state::layers.at(instance->_owner->_selected_layer);
        Layer* layer = *state::layers.insert(state::layers.begin() + instance->_owner->_selected_layer, new Layer{current->name + " (copy)"});

        for (size_t i = 0; i < state::n_frames; ++i)
        {
            layer->frames.emplace_back();
            layer->frames.back().image = new Image(*current->frames.at(i).image);
            layer->frames.back().texture = new Texture();
            layer->frames.back().texture->create_from_image(*layer->frames.back().image);
        }

        instance->_owner->_layer_rows.emplace_back(state::layers.size() - 1, instance->_owner);
        instance->_owner->_layer_row_list_view.push_back(instance->_owner->_layer_rows.back());

        for (auto& row : instance->_owner->_layer_rows)
            row.update();
    }

    void LayerView::LayerControlBar::on_layer_merge_down_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_flatten_all_button_clicked(Button*, LayerControlBar* instance)
    {}

    LayerView::LayerControlBar::operator Widget*()
    {
        return &_main;
    }

    LayerView::LayerControlBar::LayerControlBar(LayerView* owner)
            : _owner(owner)
    {
        for (auto* display : {&_layer_move_up_icon, &_layer_create_icon, &_layer_duplicate_icon, &_layer_delete_icon, &_layer_move_down_icon, &_layer_merge_down_icon, &_layer_flatten_all_icon})
            display->set_size_request(display->get_size());

        _layer_move_up_button.set_child(&_layer_move_up_icon);
        _layer_move_up_button.connect_signal_clicked(on_layer_move_up_button_clicked, this);

        _layer_create_button.set_child(&_layer_create_icon);
        _layer_create_button.connect_signal_clicked(on_layer_create_button_clicked, this);

        _layer_duplicate_button.set_child(&_layer_duplicate_icon);
        _layer_duplicate_button.connect_signal_clicked(on_layer_duplicate_button_clicked, this);

        _layer_delete_button.set_child(&_layer_delete_icon);
        _layer_delete_button.connect_signal_clicked(on_layer_delete_button_clicked, this);

        _layer_move_down_button.set_child(&_layer_move_down_icon);
        _layer_move_down_button.connect_signal_clicked(on_layer_move_down_button_clicked, this);

        _layer_merge_down_button.set_child(&_layer_merge_down_icon);
        _layer_merge_down_button.connect_signal_clicked(on_layer_merge_down_button_clicked, this);

        _layer_flatten_all_button.set_child(&_layer_flatten_all_icon);
        _layer_flatten_all_button.connect_signal_clicked(on_layer_flatten_all_button_clicked, this);

        for (auto* button : {&_layer_move_up_button, &_layer_create_button, &_layer_duplicate_button, &_layer_delete_button, &_layer_move_down_button, &_layer_merge_down_button, &_layer_flatten_all_button})
            _main.push_back(button);

        _layer_move_up_button.set_tooltip_title("Move Layer Up");
        _layer_move_up_button.set_tooltip_description("Move layer above selected layer");

        _layer_move_down_button.set_tooltip_title("Move Layer Down");
        _layer_move_down_button.set_tooltip_description("Move layer below selected layer");

        _layer_create_button.set_tooltip_title("New Layer");
        _layer_create_button.set_tooltip_description("Create layer above selected layer");

        _layer_duplicate_button.set_tooltip_title("Duplicate Layer");
        _layer_duplicate_button.set_tooltip_description("Create copy of selected layer");

        _layer_merge_down_button.set_tooltip_title("Merge Layer Down");
        _layer_merge_down_button.set_tooltip_description("Combine selected layer with layer below");

        _layer_flatten_all_button.set_tooltip_title("Flatten All Layers");
        _layer_flatten_all_button.set_tooltip_description("Merge all layers in order");

        _layer_delete_button.set_tooltip_title("Delete Layer");
        _layer_delete_button.set_tooltip_description("Delete layer, unless it is the only layer");
    }

    void LayerView::LayerControlBar::update()
    {
        _layer_move_up_button.set_can_respond_to_input(_owner->_selected_layer != 0);
        _layer_move_down_button.set_can_respond_to_input(_owner->_selected_layer < state::layers.size() - 1);
        _layer_merge_down_button.set_can_respond_to_input(_owner->_selected_layer < state::layers.size() - 1);
        _layer_delete_button.set_can_respond_to_input(state::layers.size() > 1);
        _layer_flatten_all_button.set_can_respond_to_input(state::layers.size() > 1);

        // TODO
        _layer_merge_down_button.set_can_respond_to_input(false);
        _layer_flatten_all_button.set_can_respond_to_input(false);
        // TODO
    }
}

