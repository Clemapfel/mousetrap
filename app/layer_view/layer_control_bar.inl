// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void LayerView::LayerControlBar::on_layer_move_up_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_move_down_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_create_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_delete_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_duplicate_button_clicked(Button*, LayerControlBar* instance)
    {}

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
    }
}

