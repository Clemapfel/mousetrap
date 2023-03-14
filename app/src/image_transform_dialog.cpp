//
// Created by clem on 2/7/23.
//

#include <app/image_transform_dialog.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    ImageTransformDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ImageTransformDialog::present()
    {
        _dialog.present();
    }

    void ImageTransformDialog::set_flip_horizontally(bool b)
    {
        _flip_horizontally = b;
        _h_flip_check_button.set_signal_toggled_blocked(true);
        _h_flip_check_button.set_active(b);
        _h_flip_check_button.set_signal_toggled_blocked(false);
        update_preview();
    }

    void ImageTransformDialog::set_flip_vertically(bool b)
    {
        _flip_vertically = b;
        _v_flip_check_button.set_signal_toggled_blocked(true);
        _v_flip_check_button.set_active(b);
        _v_flip_check_button.set_signal_toggled_blocked(false);
        update_preview();
    }

    void ImageTransformDialog::reset()
    {
        _flip_horizontally = false;
        _h_flip_check_button.set_signal_toggled_blocked(true);
        _h_flip_check_button.set_active(false);
        _h_flip_check_button.set_signal_toggled_blocked(false);
        _flip_vertically = false;
        _v_flip_check_button.set_signal_toggled_blocked(true);
        _v_flip_check_button.set_active(false);
        _v_flip_check_button.set_signal_toggled_blocked(false);
        update_preview();
    }

    void ImageTransformDialog::update_preview()
    {
        _accept_button.set_can_respond_to_input(_flip_horizontally or _flip_vertically);
        active_state->set_image_flip(_flip_horizontally, _flip_vertically);
    }

    ImageTransformDialog::ImageTransformDialog()
    {
        _h_flip_check_button.connect_signal_toggled([](CheckButton* button, ImageTransformDialog* instance){
            instance->_flip_horizontally = button->get_active();
            instance->update_preview();
        }, this);

        _v_flip_check_button.connect_signal_toggled([](CheckButton* button, ImageTransformDialog* instance){
            instance->_flip_vertically = button->get_active();
            instance->update_preview();
        }, this);

        _h_flip_box.push_back(&_h_flip_label);
        _h_flip_box.push_back(&_h_flip_spacer);
        _h_flip_box.push_back(&_h_flip_check_button);

        _v_flip_box.push_back(&_v_flip_label);
        _v_flip_box.push_back(&_v_flip_spacer);
        _v_flip_box.push_back(&_v_flip_check_button);

        for (auto* spacer : {&_h_flip_spacer, &_v_flip_spacer})
        {
            spacer->set_hexpand(true);
            spacer->set_opacity(0);
        }

        _apply_to_dropdown.push_back(&_everywhere_list_label, &_everywhere_when_selected_label, [](ImageTransformDialog* instance){
            active_state->set_image_flip_apply_scope(EVERYWHERE);
        }, this);

        _apply_to_dropdown.push_back(&_current_layer_list_label, &_current_layer_when_selected_label, [](ImageTransformDialog* instance){
            active_state->set_image_flip_apply_scope(CURRENT_LAYER);
        }, this);

        _apply_to_dropdown.push_back(&_current_frame_list_label, &_current_frame_when_selected_label, [](ImageTransformDialog* instance){
            active_state->set_image_flip_apply_scope(CURRENT_FRAME);
        }, this);

        _apply_to_dropdown.push_back(&_current_cell_list_label, &_current_cell_when_selected_label, [](ImageTransformDialog* instance){
            active_state->set_image_flip_apply_scope(CURRENT_CELL);
        }, this);

        _apply_to_dropdown.push_back(&_selection_list_label, &_selection_when_selected_label, [](ImageTransformDialog* instance){
            active_state->set_image_flip_apply_scope(SELECTION);
        }, this);

        _apply_to_dropdown.set_margin_horizontal(state::margin_unit);

        _accept_button.set_child(&_accept_label);
        _cancel_button.set_child(&_cancel_label);

        _accept_button.connect_signal_clicked([](Button*, ImageTransformDialog* instance){

            active_state->apply_image_flip();

            instance->_dialog.close();
            instance->reset();
        }, this);

        _cancel_button.connect_signal_clicked([](Button*, ImageTransformDialog* instance){
            instance->_dialog.close();
            instance->reset();
        }, this);

        _dialog.connect_signal_close([](Dialog*, ImageTransformDialog* instance) -> bool{
            instance->reset();
            return false;
        }, this);

        auto button_size = std::max<float>(_accept_button.get_preferred_size().natural_size.x, _cancel_button.get_preferred_size().natural_size.x);
        for (auto* button : {&_accept_button, &_cancel_button})
        {
            button->set_size_request({button_size, 0});
            button->set_hexpand(false);
        }

        _button_box_spacer.set_hexpand(true);
        _button_box_spacer.set_opacity(0);

        _button_box.set_margin(state::margin_unit);
        _cancel_button.set_margin_end(state::margin_unit);

        _button_box.push_back(&_button_box_spacer);
        _button_box.push_back(&_cancel_button);
        _button_box.push_back(&_accept_button);

        // layout

        auto add_spacer = [&](float top_margin = 0, float bottom_margin = 0) {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_horizontal(state::margin_unit);
            spacer->set_margin_top(top_margin);
            spacer->set_margin_bottom(bottom_margin);
            _window_box.push_back(spacer);
        };

        _window_box.push_back(&_instruction_label);
        _window_box.push_back(&_h_flip_box);
        _window_box.push_back(&_v_flip_box);
        add_spacer(state::margin_unit);

        _window_box.push_back(&_apply_to_label);
        _window_box.push_back(&_apply_to_dropdown);
        add_spacer(state::margin_unit);

        _window_box.push_back(&_button_box);
        _dialog.get_content_area().push_back(&_window_box);

        //_window_box.set_margin_horizontal(state::margin_unit);
        _h_flip_label.set_margin_start(state::margin_unit);
        _v_flip_label.set_margin_start(state::margin_unit);
        _h_flip_check_button.set_margin_end(3 * state::margin_unit);
        _v_flip_check_button.set_margin_end(3 * state::margin_unit);
        _h_flip_box.set_margin_horizontal(state::margin_unit);
        _h_flip_box.set_margin_bottom(0.5 * state::margin_unit);
        _v_flip_box.set_margin_horizontal(state::margin_unit);

        for (auto* label : {&_apply_to_label, &_instruction_label})
        {
            label->set_margin_start(state::margin_unit);
            label->set_halign(GTK_ALIGN_START);
            label->set_justify_mode(JustifyMode::LEFT);
            label->set_margin_vertical(state::margin_unit);
        }

        // actions

        state::actions::image_transform_dialog_open.set_function([](){

            if (state::image_transform_dialog)
                state::image_transform_dialog->present();
        });

        state::actions::image_transform_dialog_flip_horizontally.set_function([](){
            active_state->set_image_flip_apply_scope(EVERYWHERE);
            active_state->set_image_flip(true, false);
            active_state->apply_image_flip();
        });

        state::actions::image_transform_dialog_flip_vertically.set_function([](){
            active_state->set_image_flip_apply_scope(EVERYWHERE);
            active_state->set_image_flip(false, true);
            active_state->apply_image_flip();
        });

        state::actions::image_transform_dialog_rotate_clockwise.set_function([](){
            active_state->rotate_clockwise();
        });

        state::actions::image_transform_dialog_rotate_counterclockwise.set_function([](){
            active_state->rotate_counterclockwise();
        });

        for (auto* action : {
            &state::actions::image_transform_dialog_open,
            &state::actions::image_transform_dialog_flip_horizontally,
            &state::actions::image_transform_dialog_flip_vertically,
            &state::actions::image_transform_dialog_rotate_clockwise,
            &state::actions::image_transform_dialog_rotate_counterclockwise
        })
        {
            state::add_shortcut_action(*action);
        }


        set_flip_horizontally(_flip_horizontally);
        set_flip_vertically(_flip_vertically);
    }
}
