//
// Created by clem on 1/23/23.
//

#include <app/layer_view.hpp>

namespace mousetrap
{
    LayerView::LayerPreview::LayerPreview(size_t layer_i)
            : _layer_i(layer_i)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
    }

    LayerView::LayerPreview::~LayerPreview()
    {
        delete _layer_shape;
        delete _transparency_tiling_shape;
    }

    LayerView::LayerPreview::operator Widget*()
    {
        return &_area;
    }

    void LayerView::LayerPreview::set_texture(const Texture* texture)
    {
        if (_layer_shape != nullptr)
            _layer_shape->set_texture(texture);

        _area.queue_render();
    }

    void LayerView::LayerPreview::set_opacity(float v)
    {
        if (_layer_shape != nullptr)
            _layer_shape->set_color(RGBA(1, 1, 1, v));

        _area.queue_render();
    }

    void LayerView::LayerPreview::set_visible(bool b)
    {
        /*
        if (_layer_shape != nullptr)
            _layer_shape->set_visible(b);

        if (_transparency_tiling_shape != nullptr)
            _transparency_tiling_shape->set_visible(b);
            */

        _area.set_opacity(b ? 1 : state::settings_file->get_value_as<float>("frame_view", "hidden_layer_opacity"));
        _area.queue_render();
    }

    void LayerView::LayerPreview::set_locked(bool b)
    {
        // noop
    }

    void LayerView::LayerPreview::on_realize(Widget* widget, LayerPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_tiling_shader == nullptr)
        {
            _transparency_tiling_shader = new Shader();
            _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_layer_shape = new Shape();

        for (auto* shape : {instance->_transparency_tiling_shape, instance->_layer_shape})
            shape->as_rectangle({0, 0}, {1, 1});

        area->clear_render_tasks();

        auto transparency_task = RenderTask(instance->_transparency_tiling_shape, _transparency_tiling_shader);
        transparency_task.register_vec2("_canvas_size", &instance->_canvas_size);

        area->add_render_task(transparency_task);
        area->add_render_task(instance->_layer_shape);

        instance->set_texture(active_state->get_frame(instance->_layer_i, active_state->get_current_frame_index())->get_texture());
        instance->set_opacity(active_state->get_layer(instance->_layer_i)->get_opacity());
        instance->set_visible(active_state->get_layer(instance->_layer_i)->get_is_visible());

        area->queue_render();
    }

    void LayerView::LayerPreview::on_resize(GLArea* area, int w, int h, LayerPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_area.queue_render();
    }

    void LayerView::LayerPreview::set_preview_size(size_t px)
    {
        auto height = px;
        auto width = active_state->get_layer_resolution().x / active_state->get_layer_resolution().y * height;
        _area.set_size_request({width, height});
    }

    void LayerView::LayerPreview::queue_render()
    {
        _area.queue_render();
    }

    LayerView::LayerRow::LayerRow(LayerView* owner, size_t layer_i)
            : _owner(owner), _layer_i(layer_i), _layer_preview(layer_i)
    {
        _layer_preview.set_preview_size(_owner->_preview_size);

        _layer_preview_aspect_frame.set_child(_layer_preview);
        _layer_preview_list_view.push_back(&_layer_preview_aspect_frame);

        _visible_locked_buttons_box.push_back(&_is_visible_toggle_button);
        _visible_locked_buttons_box.push_back(&_is_locked_toggle_button);
        set_visible(active_state->get_layer(_layer_i)->get_is_visible());
        set_locked(active_state->get_layer(_layer_i)->get_is_locked());

        for (auto* button : {&_is_visible_toggle_button, &_is_locked_toggle_button})
        {
            button->set_expand(false);
            button->set_align(GTK_ALIGN_CENTER);
        }

        _is_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, LayerRow* instance){
            active_state->set_layer_visible(instance->_layer_i, button->get_active());
        }, this);

        _is_locked_toggle_button.connect_signal_toggled([](ToggleButton* button, LayerRow* instance){
            active_state->set_layer_locked(instance->_layer_i, button->get_active());
        }, this);

        for (auto* icon : {&_is_visible_icon, &_is_not_visible_icon, &_is_locked_icon, &_is_not_locked_icon})
            icon->set_size_request(icon->get_size());

        set_name(active_state->get_layer(_layer_i)->get_name());
        _layer_name_entry.set_hexpand(true);
        _layer_name_entry.set_vexpand(false);
        _layer_name_entry.set_has_frame(false);
        _layer_name_entry.connect_signal_activate([](Entry* entry, LayerRow* instance){
            active_state->set_layer_name(instance->_layer_i, entry->get_text());
        }, this);

        _opacity_level_bar.set_value(float(rand()) / RAND_MAX);
        _opacity_level_bar.set_opacity(0.5);
        _opacity_level_bar.set_inverted(true);
        _opacity_level_bar.set_margin_vertical(state::margin_unit * 0.5);
        _opacity_menu_button.set_margin_vertical(state::margin_unit * 0.5);

        _opacity_menu_button.set_always_show_arrow(false);

        _opacity_level_bar.set_expand(true);
        _layer_name_entry.set_expand(true);
        _opacity_level_bar_name_entry_overlay.set_child(&_opacity_level_bar);
        _opacity_level_bar_name_entry_overlay.add_overlay(&_layer_name_entry);

        using blend_mode_drop_down_data = struct {BlendMode mode; LayerRow* instance;};

        for (auto& pair: blend_mode_to_label)
        {
            auto* label_item = _blend_mode_drop_down_label_items.emplace_back(new Label("<tt>" + pair.second.first + "</tt>"));
            auto* list_item = _blend_mode_drop_down_list_items.emplace_back(new Label(pair.second.second));

            list_item->set_halign(GTK_ALIGN_START);

            _blend_mode_drop_down.push_back(
                list_item,
                label_item,
                [](blend_mode_drop_down_data data) {
                    auto i = data.instance->_blend_mode_drop_down.get_selected();

                    std::string key;
                    auto mode = blend_mode_to_label.at(i).first;

                    if (mode == BlendMode::NORMAL)
                        key = "normal";
                    else if (mode == BlendMode::ADD)
                        key = "add";
                    else if (mode == BlendMode::SUBTRACT)
                        key = "subtract";
                    else if (mode == BlendMode::REVERSE_SUBTRACT)
                        key = "reverse_subtract";
                    else if (mode == BlendMode::MULTIPLY)
                        key = "multiply";
                    else if (mode == BlendMode::MIN)
                        key = "min";
                    else if (mode == BlendMode::MAX)
                        key = "max";

                    data.instance->_blend_mode_drop_down.set_tooltip_text(state::tooltips_file->get_value("layer_view", "blend_mode_" + key + "_label"));
                    active_state->set_layer_blend_mode(data.instance->_layer_i, mode);

                }, blend_mode_drop_down_data{pair.first, this}
            );
        }

        _blend_mode_drop_down.set_expand(false);
        _blend_mode_drop_down.set_valign(GTK_ALIGN_CENTER);
        _blend_mode_drop_down.set_halign(GTK_ALIGN_END);

        _opacity_scale.connect_signal_value_changed([](Scale* scale, LayerRow* instance) {
            active_state->set_layer_opacity(instance->_layer_i, scale->get_value());
        }, this);

        _opacity_spin_button.connect_signal_value_changed([](SpinButton* scale, LayerRow* instance){
            active_state->set_layer_opacity(instance->_layer_i, scale->get_value());
        }, this);

        set_opacity(active_state->get_layer(layer_i)->get_opacity());

        _opacity_scale.set_hexpand(true);
        _opacity_scale.set_size_request({state::margin_unit * 20, 0});
        _opacity_spin_button.set_hexpand(false);
        _opacity_scale_box.push_back(&_opacity_scale);
        _opacity_scale_box.push_back(&_opacity_spin_button);

        _opacity_header.set_halign(GTK_ALIGN_CENTER);
        _opacity_header.set_valign(GTK_ALIGN_START);
        _opacity_header.set_margin_bottom(state::margin_unit);

        _opacity_box.push_back(&_opacity_header);
        _opacity_box.push_back(&_opacity_scale_box);

        _opacity_popover.set_child(&_opacity_box);
        _opacity_menu_button.set_popover(&_opacity_popover);
        _opacity_popover.set_relative_position(GTK_POS_TOP);

        _inner_box.push_back(&_visible_locked_buttons_box);
        _inner_box.push_back(&_layer_preview_list_view);
        _inner_box.push_back(&_opacity_level_bar_name_entry_overlay);
        _inner_box.push_back(&_opacity_menu_button);
        _inner_box.push_back(&_blend_mode_drop_down);
        _inner_box.set_margin_horizontal(state::margin_unit);

        _outer_box.push_back(&_inner_box);

        // tooltips

        auto get_tooltip = [](const std::string key) -> std::string {
            return state::tooltips_file->get_value("layer_view", key);
        };

        _is_visible_toggle_button.set_tooltip_text(get_tooltip("is_visible_button"));
        _is_locked_toggle_button.set_tooltip_text(get_tooltip("is_locked_button"));
        _opacity_menu_button.set_tooltip_text(get_tooltip("opacity_button"));

        for (size_t i = 0; i < blend_mode_to_label.size(); ++i)
        {
            std::string key;
            auto mode = blend_mode_to_label.at(i).first;

            if (mode == BlendMode::NORMAL)
                key = "normal";
            else if (mode == BlendMode::ADD)
                key = "add";
            else if (mode == BlendMode::SUBTRACT)
                key = "subtract";
            else if (mode == BlendMode::REVERSE_SUBTRACT)
                key = "reverse_subtract";
            else if (mode == BlendMode::MULTIPLY)
                key = "multiply";
            else if (mode == BlendMode::MIN)
                key = "min";
            else if (mode == BlendMode::MAX)
                key = "max";

            _blend_mode_drop_down_label_items.at(i)->set_tooltip_text(get_tooltip("blend_mode_" + key + "_label"));
            _blend_mode_drop_down_list_items.at(i)->set_tooltip_text(get_tooltip("blend_mode_" + key + "_explanation"));
        }
    }

    LayerView::LayerRow::operator Widget*()
    {
        return &_outer_box;
    }

    void LayerView::LayerRow::set_blend_mode(BlendMode mode)
    {
        size_t blend_mode_i = 0;
        for (auto& pair : blend_mode_to_label)
        {
            if (pair.first == active_state->get_layer(_layer_i)->get_blend_mode())
                break;

            blend_mode_i += 1;
        }
        _blend_mode_drop_down.set_selected(blend_mode_i);
    }

    void LayerView::LayerRow::set_opacity(float v)
    {
        _opacity_level_bar.set_value(1 - v);
        _layer_preview.set_opacity(v);

        _opacity_scale.set_signal_value_changed_blocked(true);
        _opacity_scale.set_value(v);
        _opacity_scale.set_signal_value_changed_blocked(false);

        _opacity_spin_button.set_signal_value_changed_blocked(true);
        _opacity_spin_button.set_value(v);
        _opacity_spin_button.set_signal_value_changed_blocked(false);
    }

    void LayerView::LayerRow::set_visible(bool b)
    {
        _is_visible_toggle_button.set_signal_toggled_blocked(true);

        if (b)
        {
            _is_visible_toggle_button.set_active(true);
            _is_visible_toggle_button.set_child(&_is_visible_icon);
        }
        else
        {
            _is_visible_toggle_button.set_active(false);
            _is_visible_toggle_button.set_child(&_is_not_visible_icon);
        }

        _is_visible_toggle_button.set_signal_toggled_blocked(false);
        _layer_preview.set_visible(b);
    }

    void LayerView::LayerRow::set_locked(bool b)
    {
        _is_locked_toggle_button.set_signal_toggled_blocked(true);

        if (b)
        {
            _is_locked_toggle_button.set_active(true);
            _is_locked_toggle_button.set_child(&_is_locked_icon);
        }
        else
        {
            _is_locked_toggle_button.set_active(false);
            _is_locked_toggle_button.set_child(&_is_not_locked_icon);
        }

        _is_locked_toggle_button.set_signal_toggled_blocked(false);
        _layer_preview.set_locked(b);

        float opacity = b ? 0.5 : 1;
        _layer_name_entry.set_opacity(opacity);
    }

    void LayerView::LayerRow::set_name(const std::string& name)
    {
        _layer_name_entry.set_signal_activate_blocked(true);
        _layer_name_entry.set_text(name);
        _layer_name_entry.set_signal_activate_blocked(false);
    }

    void LayerView::LayerRow::set_preview_size(size_t px)
    {
        _layer_preview.set_preview_size(px);
    }

    void LayerView::LayerRow::set_texture(const Texture* texture)
    {
        _layer_preview.set_texture(texture);
    }

    void LayerView::LayerRow::set_all_signals_blocked(bool b)
    {
        _is_visible_toggle_button.set_signal_toggled_blocked(b);
        _is_locked_toggle_button.set_signal_toggled_blocked(b);
        _layer_name_entry.set_signal_activate_blocked(b);
        _blend_mode_drop_down.set_signal_selection_blocked(b);
        _opacity_scale.set_signal_value_changed_blocked(b);
        _opacity_spin_button.set_signal_value_changed_blocked(b);
    }

    LayerView::LayerView()
    {
        on_layer_count_changed();

        _layer_rows_list_view.set_show_separators(true);
        _layer_rows_list_view.get_selection_model()->connect_signal_selection_changed([](SelectionModel* model, size_t i, size_t, LayerView* instance){
            active_state->set_current_layer_and_frame(instance->_layer_rows_list_view.get_n_items() - i - 1, active_state->get_current_frame_index());
        }, this);
        _layer_rows_list_view.get_selection_model()->select(active_state->get_current_layer_index());

        _layer_rows_scrolled_window_spacer.set_size_request({0, state::margin_unit * 10});

        _layer_rows_scrolled_window_box.push_back(&_layer_rows_list_view);
        _layer_rows_scrolled_window_box.push_back(&_layer_rows_scrolled_window_spacer);
        _layer_rows_scrolled_window_spacer.set_expand(true);

        _layer_rows_scrolled_window.set_child(&_layer_rows_scrolled_window_box);
        _layer_rows_scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _layer_rows_scrolled_window.set_propagate_natural_height(true);
        _layer_rows_scrolled_window.set_expand(true);

        // control bar

        for (auto* display : {&_layer_move_up_icon, &_layer_create_icon, &_layer_duplicate_icon, &_layer_delete_icon, &_layer_move_down_icon, &_layer_merge_down_icon, &_layer_flatten_all_icon})
            display->set_size_request(display->get_size());

        using namespace state::actions;

        _layer_move_up_button.set_child(&_layer_move_up_icon);
        _layer_move_up_button.set_action(layer_view_layer_move_up);

        _layer_create_button.set_child(&_layer_create_icon);
        _layer_create_button.set_action(layer_view_layer_new_above_current);

        _layer_duplicate_button.set_child(&_layer_duplicate_icon);
        _layer_duplicate_button.set_action(layer_view_layer_duplicate);

        _layer_delete_button.set_child(&_layer_delete_icon);
        _layer_delete_button.set_action(layer_view_layer_delete);

        _layer_move_down_button.set_child(&_layer_move_down_icon);
        _layer_move_down_button.set_action(layer_view_layer_move_down);

        _layer_merge_down_button.set_child(&_layer_merge_down_icon);
        _layer_merge_down_button.set_action(layer_view_layer_merge_down);

        _layer_flatten_all_button.set_child(&_layer_flatten_all_icon);
        _layer_flatten_all_button.set_action(layer_view_layer_flatten_all);

        for (auto* button : {
                &_layer_move_up_button,
                &_layer_move_down_button,
                &_layer_create_button,
                &_layer_duplicate_button,
                &_layer_delete_button,
                &_layer_merge_down_button,
                &_layer_flatten_all_button}
                )
        {
            button->set_vexpand(false);
            button->set_hexpand(true);
            _control_bar_box.push_back(button);
        }

        _control_bar_box.set_hexpand(true);

        // tooltips

        _tooltip.create_from("layer_view", state::tooltips_file, state::keybindings_file);
        _header_menu_button.set_tooltip_widget(_tooltip);

        auto layer_create_tooltip = state::tooltips_file->get_value("layer_view", "layer_create");
        _layer_create_button.set_tooltip_text(layer_create_tooltip);

        auto layer_delete_tooltip = state::tooltips_file->get_value("layer_view", "layer_delete");
        _layer_delete_button.set_tooltip_text(layer_delete_tooltip);

        auto layer_duplicate_tooltip = state::tooltips_file->get_value("layer_view", "layer_duplicate");
        _layer_duplicate_button.set_tooltip_text(layer_duplicate_tooltip);

        auto layer_merge_down_tooltip = state::tooltips_file->get_value("layer_view", "layer_merge_down");
        _layer_merge_down_button.set_tooltip_text(layer_merge_down_tooltip);

        auto layer_flatten_all_tooltip = state::tooltips_file->get_value("layer_view", "layer_flatten_all");
        _layer_flatten_all_button.set_tooltip_text(layer_flatten_all_tooltip);

        auto layer_move_up_tooltip = state::tooltips_file->get_value("layer_view", "layer_move_up");
        _layer_move_up_button.set_tooltip_text(layer_move_up_tooltip);

        auto layer_move_down_tooltip = state::tooltips_file->get_value("layer_view", "layer_move_down");
        _layer_move_down_button.set_tooltip_text(layer_move_down_tooltip);

        auto set_layer_visible_tooltip = state::tooltips_file->get_value("layer_view", "set_layer_visible");
        auto set_layer_locked_tooltip = state::tooltips_file->get_value("layer_view", "set_layer_locked");

        _preview_size_spin_button.set_margin_start(state::margin_unit);
        _preview_size_spin_button.set_value(_preview_size);
        _preview_size_spin_button.connect_signal_value_changed([](SpinButton* scale, LayerView* instance) {
            instance->set_preview_size(scale->get_value());
        }, this);
        _preview_size_box.push_back(&_preview_size_label);

        auto spacer = SeparatorLine();
        spacer.set_opacity(0);
        spacer.set_hexpand(true);
        _preview_size_box.push_back(&spacer);

        _preview_size_box.push_back(&_preview_size_spin_button);
        _preview_size_box.set_margin(state::margin_unit);

        auto settings_section = MenuModel();
        auto preview_size_submenu = MenuModel();
        preview_size_submenu.add_widget(&_preview_size_box);
        settings_section.add_submenu("Preview Size...", &preview_size_submenu);

        _menu.add_section("Settings", &settings_section);

        auto create_section = MenuModel();
        create_section.add_action("New Layer Above Current", layer_view_layer_new_above_current.get_id());
        create_section.add_action("New Layer Below Current", layer_view_layer_new_below_current.get_id());
        create_section.add_action(layer_delete_tooltip, layer_view_layer_delete.get_id());
        create_section.add_action(layer_duplicate_tooltip, layer_view_layer_duplicate.get_id());
        _menu.add_section("Create / Delete", &create_section);

        auto merge_section = MenuModel();
        merge_section.add_action(layer_merge_down_tooltip, layer_view_layer_merge_down.get_id());
        merge_section.add_action(layer_flatten_all_tooltip, layer_view_layer_flatten_all.get_id());
        _menu.add_section("Merge", &merge_section);

        auto other_section = MenuModel();
        other_section.add_action(layer_move_up_tooltip, layer_view_layer_move_up.get_id());
        other_section.add_action(layer_move_down_tooltip, layer_view_layer_move_down.get_id());
        other_section.add_action(set_layer_visible_tooltip, layer_view_toggle_layer_visible.get_id());
        other_section.add_action(set_layer_locked_tooltip, layer_view_toggle_layer_locked.get_id());

        _menu.add_section("Other", &other_section);

        _header_menu_button.set_child(&_header_menu_button_label);

        _header_menu_button_popover.refresh_widgets();
        _header_menu_button.set_popover(&_header_menu_button_popover);

        _main.set_homogeneous(false);
        _main.push_back(&_header_menu_button);
        _main.push_back(&_layer_rows_scrolled_window);
        _main.push_back(&_control_bar_box);

        // actions

        layer_view_layer_new_above_current.set_function([](){
           active_state->add_layer(active_state->get_current_layer_index());
        });

        layer_view_layer_new_below_current.set_function([](){
            active_state->add_layer(active_state->get_current_layer_index()-1);
        });

        layer_view_layer_duplicate.set_function([](){
            auto current_i = active_state->get_current_layer_index();
            active_state->duplicate_layer(current_i, *active_state->get_layer(current_i));
            active_state->set_current_layer_and_frame(current_i, active_state->get_current_frame_index());
        });

        layer_view_layer_delete.set_function([](){
            active_state->delete_layer(active_state->get_current_layer_index());
        });

        layer_view_layer_move_up.set_function([]()
        {
           auto current = active_state->get_current_layer_index();
           if (current > 0)
           {
               auto next = current - 1;
               active_state->swap_layers(current, next);
               active_state->set_current_layer_and_frame(next, active_state->get_current_frame_index());
           }
        });

        layer_view_layer_move_down.set_function([]()
        {
            auto current = active_state->get_current_layer_index();
            if (current < active_state->get_n_layers() - 1)
            {
                auto next = current + 1;
                active_state->swap_layers(current, next);
                active_state->set_current_layer_and_frame(next, active_state->get_current_frame_index());
            }
        });

        for (auto* action : {
            &layer_view_layer_move_up,
            &layer_view_layer_move_down,
            &layer_view_layer_new_above_current,
            &layer_view_layer_new_below_current,
            &layer_view_layer_duplicate,
            &layer_view_layer_delete,
            &layer_view_layer_merge_down,
            &layer_view_layer_flatten_all,
            &layer_view_layer_create_from_visible,
            &layer_view_toggle_layer_visible,
            &layer_view_toggle_layer_locked,
            &layer_view_show_all_other_layers,
            &layer_view_hide_all_other_layers
        })
            state::add_shortcut_action(*action);
    }

    void LayerView::set_preview_size(size_t v)
    {
        _preview_size = v;

        for (auto& row : _layer_rows)
            row.set_preview_size(_preview_size);
    }

    void LayerView::on_layer_frame_selection_changed()
    {
        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
            _layer_rows.at(layer_i).set_texture(active_state->get_frame(layer_i, active_state->get_current_frame_index())->get_texture());

        _layer_rows_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _layer_rows_list_view.get_selection_model()->select(_layer_rows_list_view.get_n_items() - active_state->get_current_layer_index() - 1);
        _layer_rows_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    void LayerView::on_layer_image_updated()
    {
        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
            _layer_rows.at(layer_i).set_texture(active_state->get_frame(layer_i, active_state->get_current_frame_index())->get_texture());
    }

    void LayerView::on_layer_properties_changed()
    {
        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
        {
            auto& row = _layer_rows.at(layer_i);
            const auto* layer = active_state->get_layer(layer_i);

            row.set_opacity(layer->get_opacity());
            row.set_visible(layer->get_is_visible());
            row.set_locked(layer->get_is_locked());
            row.set_blend_mode(layer->get_blend_mode());
            row.set_name(layer->get_name());
        }
    }

    void LayerView::on_layer_count_changed()
    {
        _layer_rows_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);

        _layer_rows_list_view.clear();
        _layer_rows.clear();

        while (_layer_rows.size() < active_state->get_n_layers())
            _layer_rows.emplace_back(this, _layer_rows.size());

        for (auto& row : _layer_rows)
        {
            row.set_all_signals_blocked(true);
            _layer_rows_list_view.push_front(row);
            row.set_all_signals_blocked(false);
        }

        _layer_rows_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);

        on_layer_properties_changed();
        on_layer_frame_selection_changed(); // also updates texture
    }

    LayerView::operator Widget*()
    {
        return &_main;
    }
}
