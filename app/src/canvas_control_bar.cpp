//
// Created by clem on 2/25/23.
//

#include <app/canvas.hpp>

#include <sstream>

namespace mousetrap
{
    Canvas::ControlBar::ControlBar(Canvas* owner)
        : _owner(owner)
    {
        _grid_visible_toggle_button.set_child(&_grid_visible_icon);
        _grid_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_grid_visible(button->get_active());
        }, this);

        _background_visible_toggle_button.set_child(&_background_visible_icon);
        _background_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_background_visible(button->get_active());
        }, this);

        _horizontal_symmetry_toggle_button.set_child(&_horizontal_symmetry_icon);
        _horizontal_symmetry_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_horizontal_symmetry_active(button->get_active());
        }, this);

        _vertical_symmetry_toggle_button.set_child(&_vertical_symmetry_icon);
        _vertical_symmetry_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_vertical_symmetry_active(button->get_active());
        }, this);

        _symmetry_control_menu_popover.set_child(*_symmetry_control_menu);
        _symmetry_control_menu_button.set_popover(&_symmetry_control_menu_popover);
        _symmetry_control_menu_button.set_size_request(_vertical_symmetry_icon.get_size());
        _symmetry_control_menu_button.set_child(&_symmetry_control_label);

        _brush_outline_visible_toggle_button.set_child(&_brush_outline_visible_icon);
        _brush_outline_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_brush_outline_visible(button->get_active());
        }, this);

        _scale_scale.connect_signal_value_changed([](SpinButton* scale, ControlBar* instance) {
            instance->_owner->set_scale(scale->get_value());
        }, this);

        _reset_view_button.set_action(state::actions::canvas_reset_transform);
        _reset_view_button.set_child(&_reset_view_icon);

        _flip_horizontally_button.set_child(&_flip_horizontally_icon);
        _flip_horizontally_button.connect_signal_clicked([](Button* button, auto){
            active_state->set_image_flip_apply_scope(ApplyScope::CURRENT_CELL);
            active_state->set_image_flip(true, false);
            active_state->apply_image_flip();
        }, this);

        _flip_vertically_button.set_child(&_flip_vertically_icon);
        _flip_vertically_button.connect_signal_clicked([](Button* button, auto){
            active_state->set_image_flip_apply_scope(ApplyScope::CURRENT_CELL);
            active_state->set_image_flip(false, true);
            active_state->apply_image_flip();
        }, this);

        for (auto* icon : {
            &_grid_visible_icon,
            &_background_visible_icon,
            &_horizontal_symmetry_icon,
            &_vertical_symmetry_icon,
            &_brush_outline_visible_icon,
            &_reset_view_icon,
            &_flip_horizontally_icon,
            &_flip_vertically_icon,
            &_invert_selection_icon,
            &_allow_draw_outside_selection_icon
        })
        {
            icon->set_size_request(icon->get_size());
        }

        _scale_scale.set_expand(false);
        _position_label.set_expand(false);

        auto insert_spacer = [&](){
            auto sep = SeparatorLine();
            (&sep)->set_opacity(0);
            (&sep)->set_expand(true);
            _scrolled_window_box.push_back(&sep);
        };

        _position_label.set_text("");

        //_visibility_box.push_back(&_brush_outline_visible_toggle_button);
        _visibility_box.push_back(&_grid_visible_toggle_button);
        _visibility_box.push_back(&_background_visible_toggle_button);

        _flip_box.push_back(&_flip_vertically_button);
        _flip_box.push_back(&_flip_horizontally_button);

        _mode_dropdown.push_back(&_replace_list_label, &_replace_selected_label, [](auto){
            active_state->set_selection_mode(SelectionMode::REPLACE);
        }, this);

        _mode_dropdown.push_back(&_add_list_label, &_add_selected_label, [](auto){
            active_state->set_selection_mode(SelectionMode::ADD);
        }, this);

        _mode_dropdown.push_back(&_subtract_list_label, &_subtract_selected_label, [](auto){
            active_state->set_selection_mode(SelectionMode::SUBTRACT);
        }, this);


        float selected_label_max_width = 0;
        for (auto* label : {
            &_replace_list_label,
            &_replace_selected_label,
            &_add_list_label,
            &_add_selected_label,
            &_subtract_list_label,
            &_subtract_selected_label
        })
        {
            float size = label->get_preferred_size().natural_size.x;
            if (size > selected_label_max_width)
                selected_label_max_width = size;
        }

        for (auto* label : {
            &_replace_list_label,
            &_replace_selected_label,
            &_add_list_label,
            &_add_selected_label,
            &_subtract_list_label,
            &_subtract_selected_label
        })
            label->set_size_request({selected_label_max_width, 0});

        _allow_draw_outside_selection_button.set_active(active_state->get_allow_draw_outside_selection());
        _allow_draw_outside_selection_button.connect_signal_toggled([](ToggleButton* button, auto){
            active_state->set_allow_draw_outside_selection(button->get_active());
            state::actions::canvas_toggle_allow_drawing_outside_selection.set_state(button->get_active());
        }, this);
        _allow_draw_outside_selection_button.set_child(&_allow_draw_outside_selection_icon);

        _invert_selection_button.set_action(state::actions::canvas_invert_selection);
        _invert_selection_button.set_child(&_invert_selection_icon);
        _selection_box.push_back(&_invert_selection_button);
        _selection_box.push_back(&_allow_draw_outside_selection_button);
        _selection_box.push_back(&_mode_dropdown);

        _view_box.push_back(&_reset_view_button);
        _view_box.push_back(&_scale_scale);

        _symmetry_box.push_back(&_horizontal_symmetry_toggle_button);
        _symmetry_box.push_back(&_vertical_symmetry_toggle_button);
        _symmetry_box.push_back(&_symmetry_control_menu_button);

        _symmetry_box.set_halign(GTK_ALIGN_END);
        _symmetry_box.set_hexpand(false);

        float spacer_size = _reset_view_button.get_preferred_size().natural_size.x;
        _view_spacer.set_size_request({spacer_size, 0});

        auto add_spacer = [&](){
            auto* spacer = new SeparatorLine();
            spacer->set_hexpand(false);
            spacer->set_size_request({spacer_size, 0});
            _scrolled_window_box.push_back(spacer);
        };

        _scrolled_window_box.push_back(&_view_box);
        _scrolled_window_box.push_back(&_view_spacer);
        _scrolled_window_box.push_back(&_visibility_box);
        add_spacer();
        _scrolled_window_box.push_back(&_flip_box);
        add_spacer();
        _scrolled_window_box.push_back(&_selection_box);
        add_spacer();
        _scrolled_window_box.push_back(&_symmetry_box);

        _scrolled_window.set_child(&_scrolled_window_box);
        _scrolled_window.set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
        _scrolled_window.set_propagate_natural_height(true);

        set_scale(_owner->_scale);
        set_horizontal_symmetry_active(_owner->_horizontal_symmetry_active);
        set_vertical_symmetry_active(_owner->_vertical_symmetry_active);
        set_cursor_in_bounds(_position_visible);
        set_brush_outline_visible(_owner->_brush_outline_visible);

        auto tooltip = [](const std::string& value) {
            return state::tooltips_file->get_value("canvas", value);
        };

        _position_label.set_tooltip_text(tooltip("position_label"));
        _scale_scale.set_tooltip_text(tooltip("scale_scale"));
        _brush_outline_visible_toggle_button.set_tooltip_text(tooltip("toggle_brush_outline_visible"));
        _grid_visible_toggle_button.set_tooltip_text(tooltip("toggle_grid_visible"));
        _background_visible_toggle_button.set_tooltip_text(tooltip("toggle_background_visible"));
        _horizontal_symmetry_toggle_button.set_tooltip_text(tooltip("toggle_horizontal_symmetry_active"));
        _vertical_symmetry_toggle_button.set_tooltip_text(tooltip("toggle_vertical_symmetry_active"));
        _flip_horizontally_button.set_tooltip_text(tooltip("flip_horizontally"));
        _flip_vertically_button.set_tooltip_text(tooltip("flip_vertically"));
        _symmetry_control_menu_button.set_tooltip_text(tooltip("symmetry_control_menu"));
        _reset_view_button.set_tooltip_text(tooltip("reset_transform"));

        _invert_selection_button.set_tooltip_text(tooltip("invert_selection"));
        _allow_draw_outside_selection_button.set_tooltip_text(tooltip("allow_drawing_outside_selection"));
        _replace_list_label.set_tooltip_text(tooltip("selection_mode_replace_verbose"));
        _add_list_label.set_tooltip_text(tooltip("selection_mode_add_verbose"));
        _subtract_list_label.set_tooltip_text(tooltip("selection_mode_subtract_verbose"));
        _replace_selected_label.set_tooltip_text(tooltip("selection_mode_replace_verbose"));
        _add_selected_label.set_tooltip_text(tooltip("selection_mode_add_verbose"));
        _subtract_selected_label.set_tooltip_text(tooltip("selection_mode_subtract_verbose"));
    }

    Canvas::ControlBar::operator Widget*()
    {
        return &_scrolled_window;
    }

    void Canvas::ControlBar::set_grid_visible(bool b)
    {
        _grid_visible_toggle_button.set_signal_toggled_blocked(true);
        _grid_visible_toggle_button.set_active(b);
        _grid_visible_toggle_button.set_signal_toggled_blocked(false);
        state::actions::canvas_toggle_grid_visible.set_state(b);
    }

    void Canvas::ControlBar::set_background_visible(bool b)
    {
        _background_visible_toggle_button.set_signal_toggled_blocked(true);
        _background_visible_toggle_button.set_active(b);
        _background_visible_toggle_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::set_horizontal_symmetry_active(bool b)
    {
        _horizontal_symmetry_toggle_button.set_signal_toggled_blocked(true);
        _horizontal_symmetry_toggle_button.set_active(b);
        _horizontal_symmetry_toggle_button.set_signal_toggled_blocked(false);

        _symmetry_control_menu->set_horizontal_symmetry_active(b);
    }

    void Canvas::ControlBar::set_vertical_symmetry_active(bool b)
    {
        _vertical_symmetry_toggle_button.set_signal_toggled_blocked(true);
        _vertical_symmetry_toggle_button.set_active(b);
        _vertical_symmetry_toggle_button.set_signal_toggled_blocked(false);

        _symmetry_control_menu->set_vertical_symmetry_active(b);
    }

    void Canvas::ControlBar::set_horizontal_symmetry_pixel_position(size_t px)
    {
        _symmetry_control_menu->set_horizontal_symmetry_pixel_position(px);
    }

    void Canvas::ControlBar::set_vertical_symmetry_pixel_position(size_t px)
    {
        _symmetry_control_menu->set_vertical_symmetry_pixel_position(px);
    }

    void Canvas::ControlBar::set_brush_outline_visible(bool b)
    {
        _brush_outline_visible_toggle_button.set_signal_toggled_blocked(true);
        _brush_outline_visible_toggle_button.set_active(b);
        _brush_outline_visible_toggle_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::set_cursor_position(Vector2i xy)
    {
        std::stringstream str;
        str << "<tt>[" << xy.x << " | " << xy.y << "]</tt>";
        //_position_label.set_text(str.str());
    }

    void Canvas::ControlBar::set_cursor_in_bounds(bool b)
    {
        _position_visible = b;
        _position_label.set_opacity(_position_visible ? 1 : 0);
    }

    void Canvas::ControlBar::set_scale(float scale)
    {
        _scale_scale.set_signal_value_changed_blocked(true);
        _scale_scale.set_value(scale);
        _scale_scale.set_signal_value_changed_blocked(false);
    }

    void Canvas::ControlBar::on_layer_resolution_changed()
    {
        _symmetry_control_menu->on_layer_resolution_changed();
    }

    Canvas::ControlBar::SymmetryControlMenu::SymmetryControlMenu(ControlBar* owner)
        : _owner(owner)
    {
        _h_spin_button.connect_signal_value_changed([](SpinButton* scale, SymmetryControlMenu* instance){
            instance->_owner->_owner->set_horizontal_symmetry_pixel_position(scale->get_value());
        }, this);

        _h_center_button.connect_signal_clicked([](Button* button, SymmetryControlMenu* instance) {
            instance->_owner->_owner->set_horizontal_symmetry_pixel_position(active_state->get_layer_resolution().x / 2);
        }, this);

        _h_check_button.connect_signal_toggled([](CheckButton* button, SymmetryControlMenu* instance){
            instance->_owner->_owner->set_horizontal_symmetry_active(button->get_active());
        }, this);

        _v_spin_button.connect_signal_value_changed([](SpinButton* scale, SymmetryControlMenu* instance){
            instance->_owner->_owner->set_vertical_symmetry_pixel_position(scale->get_value());
        }, this);

        _v_center_button.connect_signal_clicked([](Button* button, SymmetryControlMenu* instance) {
            instance->_owner->_owner->set_vertical_symmetry_pixel_position(active_state->get_layer_resolution().x / 2);
        }, this);

        _v_check_button.connect_signal_toggled([](CheckButton* button, SymmetryControlMenu* instance){
            instance->_owner->_owner->set_vertical_symmetry_active(button->get_active());
        }, this);


        _h_center_button.set_child(&_h_center_button_label);
        _v_center_button.set_child(&_v_center_button_label);

        _h_check_button_box.push_back(&_h_enabled_label);
        _h_check_button_box.push_back(&_h_check_button);
        _h_button_box.push_back(&_h_position_label);
        _h_button_box.push_back(&_h_spin_button);
        _h_button_box.push_back(&_h_center_button);

        _v_check_button_box.push_back(&_v_enabled_label);
        _v_check_button_box.push_back(&_v_check_button);
        _v_button_box.push_back(&_v_position_label);
        _v_button_box.push_back(&_v_spin_button);
        _v_button_box.push_back(&_v_center_button);

        _main.push_back(&_title_label);
        _main.push_back(&_h_label);
        _main.push_back(&_h_check_button_box);
        _main.push_back(&_h_button_box);
        _main.push_back(&_v_label);
        _main.push_back(&_v_check_button_box);
        _main.push_back(&_v_button_box);

        on_layer_resolution_changed();
        set_horizontal_symmetry_active(_owner->_owner->_horizontal_symmetry_active);
        set_vertical_symmetry_active(_owner->_owner->_vertical_symmetry_active);
        set_horizontal_symmetry_pixel_position(active_state->get_layer_resolution().x / 2);
        set_vertical_symmetry_pixel_position(active_state->get_layer_resolution().y / 2);
    }

    Canvas::ControlBar::SymmetryControlMenu::operator Widget*()
    {
        return &_main;
    }

    void Canvas::ControlBar::SymmetryControlMenu::set_horizontal_symmetry_active(bool b)
    {
        _h_check_button.set_signal_toggled_blocked(true);
        _h_check_button.set_active(b);
        _h_spin_button.set_can_respond_to_input(b);
        _h_center_button.set_can_respond_to_input(b);
        _h_check_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::SymmetryControlMenu::set_vertical_symmetry_active(bool b)
    {
        _v_check_button.set_signal_toggled_blocked(true);
        _v_check_button.set_active(b);
        _v_spin_button.set_can_respond_to_input(b);
        _v_center_button.set_can_respond_to_input(b);
        _v_check_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::SymmetryControlMenu::set_vertical_symmetry_pixel_position(size_t px)
    {
        _v_spin_button.set_signal_value_changed_blocked(true);
        _v_spin_button.set_value(px);
        _v_spin_button.set_signal_value_changed_blocked(false);
    }

    void Canvas::ControlBar::SymmetryControlMenu::set_horizontal_symmetry_pixel_position(size_t px)
    {
        _h_spin_button.set_signal_value_changed_blocked(true);
        _h_spin_button.set_value(px);
        _h_spin_button.set_signal_value_changed_blocked(false);
    }

    void Canvas::ControlBar::SymmetryControlMenu::on_layer_resolution_changed()
    {
        _h_spin_button.set_upper_limit(active_state->get_layer_resolution().x - 1);
        _v_spin_button.set_upper_limit(active_state->get_layer_resolution().y - 1);
    }
}
