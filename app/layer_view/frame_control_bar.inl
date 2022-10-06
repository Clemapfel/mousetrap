// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void LayerView::FrameControlBar::on_frame_move_left_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_move_right_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_create_left_of_this_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_create_right_of_this_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_duplicate_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_delete_button_clicked(Button*, FrameControlBar* instance)
    {
        for (auto* layer : state::layers)
            layer->frames.erase(layer->frames.begin() + instance->_owner->_selected_frame);

        state::n_frames -= 1;
        for (auto& row : instance->_owner->_layer_rows)
            row.delete_frame(instance->_owner->_selected_frame);
    }

    void LayerView::FrameControlBar::on_frame_keyframe_toggle_button_toggled(ToggleButton*, FrameControlBar* instance)
    {}

    LayerView::FrameControlBar::FrameControlBar(LayerView* owner)
            : _owner(owner)
    {
        for (auto* display : {&_frame_move_left_icon, &_frame_create_left_of_this_icon, &_frame_delete_icon, &_frame_duplicate_icon, &_frame_create_right_of_this_icon, &_frame_move_right_icon, &_frame_is_keyframe_icon, &_frame_is_not_keyframe_icon})
            display->set_size_request(display->get_size());

        _frame_move_left_button.set_child(&_frame_move_left_icon);
        _frame_move_left_button.connect_signal_clicked(on_frame_move_left_button_clicked, this);

        _frame_create_left_of_this_button.set_child(&_frame_create_left_of_this_icon);
        _frame_create_left_of_this_button.connect_signal_clicked(on_frame_create_left_of_this_button_clicked, this);

        _frame_delete_button.set_child(&_frame_delete_icon);
        _frame_delete_button.connect_signal_clicked(on_frame_delete_button_clicked, this);

        _frame_keyframe_toggle_button.set_child(&_frame_is_keyframe_icon);
        _frame_keyframe_toggle_button.connect_signal_toggled(on_frame_keyframe_toggle_button_toggled, this);

        _frame_duplicate_button.set_child(&_frame_duplicate_icon);
        _frame_duplicate_button.connect_signal_clicked(on_frame_duplicate_button_clicked, this);

        _frame_create_right_of_this_button.set_child(&_frame_create_right_of_this_icon);
        _frame_create_right_of_this_button.connect_signal_clicked(on_frame_create_right_of_this_button_clicked, this);

        _frame_move_right_button.set_child(&_frame_move_right_icon);
        _frame_move_right_button.connect_signal_clicked(on_frame_move_right_button_clicked, this);

        _main.push_back(&_frame_move_left_button);
        _main.push_back(&_frame_create_left_of_this_button);
        _main.push_back(&_frame_keyframe_toggle_button);
        _main.push_back(&_frame_duplicate_button);
        _main.push_back(&_frame_delete_button);
        _main.push_back(&_frame_create_right_of_this_button);
        _main.push_back(&_frame_move_right_button);
    }

    void LayerView::FrameControlBar::update()
    {}

    LayerView::FrameControlBar::operator Widget*()
    {
        return &_main;
    }
}