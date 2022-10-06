// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void LayerView::FrameControlBar::on_frame_move_left_button_clicked(Button*, FrameControlBar* instance)
    {
        size_t current_pos = instance->_owner->_selected_frame;
        size_t new_pos = instance->_owner->_selected_frame - 1;

        if (current_pos == 0)
            return;

        for (size_t i = 0; i < state::layers.size(); ++i)
        {
            auto* layer = state::layers.at(i);

            auto temp = Layer::Frame(layer->frames.at(current_pos));
            layer->frames.at(current_pos) = layer->frames.at(new_pos);
            layer->frames.at(new_pos) = temp;

            instance->_owner->_layer_rows.at(i).update_frame(current_pos);
            instance->_owner->_layer_rows.at(i).update_frame(new_pos);
        }

        instance->_owner->set_selection(instance->_owner->_selected_layer, new_pos);
    }

    void LayerView::FrameControlBar::on_frame_move_right_button_clicked(Button*, FrameControlBar* instance)
    {
        size_t current_pos = instance->_owner->_selected_frame;
        size_t new_pos = instance->_owner->_selected_frame + 1;

        if (current_pos == state::n_frames - 1)
            return;

        for (size_t i = 0; i < state::layers.size(); ++i)
        {
            auto* layer = state::layers.at(i);

            auto temp = Layer::Frame(layer->frames.at(current_pos));
            layer->frames.at(current_pos) = layer->frames.at(new_pos);
            layer->frames.at(new_pos) = temp;

            instance->_owner->_layer_rows.at(i).update_frame(current_pos);
            instance->_owner->_layer_rows.at(i).update_frame(new_pos);
        }

        instance->_owner->set_selection(instance->_owner->_selected_layer, new_pos);
    }

    void LayerView::FrameControlBar::on_frame_create_left_of_this_button_clicked(Button*, FrameControlBar* instance)
    {
        const size_t frame_i = instance->_owner->_selected_frame;
        const size_t layer_i = instance->_owner->_selected_layer;

        for (auto* layer : state::layers)
        {
            auto it = layer->frames.emplace(layer->frames.begin() + frame_i);
            it->texture = new Texture();
            it->image = new Image();
            it->image->create(state::layer_resolution.x, state::layer_resolution.y, RGBA(1, 1, 1, 0));
            it->texture->create_from_image(*it->image);
            it->is_tween_repeat = false;
        }

        state::n_frames += 1;
        instance->_owner->set_selection(layer_i, frame_i);

        for (auto& row : instance->_owner->_layer_rows)
            row.insert_frame(frame_i);
    }

    void LayerView::FrameControlBar::on_frame_create_right_of_this_button_clicked(Button*, FrameControlBar* instance)
    {
        const size_t frame_i = instance->_owner->_selected_frame;
        const size_t layer_i = instance->_owner->_selected_layer;

        for (auto* layer : state::layers)
        {
            auto it = layer->frames.emplace(layer->frames.begin() + frame_i + 1);
            it->texture = new Texture();
            it->image = new Image();
            it->image->create(state::layer_resolution.x, state::layer_resolution.y, RGBA(1, 1, 1, 0));
            it->texture->create_from_image(*it->image);
            it->is_tween_repeat = false;
        }

        state::n_frames += 1;
        instance->_owner->set_selection(layer_i, frame_i+1);

        for (auto& row : instance->_owner->_layer_rows)
            row.insert_frame(frame_i);
    }

    void LayerView::FrameControlBar::on_frame_duplicate_button_clicked(Button*, FrameControlBar* instance)
    {
        const size_t frame_i = instance->_owner->_selected_frame;
        const size_t layer_i = instance->_owner->_selected_layer;

        for (auto* layer : state::layers)
        {
            auto it = layer->frames.emplace(layer->frames.begin() + frame_i + 1);
            it->texture = new Texture();
            it->image = new Image(*layer->frames.at(frame_i).image);
            it->texture->create_from_image(*it->image);
            it->is_tween_repeat = true;
        }

        state::n_frames += 1;
        instance->_owner->set_selection(layer_i, frame_i+1);

        for (auto& row : instance->_owner->_layer_rows)
            row.insert_frame(frame_i);
    }

    void LayerView::FrameControlBar::on_frame_delete_button_clicked(Button*, FrameControlBar* instance)
    {
        const size_t frame_i = instance->_owner->_selected_frame;
        for (auto* layer : state::layers)
        {
            delete layer->frames.at(frame_i).image;
            delete layer->frames.at(frame_i).texture;
            layer->frames.erase(layer->frames.begin() + frame_i);
        }

        state::n_frames -= 1;
        for (auto& row : instance->_owner->_layer_rows)
            row.delete_frame(instance->_owner->_selected_frame);
    }

    void LayerView::FrameControlBar::on_frame_keyframe_toggle_button_toggled(ToggleButton* button, FrameControlBar* instance)
    {
        bool is_key = not button->get_active();
        state::layers.at(instance->_owner->_selected_layer)->frames.at(instance->_owner->_selected_frame).is_tween_repeat = not is_key;
        instance->_owner->_layer_rows.at(instance->_owner->_selected_layer).update_frame(instance->_owner->_selected_frame);

        instance->set_selected_frame_is_keyframe(is_key);
    }

    void LayerView::FrameControlBar::set_selected_frame_is_keyframe(bool is_key)
    {
        if (is_key)
            _frame_keyframe_toggle_button.set_child(&_frame_is_keyframe_icon);
        else
            _frame_keyframe_toggle_button.set_child(&_frame_is_not_keyframe_icon);

        _frame_keyframe_toggle_button.set_signal_toggled_blocked(true);
        _frame_keyframe_toggle_button.set_active(not is_key);
        _frame_keyframe_toggle_button.set_signal_toggled_blocked(false);
    }

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
    {
        const size_t layer_i = _owner->_selected_layer;
        const size_t frame_i = _owner->_selected_frame;
        auto& frame = state::layers.at(layer_i)->frames.at(frame_i);

        _frame_move_left_button.set_can_respond_to_input(frame_i != 0);
        _frame_move_right_button.set_can_respond_to_input(frame_i < state::n_frames-1);

        set_selected_frame_is_keyframe(not frame.is_tween_repeat);
    }

    LayerView::FrameControlBar::operator Widget*()
    {
        return &_main;
    }
}