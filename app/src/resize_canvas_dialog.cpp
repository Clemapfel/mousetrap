//
// Created by clem on 1/27/23.
//

#include <app/resize_canvas_dialog.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::ButtonAndArrow(Alignment alignment, const std::string& id)
        : _alignment(alignment), _arrow(get_resource_path() + "icons/alignment_arrow_center.png") //" + id)
    {
        _button.set_child(&_arrow);
    }

    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::operator Widget*()
    {
        return &_button;
    }

    ResizeCanvasDialog::AlignmentSelector::AlignmentSelector(ResizeCanvasDialog* owner)
        : _owner(owner)
    {
        _arrows = {
            {TOP_LEFT, new ButtonAndArrow(TOP_LEFT, "top_left")},
            {TOP, new ButtonAndArrow(TOP, "top")},
            {TOP_RIGHT, new ButtonAndArrow(TOP_RIGHT, "top_right")},
            {RIGHT, new ButtonAndArrow(RIGHT, "right")},
            {BOTTOM_RIGHT, new ButtonAndArrow(BOTTOM_RIGHT, "bottom_right")},
            {BOTTOM, new ButtonAndArrow(BOTTOM, "bottom")},
            {BOTTOM_LEFT, new ButtonAndArrow(BOTTOM_LEFT, "bottom_left")},
            {LEFT, new ButtonAndArrow(LEFT, "left")},
            {CENTER, new ButtonAndArrow(CENTER, "center")}
        };

        for (auto& pair : _arrows)
        {
            pair.second->_button.connect_signal_toggled([](ToggleButton* button, ButtonAndArrow* instance) {
                state::resize_canvas_dialog->set_alignment(instance->_alignment);
            }, pair.second);
        }

        _top_row.push_back(*_arrows.at(TOP_LEFT));
        _top_row.push_back(*_arrows.at(TOP));
        _top_row.push_back(*_arrows.at(TOP_RIGHT));

        _center_row.push_back(*_arrows.at(LEFT));
        _center_row.push_back(*_arrows.at(CENTER));
        _center_row.push_back(*_arrows.at(RIGHT));

        _bottom_row.push_back(*_arrows.at(BOTTOM_LEFT));
        _bottom_row.push_back(*_arrows.at(BOTTOM));
        _bottom_row.push_back(*_arrows.at(BOTTOM_RIGHT));

        _all_rows.push_back(&_top_row);
        _all_rows.push_back(&_center_row);
        _all_rows.push_back(&_bottom_row);

        _frame.set_child(&_all_rows);
    }

    ResizeCanvasDialog::AlignmentSelector::~AlignmentSelector() noexcept
    {
        for (auto& pair : _arrows)
            delete pair.second;
    }

    void ResizeCanvasDialog::AlignmentSelector::set_alignment(Alignment alignment)
    {
        for (auto& pair : _arrows)
        {
            auto& button = pair.second->_button;
            button.set_signal_toggled_blocked(true);
            button.set_active(pair.first == alignment);
            button.set_signal_toggled_blocked(false);

            pair.second->_arrow.set_opacity(pair.first == alignment ? 1 : 0);
        }
    }

    ResizeCanvasDialog::AlignmentSelector::operator Widget*()
    {
        return &_frame;
    }

    void ResizeCanvasDialog::set_alignment(Alignment a)
    {
        _alignment = a;
        _alignment_selector.set_alignment(a);
        _alignment_state_label.set_text("<tt>" + std::string(a) + "</tt>");
    }

    void ResizeCanvasDialog::on_layer_resolution_changed()
    {
        auto size = active_state->get_layer_resolution();
        if (_scale_mode == ABSOLUTE)
        {
            _width_spin_button.set_value(size.x);
            _height_spin_button.set_value(size.y);
        }

        auto ratio = size.x / float(size.y);
        auto ratio_string =  std::to_string(ratio);
        std::string ratio_string_formatted = "";

        if (int(ratio) == ratio)
            ratio_string_formatted = std::to_string(int(ratio));
        else
        {
            bool comma_seen = false;
            size_t post_comma_count = 0;
            for (auto c:ratio_string)
            {
                if (comma_seen and (post_comma_count++ >= 3 or c == '0'))
                    break;

                ratio_string_formatted.push_back(c);

                if (c == '.')
                    comma_seen = true;
            }
        }

        _maintain_aspect_ratio_label.set_text(state::tooltips_file->get_value("scale_canvas_dialog", "maintain_aspect_ratio_label") + " (" + ratio_string_formatted + ")");
    }

    void ResizeCanvasDialog::set_final_size(size_t w, size_t h)
    {
        _final_size_label.set_text("New Size: <tt>" + std::to_string(w) + "</tt>x<tt>" + std::to_string(h) + "</tt> px");
    }

    void ResizeCanvasDialog::set_scale_mode(ScaleMode mode)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        auto max_size = state::settings_file->get_value_as<size_t>("global", "maximum_image_size");
        if (mode == ABSOLUTE)
        {
            _width_spin_button.set_upper_limit(max_size);
            _height_spin_button.set_upper_limit(max_size);

            _width_spin_button.set_value(active_state->get_layer_resolution().x);
            _height_spin_button.set_value(active_state->get_layer_resolution().y);
        }
        else
        {
            _width_spin_button.set_upper_limit(int(max_size / float(active_state->get_layer_resolution().x)) * 100);
            _height_spin_button.set_upper_limit(int(max_size / float(active_state->get_layer_resolution().y)) * 100);

            _width_spin_button.set_value(100);
            _height_spin_button.set_value(100);
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        _scale_mode = mode;
        set_final_size(active_state->get_layer_resolution().x, active_state->get_layer_resolution().y);
    }

    void ResizeCanvasDialog::set_aspect_ratio_locked(bool b)
    {
        _aspect_ratio_locked = b;
        // TODO
    }

    void ResizeCanvasDialog::set_width(float v)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        size_t final_x = 0;
        size_t final_y = 0;

        if (_scale_mode == ABSOLUTE)
        {
            auto target = v;
            _width_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _height_spin_button.set_value(target * (active_state->get_layer_resolution().y / float(active_state->get_layer_resolution().x)));

            final_x = _width_spin_button.get_value();
            final_y = _height_spin_button.get_value();
        }
        else
        {
            auto target = v;
            _width_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _height_spin_button.set_value(v);

            final_x = _width_spin_button.get_value() / 100 *  active_state->get_layer_resolution().x;
            final_y = _height_spin_button.get_value() / 100 * active_state->get_layer_resolution().y;
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        set_final_size(final_x, final_y);
    }

    void ResizeCanvasDialog::set_height(float v)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        size_t final_x = 0;
        size_t final_y = 0;

        if (_scale_mode == ABSOLUTE)
        {
            auto target = v;
            _height_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _width_spin_button.set_value(target * (active_state->get_layer_resolution().x / float(active_state->get_layer_resolution().y)));

            final_x = _width_spin_button.get_value();
            final_y = _height_spin_button.get_value();
        }
        else
        {
            auto target = v;
            _height_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _width_spin_button.set_value(v);

            final_x = _width_spin_button.get_value() / 100 *  active_state->get_layer_resolution().x;
            final_y = _height_spin_button.get_value() / 100 * active_state->get_layer_resolution().y;
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        set_final_size(final_x, final_y);
    }

    ResizeCanvasDialog::ResizeCanvasDialog()
        : _width_spin_button(1, 1, 1),
          _height_spin_button(1, 1, 1)
    {
        set_scale_mode(_scale_mode);
        on_layer_resolution_changed();
        set_final_size(0, 0);
        set_alignment(_alignment);

        _width_spin_button.connect_signal_value_changed([](SpinButton* button, ResizeCanvasDialog* instance) {
            instance->set_width(button->get_value());
        }, this);

        _height_spin_button.connect_signal_value_changed([](SpinButton* button, ResizeCanvasDialog* instance) {
            instance->set_height(button->get_value());
        }, this);

        _width_box.push_back(&_width_label);
        _width_box.push_back(&_width_spacer);
        _width_box.push_back(&_width_spin_button);

        _height_box.push_back(&_height_label);
        _height_box.push_back(&_height_spacer);
        _height_box.push_back(&_height_spin_button);

        for (auto* label : {&_width_label, &_height_label})
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_expand(false);
        }

        for (auto* spacer : {&_width_spacer, &_height_spacer})
        {
            spacer->set_opacity(0);
            spacer->set_expand(true);
            spacer->set_halign(GTK_ALIGN_CENTER);
        }

        for (auto* button : {&_width_spin_button, &_height_spin_button})
        {
            button->set_expand(false);
            button->set_halign(GTK_ALIGN_END);
        }

        _spin_button_box.push_back(&_width_box);
        _spin_button_box.push_back(&_height_box);
        _spin_button_box.set_expand(true);
        _spin_button_box.set_margin_vertical(state::margin_unit);

        _absolute_or_relative_dropdown.push_back(&_absolute_list_label, &_absolute_when_selected_label, [](ResizeCanvasDialog* instance){
            instance->set_scale_mode(ABSOLUTE);
        }, this);

        _absolute_or_relative_dropdown.push_back(&_relative_list_label, &_relative_when_selected_label, [](ResizeCanvasDialog* instance){
            instance->set_scale_mode(RELATIVE);
        }, this);

        _absolute_or_relative_dropdown.set_margin_start(state::margin_unit);
        _absolute_or_relative_dropdown.set_align(GTK_ALIGN_CENTER);
        _absolute_or_relative_dropdown.set_expand(false);

        _absolute_list_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "absolute_dropdown_label"));
        _absolute_when_selected_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "absolute_dropdown_label"));

        _relative_list_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "relative_dropdown_label"));
        _relative_when_selected_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "relative_dropdown_label"));

        _spin_button_and_dropdown_box.push_back(&_spin_button_box);
        _spin_button_and_dropdown_box.push_back(&_absolute_or_relative_dropdown);

        _maintain_aspect_ratio_button.set_active(_aspect_ratio_locked);
        _maintain_aspect_ratio_button.connect_signal_toggled([](CheckButton* button, ResizeCanvasDialog* instance){
            instance->set_aspect_ratio_locked(button->get_active());
        }, this);
        _maintain_aspect_ratio_button.set_margin_horizontal(state::margin_unit);

        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_button);
        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_label);

        _final_size_label.set_justify_mode(JustifyMode::LEFT);
        _final_size_label.set_halign(GTK_ALIGN_START);
        _final_size_label.set_margin_top(state::margin_unit);

        _spin_button_and_dropdown_box.set_margin_start(state::margin_unit);
        _maintain_aspect_ratio_box.set_margin_start(state::margin_unit);

        _alignment_selector.operator Widget*()->set_expand(false);
        _alignment_selector.operator Widget*()->set_align(GTK_ALIGN_CENTER);

        _window_box.push_back(&_instruction_label);
        _window_box.push_back(&_spin_button_and_dropdown_box);
        _window_box.push_back(&_maintain_aspect_ratio_box);
        _window_box.push_back(&_final_size_label);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_top(state::margin_unit);
            _window_box.push_back(spacer);
        }

        _alignment_instruction_label.set_margin_vertical(state::margin_unit);

        _window_box.push_back(&_alignment_instruction_label);
        _window_box.push_back(_alignment_selector);
        _window_box.push_back(&_alignment_state_label);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_top(state::margin_unit);
            _window_box.push_back(spacer);
        }

        _instruction_label.set_justify_mode(JustifyMode::LEFT);
        _instruction_label.set_halign(GTK_ALIGN_START);
        _alignment_instruction_label.set_justify_mode(JustifyMode::LEFT);
        _alignment_instruction_label.set_halign(GTK_ALIGN_START);

        _window_box.set_margin(state::margin_unit);

        _accept_button.set_child(&_accept_button_label);
        _cancel_button.set_child(&_cancel_button_label);

        _dialog.get_content_area().push_back(&_window_box);

        _dialog.add_action_widget(&_cancel_button, [](ResizeCanvasDialog* instance){
            instance->_dialog.close();
        }, this);

        _dialog.add_action_widget(&_accept_button,[](ResizeCanvasDialog* instance){

            auto width_value = instance->_width_spin_button.get_value();
            auto height_value = instance->_height_spin_button.get_value();

            auto width = instance->_scale_mode == ABSOLUTE ? width_value  : (width_value / 100) * active_state->get_layer_resolution().x;
            auto height = instance->_scale_mode == ABSOLUTE ? height_value  : (height_value / 100) * active_state->get_layer_resolution().y;

            active_state->resize_canvas(Vector2ui(round(width), round(height)));
            instance->_dialog.close();
        }, this);

        auto button_size = std::max<float>(_accept_button.get_preferred_size().natural_size.x, _cancel_button.get_preferred_size().natural_size.x);
        for (auto* button : {&_accept_button, &_cancel_button})
        {
            button->set_margin_end(state::margin_unit);
            button->set_size_request({button_size, 0});
        }

        _accept_button.set_margin_end(state::margin_unit);
        _cancel_button.set_margin_end(state::margin_unit);
        GtkWidget* parent = gtk_widget_get_parent(_accept_button.operator GtkWidget*());
        gtk_widget_set_margin_start(parent, state::margin_unit);
        gtk_widget_set_margin_bottom(parent, state::margin_unit);

        state::actions::resize_canvas_dialog_open.set_function([](){
            state::resize_canvas_dialog->present();
        });
        state::add_shortcut_action(state::actions::resize_canvas_dialog_open);
    }

    ResizeCanvasDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ResizeCanvasDialog::present()
    {
        _dialog.present();
    }
}

/*
namespace mousetrap
{
    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::ButtonAndArrow(Alignment alignment, const std::string& id)
        : alignment(alignment), arrow(get_resource_path() + "icons/alignment_arrow_center.png") //" + id)
    {
        button.set_child(&arrow);
    }

    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::operator Widget*()
    {
        return &button;
    }

    ResizeCanvasDialog::AlignmentSelector::AlignmentSelector(ResizeCanvasDialog* owner)
        : _owner(owner)
    {
        _arrows =  {
            {TOP_LEFT, ButtonAndArrow(TOP_LEFT, "top_left")},
            {TOP, ButtonAndArrow(TOP, "top")},
            {TOP_RIGHT, ButtonAndArrow(TOP_RIGHT, "top_right")},
            {RIGHT, ButtonAndArrow(RIGHT, "right")},
            {BOTTOM_RIGHT, ButtonAndArrow(BOTTOM_RIGHT, "bottom_right")},
            {BOTTOM, ButtonAndArrow(BOTTOM, "bottom")},
            {BOTTOM_LEFT, ButtonAndArrow(BOTTOM_LEFT, "bottom_left")},
            {LEFT, ButtonAndArrow(LEFT, "left")},
            {CENTER, ButtonAndArrow(CENTER, "center")}
        };

        _top_row.push_back(_arrows.at(TOP_LEFT));
        _top_row.push_back(_arrows.at(TOP));
        _top_row.push_back(_arrows.at(TOP_RIGHT));

        _center_row.push_back(_arrows.at(LEFT));
        _center_row.push_back(_arrows.at(CENTER));
        _center_row.push_back(_arrows.at(RIGHT));

        _bottom_row.push_back(_arrows.at(BOTTOM_LEFT));
        _bottom_row.push_back(_arrows.at(BOTTOM));
        _bottom_row.push_back(_arrows.at(BOTTOM_RIGHT));

        _main.push_back(&_top_row);
        _main.push_back(&_center_row);
        _main.push_back(&_bottom_row);
    }

    ResizeCanvasDialog::AlignmentSelector::operator Widget*()
    {
        return &_main;
    }

    ResizeCanvasDialog::ResizeCanvasDialog()
    {}

    ResizeCanvasDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ResizeCanvasDialog::present()
    {
        _dialog.present();
    }
}
*/