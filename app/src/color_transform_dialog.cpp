//
// Created by clem on 2/5/23.
//

#include <app/color_transform_dialog.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    ColorTransformDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ColorTransformDialog::present()
    {
        _dialog.present();
    }

    ColorTransformDialog::ColorTransformDialog()
    {
        _window.set_modal(false);

        _h_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_h_offset(scale->get_value());
        }, this);

        _s_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_s_offset(scale->get_value());
        }, this);

        _v_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_v_offset(scale->get_value());
        }, this);

        _r_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_r_offset(scale->get_value());
        }, this);

        _g_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_g_offset(scale->get_value());
        }, this);

        _b_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_b_offset(scale->get_value());
        }, this);

        _a_offset_scale.connect_signal_value_changed([](Scale* scale, ColorTransformDialog* instance){
            instance->set_a_offset(scale->get_value());
        }, this);

        _h_offset_box.push_back(&_h_label);
        _h_offset_box.push_back(&_h_offset_spacer);
        _h_offset_box.push_back(&_h_offset_scale);

        _s_offset_box.push_back(&_s_label);
        _s_offset_box.push_back(&_s_offset_spacer);
        _s_offset_box.push_back(&_s_offset_scale);

        _v_offset_box.push_back(&_v_label);
        _v_offset_box.push_back(&_v_offset_spacer);
        _v_offset_box.push_back(&_v_offset_scale);

        _r_offset_box.push_back(&_r_label);
        _r_offset_box.push_back(&_r_offset_spacer);
        _r_offset_box.push_back(&_r_offset_scale);

        _g_offset_box.push_back(&_g_label);
        _g_offset_box.push_back(&_g_offset_spacer);
        _g_offset_box.push_back(&_g_offset_scale);

        _b_offset_box.push_back(&_b_label);
        _b_offset_box.push_back(&_b_offset_spacer);
        _b_offset_box.push_back(&_b_offset_scale);

        _a_offset_box.push_back(&_a_label);
        _a_offset_box.push_back(&_a_offset_spacer);
        _a_offset_box.push_back(&_a_offset_scale);

        for (auto* label : {
            &_h_label, &_s_label, &_v_label, &_r_label, &_g_label, &_b_label, &_a_label
        })
        {
            label->set_justify_mode(JustifyMode::LEFT);
            label->set_halign(GTK_ALIGN_START);
            label->set_margin_horizontal(state::margin_unit);
        }

        for (auto* spacer : {
                &_h_offset_spacer, &_s_offset_spacer, &_v_offset_spacer, &_r_offset_spacer, &_g_offset_spacer, &_b_offset_spacer, &_a_offset_spacer
        })
        {
            spacer->set_hexpand(true);
            spacer->set_opacity(0);
        }

        for (auto* scale : {
            &_h_offset_scale, &_s_offset_scale, &_v_offset_scale, &_r_offset_scale, &_g_offset_scale, &_b_offset_scale, &_a_offset_scale
        })
        {
            scale->set_margin_start(state::margin_unit);
            scale->set_draw_value(true);
            scale->set_vexpand(false);
            scale->set_size_request({20 * state::margin_unit, 0});
        }

        for (auto* box : {
                &_h_offset_box, &_s_offset_box, &_v_offset_box, &_r_offset_box, &_g_offset_box, &_b_offset_box, &_a_offset_box
        })
        {
            box->set_margin_horizontal(state::margin_unit);
        }

        _apply_to_dropdown.push_back(&_everywhere_list_label, &_everywhere_when_selected_label, [](ColorTransformDialog* instance){
            active_state->set_color_offset_apply_scope(EVERYWHERE);
        }, this);
        
        _apply_to_dropdown.push_back(&_current_layer_list_label, &_current_layer_when_selected_label, [](ColorTransformDialog* instance){
            active_state->set_color_offset_apply_scope(CURRENT_LAYER);
        }, this);

        _apply_to_dropdown.push_back(&_current_frame_list_label, &_current_frame_when_selected_label, [](ColorTransformDialog* instance){
            active_state->set_color_offset_apply_scope(CURRENT_FRAME);
        }, this);

        _apply_to_dropdown.push_back(&_current_cell_list_label, &_current_cell_when_selected_label, [](ColorTransformDialog* instance){
            active_state->set_color_offset_apply_scope(CURRENT_CELL);
        }, this);

        _apply_to_dropdown.push_back(&_selection_list_label, &_selection_when_selected_label, [](ColorTransformDialog* instance){
            active_state->set_color_offset_apply_scope(SELECTION);
        }, this);

        _apply_to_dropdown.set_margin_horizontal(state::margin_unit);

        for (auto* label : {
            &_offset_label, &_offset_instruction_label, &_apply_to_label
        })
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_justify_mode(JustifyMode::LEFT);
            label->set_margin_horizontal(state::margin_unit);
        }

        // layout

        _offset_label.set_margin_vertical(state::margin_unit);

        _window_box.push_back(&_offset_label);
        _window_box.push_back(&_offset_instruction_label);
        _window_box.set_homogeneous(false);

        auto add_spacer = [&](float top_margin = 0, float bottom_margin = 0)
        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_horizontal(state::margin_unit);
            spacer->set_margin_top(top_margin);
            spacer->set_margin_bottom(bottom_margin);
            _window_box.push_back(spacer);
        };

        add_spacer(state::margin_unit, 0);

        _window_box.push_back(&_h_offset_box);
        add_spacer();

        _window_box.push_back(&_s_offset_box);
        add_spacer();

        _window_box.push_back(&_v_offset_box);
        add_spacer();

        _window_box.push_back(&_r_offset_box);
        add_spacer();

        _window_box.push_back(&_g_offset_box);
        add_spacer();

        _window_box.push_back(&_b_offset_box);
        add_spacer();

        _window_box.push_back(&_a_offset_box);
        add_spacer(0, state::margin_unit);

        _apply_to_dropdown.set_margin(state::margin_unit);
        _window_box.push_back(&_apply_to_label);
        _window_box.push_back(&_apply_to_dropdown);

        add_spacer();

        _dialog.get_content_area().push_back(&_window_box);

        _accept_button.set_child(&_accept_label);
        _cancel_button.set_child(&_cancel_label);
        _reset_button.set_child(&_reset_label);

        _accept_button.connect_signal_clicked([](Button*, ColorTransformDialog* instance){

            active_state->apply_color_offset();

            instance->_dialog.close();
            instance->reset();
        }, this);

        _cancel_button.connect_signal_clicked([](Button*, ColorTransformDialog* instance){
            instance->_dialog.close();
            instance->reset();
        }, this);

        _reset_button.connect_signal_clicked([](Button*, ColorTransformDialog* instance){
            instance->reset();
        }, this);

        _dialog.connect_signal_close([](Dialog*, ColorTransformDialog* instance) -> bool{
            instance->reset();
            return false;
        }, this);

        auto button_size = std::max<float>(_accept_button.get_preferred_size().natural_size.x, _cancel_button.get_preferred_size().natural_size.x);
        for (auto* button : {&_accept_button, &_cancel_button, &_reset_button})
        {
            button->set_size_request({button_size, 0});
            button->set_hexpand(false);
        }

        _button_box_spacer.set_hexpand(true);
        _button_box_spacer.set_opacity(0);

        _button_box.set_margin(state::margin_unit);
        _cancel_button.set_margin_end(state::margin_unit);

        _button_box.push_back(&_reset_button);
        _button_box.push_back(&_button_box_spacer);
        _button_box.push_back(&_cancel_button);
        _button_box.push_back(&_accept_button);
        _window_box.push_back(&_button_box);

        state::actions::color_transform_dialog_open.set_function([](){
            state::color_transform_dialog->present();
        });
        state::add_shortcut_action(state::actions::color_transform_dialog_open);

        set_h_offset(_h_offset);
        set_s_offset(_s_offset);
        set_v_offset(_v_offset);
        set_r_offset(_r_offset);
        set_g_offset(_g_offset);
        set_b_offset(_b_offset);
        set_a_offset(_a_offset);

        state::actions::color_transform_dialog_invert.set_function([](){
            active_state->color_invert(ApplyScope::EVERYWHERE);
        });
        state::add_shortcut_action(state::actions::color_transform_dialog_invert);

        state::actions::color_transform_dialog_to_grayscale.set_function([](){
            active_state->color_to_grayscale(ApplyScope::EVERYWHERE);
        });
        state::add_shortcut_action(state::actions::color_transform_dialog_to_grayscale);

        auto add_tooltip_text = [](const std::string& id, Widget* widget) {
            widget->set_tooltip_text(state::tooltips_file->get_value("color_transform_dialog", id));
        };

        add_tooltip_text("hue", &_h_offset_box);
        add_tooltip_text("saturation", &_s_offset_box);
        add_tooltip_text("value", &_v_offset_box);
        add_tooltip_text("red", &_r_offset_box);
        add_tooltip_text("green", &_g_offset_box);
        add_tooltip_text("blue", &_b_offset_box);
        add_tooltip_text("opacity", &_a_offset_box);
    }

    void ColorTransformDialog::update_preview()
    {
        active_state->set_color_offset(
            _h_offset,
            _s_offset,
            _v_offset,
            _r_offset,
            _g_offset,
            _b_offset,
            _a_offset
        );
    }

    void ColorTransformDialog::set_h_offset(float v)
    {
        _h_offset = v;
        _h_offset_scale.set_signal_value_changed_blocked(true);
        _h_offset_scale.set_value(v);
        _h_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::set_s_offset(float v)
    {
        _s_offset = v;
        _s_offset_scale.set_signal_value_changed_blocked(true);
        _s_offset_scale.set_value(v);
        _s_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::set_v_offset(float v)
    {
        _v_offset = v;
        _v_offset_scale.set_signal_value_changed_blocked(true);
        _v_offset_scale.set_value(v);
        _v_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::set_r_offset(float v)
    {
        _r_offset = v;
        _r_offset_scale.set_signal_value_changed_blocked(true);
        _r_offset_scale.set_value(v);
        _r_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::set_g_offset(float v)
    {
        _g_offset = v;
        _g_offset_scale.set_signal_value_changed_blocked(true);
        _g_offset_scale.set_value(v);
        _g_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::set_b_offset(float v)
    {
        _b_offset = v;
        _b_offset_scale.set_signal_value_changed_blocked(true);
        _b_offset_scale.set_value(v);
        _b_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::set_a_offset(float v)
    {
        _a_offset = v;
        _a_offset_scale.set_signal_value_changed_blocked(true);
        _a_offset_scale.set_value(v);
        _a_offset_scale.set_signal_value_changed_blocked(false);
        update_preview();
    }

    void ColorTransformDialog::reset()
    {
        set_h_offset(0);
        set_s_offset(0);
        set_v_offset(0);
        set_r_offset(0);
        set_g_offset(0);
        set_b_offset(0);
        set_a_offset(0);
    }
}