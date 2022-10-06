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
    }
}

