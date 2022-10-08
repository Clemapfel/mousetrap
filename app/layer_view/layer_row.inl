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
        _layer_frame_display_list_view_wrapper.set_child(&_layer_frame_display_list_view);
        _layer_frame_display_list_view_wrapper.set_scroll_to_focus(false);
        _layer_frame_display_list_view_viewport.set_child(&_layer_frame_display_list_view_wrapper);
        _layer_frame_display_list_view_viewport.set_hexpand(true);
        _layer_frame_display_list_view_viewport.set_policy(GTK_POLICY_EXTERNAL, GTK_POLICY_NEVER);

        _main.push_back(_layer_property_options);
        _main.push_back(&_layer_frame_display_list_view_viewport);
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

        _layer_property_options.update();
    }

    void LayerView::LayerRow::delete_frame(size_t to_delete)
    {
        _layer_frame_display_list_view.clear();
        _layer_frame_displays.erase(_layer_frame_displays.end() - 1);

        assert(state::n_frames == _layer_frame_displays.size());

        auto& layer = state::layers.at(_layer);
        for (size_t i = 0; i < state::n_frames; ++i)
            _layer_frame_displays.at(i).set_frame(i);

        for (auto& display : _layer_frame_displays)
            _layer_frame_display_list_view.push_back(display);
    }

    void LayerView::LayerRow::insert_frame(size_t frame_i)
    {
        _layer_frame_displays.emplace_back(_layer, state::n_frames-1, _owner);
        _layer_frame_display_list_view.push_back(_layer_frame_displays.back());

        assert(state::n_frames == _layer_frame_displays.size());

        for (size_t i = frame_i; i < state::n_frames; ++i)
            _layer_frame_displays.at(i).set_frame(i);

        update_frame(frame_i+1);
    }

    void LayerView::LayerRow::update_frame(size_t i)
    {
        _layer_frame_displays.at(i).update();
    }

    void LayerView::LayerRow::set_layer(size_t i)
    {
        _layer = i;
        for (auto& frame : _layer_frame_displays)
            frame.set_layer(_layer);

        _layer_property_options.update();
    }

    void LayerView::LayerRow::connect_viewport_hadjustment(Adjustment& adjustment)
    {
        _layer_frame_display_list_view_viewport.set_hadjustment(adjustment);
    }
}
