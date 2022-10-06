// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    LayerView::LayerRow::LayerRow(size_t layer, LayerView* owner)
            : _layer(layer), _owner(owner), _layer_property_options(layer, owner)
    {
        _layer_frame_display_list_view.set_focusable(false);

        _main.push_back(_layer_property_options);
        _main.push_back(&_layer_frame_display_list_view);
    }

    LayerView::LayerRow::operator Widget*()
    {
        return &_main;
    }

    void LayerView::LayerRow::on_layer_frame_display_list_view_selection_changed(SelectionModel*, size_t first_item_position,
                                                                            size_t n_items_changed, LayerRow* instance)
    {
        instance->_owner->set_selection(instance->_layer, first_item_position);
    }

    void LayerView::LayerRow::update_selection()
    {
        _layer_frame_display_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _layer_frame_display_list_view.get_selection_model()->select(_owner->_selected_frame);
        _layer_frame_display_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);

        for (auto& display : _layer_frame_displays)
            display.update_selection();
    }

    void LayerView::LayerRow::update()
    {
        if (_layer_frame_displays.size() != state::n_frames)
        {
            _layer_frame_display_list_view.clear();
            _layer_frame_displays.clear();

            for (size_t i = 0; i < state::n_frames; ++i)
                _layer_frame_displays.emplace_back(_layer, i, _owner);

            for (auto& display : _layer_frame_displays)
                _layer_frame_display_list_view.push_back(display);
        }

        _layer_frame_display_list_view.get_selection_model()->connect_signal_selection_changed(
                on_layer_frame_display_list_view_selection_changed, this);

        for (auto& display : _layer_frame_displays)
            display.update();
    }
}
