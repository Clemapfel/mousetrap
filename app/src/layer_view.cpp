#include <app/layer_view.hpp>
#include <app/canvas.hpp>
#include <app/frame_view.hpp>
#include <app/animation_preview.hpp>

namespace mousetrap
{
    LayerView::LayerPreview::LayerPreview(Layer* layer, size_t frame_i)
            : _layer(layer), _frame_i(frame_i)
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

    void LayerView::LayerPreview::set_layer(Layer* layer)
    {
        _layer = layer;

        if (_layer_shape != nullptr)
            _layer_shape->set_texture(_layer->frames.at(_frame_i).texture);
    }

    void LayerView::LayerPreview::set_frame(size_t i)
    {
        _frame_i = i;

        if (_layer_shape != nullptr)
            _layer_shape->set_texture(_layer->frames.at(i).texture);
    }

    void LayerView::LayerPreview::set_opacity(float v)
    {
        if (_layer_shape != nullptr)
            _layer_shape->set_color(RGBA(1, 1, 1, v));

        _area.queue_render();
    }

    void LayerView::LayerPreview::set_visible(bool b)
    {
        if (_layer_shape != nullptr)
            _layer_shape->set_visible(b);

        if (_transparency_tiling_shape != nullptr)
            _transparency_tiling_shape->set_visible(b);

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

        instance->set_frame(instance->_frame_i);
        instance->set_layer(instance->_layer);
        instance->set_opacity(instance->_layer->opacity);

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
        auto width = active_state->layer_resolution.x / active_state->layer_resolution.y * height;
        _area.set_size_request({width, height});
    }

    void LayerView::LayerPreview::queue_render()
    {
        _area.queue_render();
    }

    // ###

    LayerView::LayerRow::LayerRow(LayerView* owner, Layer* layer, size_t frame_i)
            : _owner(owner), _layer(layer), _frame_i(frame_i), _layer_preview(layer, frame_i)
    {
        _layer_preview.set_preview_size(_owner->_preview_size);

        _layer_preview_frame.set_child(_layer_preview);
        _layer_preview_frame.set_label_widget(nullptr);
        _layer_preview_aspect_frame.set_child(&_layer_preview_frame);
        _layer_preview_list_view.push_back(&_layer_preview_aspect_frame);

        _visible_locked_buttons_box.push_back(&_is_visible_toggle_button);
        _visible_locked_buttons_box.push_back(&_is_locked_toggle_button);
        set_visible(layer->is_visible);
        set_locked(layer->is_locked);

        for (auto* button : {&_is_visible_toggle_button, &_is_locked_toggle_button})
        {
            button->set_expand(false);
            button->set_align(GTK_ALIGN_CENTER);
        }

        _is_visible_toggle_button.connect_signal_toggled(on_is_visible_toggle_button_toggled, this);
        _is_locked_toggle_button.connect_signal_toggled(on_is_locked_toggle_button_toggled, this);

        for (auto* icon : {&_is_visible_icon, &_is_not_visible_icon, &_is_locked_icon, &_is_not_locked_icon})
            icon->set_size_request(icon->get_size());

        set_name(_layer->name);
        _layer_name_entry.set_hexpand(true);
        _layer_name_entry.set_vexpand(false);
        _layer_name_entry.set_has_frame(false);
        _layer_name_entry.connect_signal_activate(on_layer_name_entry_activated, this);

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

                        data.instance->_layer->blend_mode = mode;
                        data.instance->signal_layer_updated();

                    }, blend_mode_drop_down_data{pair.first, this}
            );
        }

        _blend_mode_drop_down.set_expand(false);
        _blend_mode_drop_down.set_valign(GTK_ALIGN_CENTER);
        _blend_mode_drop_down.set_halign(GTK_ALIGN_END);

        _opacity_scale.connect_signal_value_changed(on_opacity_scale_value_changed, this);
        _opacity_spin_button.connect_signal_value_changed(on_opacity_spin_button_value_changed, this);
        set_opacity(_layer->opacity);

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

        update();
    }

    LayerView::LayerRow::operator Widget*()
    {
        return &_outer_box;
    }

    void LayerView::LayerRow::update()
    {
        size_t layer_i = 0;
        for (auto* layer : active_state->layers)
        {
            if (layer == _layer)
                break;

            layer_i += 1;
        }

        std::stringstream tooltip;
        tooltip << "Layer #" << std::to_string(layer_i) << ": \"" << _layer_name_entry.get_text() << "\"";
        _layer_name_entry.set_tooltip_text(tooltip.str());
        _layer_preview.operator Widget*()->set_tooltip_text(tooltip.str());
    }

    void LayerView::LayerRow::set_layer(Layer* layer)
    {
        _layer = layer;
        _layer_preview.set_layer(layer);

        set_locked(_layer->is_locked);
        set_visible(_layer->is_visible);
        set_blend_mode(_layer->blend_mode);
        set_name(_layer->name);

        update();
    }

    void LayerView::LayerRow::set_blend_mode(BlendMode mode)
    {
        size_t blend_mode_i = 0;
        for (auto& pair : blend_mode_to_label)
        {
            if (pair.first == _layer->blend_mode)
                break;

            blend_mode_i += 1;
        }
        _blend_mode_drop_down.set_selected(blend_mode_i);
    }

    void LayerView::LayerRow::set_frame(size_t i)
    {
        _frame_i = i;
        _layer_preview.set_frame(i);
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

    void LayerView::LayerRow::signal_layer_updated()
    {
        state::canvas->update();
        state::frame_view->update();
    }

    void LayerView::LayerRow::on_is_visible_toggle_button_toggled(ToggleButton* button, LayerRow* instance)
    {
        auto b = button->get_active();
        instance->set_visible(b);

        instance->_layer->is_visible = b;
        instance->signal_layer_updated();
    }

    void LayerView::LayerRow::on_is_locked_toggle_button_toggled(ToggleButton* button, LayerRow* instance)
    {
        auto b = button->get_active();
        instance->set_locked(b);

        instance->_layer->is_locked = b;
        instance->signal_layer_updated();
    }

    void LayerView::LayerRow::on_layer_name_entry_activated(Entry* entry, LayerRow* instance)
    {
        instance->_layer->name = instance->_layer_name_entry.get_text();
        instance->signal_layer_updated();
    }

    void LayerView::LayerRow::on_opacity_scale_value_changed(Scale* scale, LayerRow* instance)
    {
        auto v = scale->get_value();
        instance->set_opacity(v);
        instance->_layer->opacity = v;
        instance->signal_layer_updated();
    }

    void LayerView::LayerRow::on_opacity_spin_button_value_changed(SpinButton* scale, LayerRow* instance)
    {
        auto v = scale->get_value();
        instance->set_opacity(v);
        instance->_layer->opacity = v;
        instance->signal_layer_updated();
    }

    // ###

    LayerView::LayerView()
    {
        for (auto* layer : active_state->layers)
            _layer_rows.emplace_back(this, layer, active_state->current_frame);

        for (auto& row : _layer_rows)
            _layer_rows_list_view.push_back(row);


        _layer_rows_list_view.set_show_separators(true);
        _layer_rows_list_view.get_selection_model()->connect_signal_selection_changed(on_layer_rows_list_view_selection_changed, this);
        _layer_rows_list_view.get_selection_model()->select(active_state->current_layer);

        _layer_rows_scrolled_window_spacer.set_size_request({0, state::margin_unit * 10});

        _layer_rows_scrolled_window_box.push_back(&_layer_rows_list_view);
        _layer_rows_scrolled_window_box.push_back(&_layer_rows_scrolled_window_spacer);
        _layer_rows_scrolled_window_spacer.set_expand(true);

        _layer_rows_scrolled_window.set_child(&_layer_rows_scrolled_window_box);
        _layer_rows_scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _layer_rows_scrolled_window.set_propagate_natural_height(true);
        _layer_rows_scrolled_window.set_expand(true);

        // control bar
        state::initialize_layer_view_actions();

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
        _preview_size_spin_button.connect_signal_value_changed(on_preview_size_spin_button_value_changed, this);
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
        create_section.add_action(layer_create_tooltip, "layer_view.layer_create");
        create_section.add_action(layer_delete_tooltip, "layer_view.layer_delete");
        create_section.add_action(layer_duplicate_tooltip, "layer_view.layer_duplicate");
        _menu.add_section("Create / Delete", &create_section);

        auto merge_section = MenuModel();
        merge_section.add_action(layer_merge_down_tooltip, "layer_view.layer_merge_down");
        merge_section.add_action(layer_flatten_all_tooltip, "layer_view.layer_flatten_all");
        _menu.add_section("Merge", &merge_section);

        auto other_section = MenuModel();
        other_section.add_action(layer_move_up_tooltip, "layer_view.layer_move_up");
        other_section.add_action(layer_move_down_tooltip, "layer_view.layer_move_down");
        other_section.add_action(set_layer_visible_tooltip, "layer_view.set_layer_visible");
        other_section.add_action(set_layer_locked_tooltip, "layer_view.set_layer_locked");

        _menu.add_section("Other", &other_section);

        _header_menu_button.set_child(&_header_menu_button_label);

        _header_menu_button_popover.refresh_widgets();
        _header_menu_button.set_popover(&_header_menu_button_popover);

        _main.set_homogeneous(false);
        _main.push_back(&_header_menu_button);
        _main.push_back(&_layer_rows_scrolled_window);
        _main.push_back(&_control_bar_box);
    }

    void LayerView::set_preview_size(size_t v)
    {
        _preview_size = v;

        for (auto& row : _layer_rows)
            row.set_preview_size(_preview_size);
    }

    size_t LayerView::get_preview_size() const
    {
        return _preview_size;
    }

    void LayerView::on_preview_size_spin_button_value_changed(SpinButton* scale, LayerView* instance)
    {
        instance->set_preview_size(scale->get_value());
    }

    LayerView::operator Widget*()
    {
        return &_main;
    }

    void LayerView::update()
    {
        for (auto& row : _layer_rows)
            row.update();
    }

    void LayerView::on_layer_rows_list_view_selection_changed(SelectionModel*, size_t first_item_position,
            size_t n_items_changed, LayerView* instance)
    {
        active_state->current_layer = first_item_position;
        state::canvas->update();
        state::frame_view->update();
    }
}

namespace mousetrap::state
{
    void swap_layers_at_position(size_t a, size_t b)
    {
        auto* a_layer = active_state->layers.at(a);
        auto* b_layer = active_state->layers.at(b);

        std::cout << a_layer->name << " " << b_layer->name << std::endl;

        auto a_backup = Layer(*a_layer);

        for (size_t i = 0; i < a_layer->frames.size(); ++i)
        {
            a_layer->frames.at(i) = b_layer->frames.at(i);
            b_layer->frames.at(i) = a_backup.frames.at(i);
        }

        std::cout << a_layer->name << " " << b_layer->name << std::endl;
    }

    void initialize_layer_view_actions()
    {
        using namespace state::actions;

        // move up
        {
            static std::deque<std::pair<size_t, size_t>> move_up_undo_data;
            static std::deque<std::pair<size_t, size_t>> move_up_redo_data;
            auto move_up_do = []()
            {
                if (active_state->current_layer > 0)
                {
                    auto before_i = active_state->current_layer;
                    auto after_i = active_state->current_layer - 1;

                    move_up_undo_data.emplace_back(before_i, after_i);
                    state::swap_layers_at_position(before_i, after_i);
                }

                state::layer_view->update();
                state::frame_view->update();
                state::canvas->update();
                state::animation_preview->update();
            };

            auto move_up_undo = []()
            {
                auto to_swap = move_up_undo_data.back();
                state::swap_layers_at_position(to_swap.second, to_swap.first);
                move_up_undo_data.pop_back();
                move_up_redo_data.emplace_back(to_swap.second, to_swap.first);

                state::layer_view->update();
                state::frame_view->update();
                state::canvas->update();
                state::animation_preview->update();
            };

            auto move_up_redo = [](){
                auto to_swap = move_up_redo_data.back();
                state::swap_layers_at_position(to_swap.first, to_swap.second);
                move_up_redo_data.pop_back();
                move_up_undo_data.emplace_back(to_swap.first, to_swap.second);

                state::layer_view->update();
                state::frame_view->update();
                state::canvas->update();
                state::animation_preview->update();
            };

            layer_view_layer_move_up.set_function(move_up_do, move_up_undo, move_up_do);
        }

        for (auto* action : {
                &layer_view_layer_move_up,
                &layer_view_layer_move_down,
                &layer_view_layer_new_above_current,
                &layer_view_layer_new_below_current,
                &layer_view_layer_duplicate,
                &layer_view_layer_delete,
                &layer_view_layer_merge_down,
                &layer_view_layer_flatten_all,
                &layer_view_set_layer_visible,
                &layer_view_set_layer_locked
        })
            state::add_shortcut_action(*action);
    }
}
