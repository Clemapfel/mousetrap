//
// Created by clem on 1/24/23.
//

#include <app/frame_view.hpp>

namespace mousetrap
{
    FrameView::FramePreview::FramePreview(FrameView* owner)
        : _owner(owner), _aspect_frame(active_state->get_layer_resolution().x / active_state->get_layer_resolution().y)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
        _aspect_frame.set_child(&_area);

        set_preview_size(owner->_preview_size);
        _inbetween_label_overlay.set_child(&_aspect_frame);
        _inbetween_label_overlay.add_overlay(&_inbetween_label);
    }

    FrameView::FramePreview::~FramePreview()
    {
        delete _transparency_tiling_shape;
        delete _layer_shape;
    }

    void FrameView::FramePreview::set_preview_size(size_t x)
    {
        float width = (active_state->get_layer_resolution().x / active_state->get_layer_resolution().y) * x;
        _area.set_size_request({width, x});

        _area.set_expand(true);
        _area.set_align(GTK_ALIGN_CENTER);
        if (active_state->get_layer_resolution().x > active_state->get_layer_resolution().y)
            _area.set_hexpand(false);
        else if (active_state->get_layer_resolution().y > active_state->get_layer_resolution().x)
            _area.set_vexpand(false);
    }

    void FrameView::FramePreview::set_is_inbetween(bool b)
    {
        _is_inbetween = b;

        if (_is_inbetween)
            _inbetween_label.set_visible(true);
        else
            _inbetween_label.set_visible(false);
    }

    void FrameView::FramePreview::set_opacity(float x)
    {
        if (_layer_shape != nullptr)
            _layer_shape->set_color(RGBA(1, 1, 1, x));

        _area.queue_render();
    }

    void FrameView::FramePreview::set_visible(bool b)
    {
        _area.set_opacity(b ? 1 : state::settings_file->get_value_as<float>("layer_view", "hidden_layer_opacity"));
        _area.queue_render();
    }

    void FrameView::FramePreview::set_selected(bool b)
    {
        // noop
    }

    void FrameView::FramePreview::set_texture(const Texture* texture)
    {
        _texture = texture;
        if (_layer_shape != nullptr)
            _layer_shape->set_texture(_texture);
    }

    FrameView::FramePreview::operator Widget*()
    {
        return &_inbetween_label_overlay;
    }

    void FrameView::FramePreview::on_realize(Widget* widget, FramePreview* instance)
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


        if (instance->_texture != nullptr)
            instance->_layer_shape->set_texture(instance->_texture);

        area->clear_render_tasks();

        auto transparency_task = RenderTask(instance->_transparency_tiling_shape, _transparency_tiling_shader);
        transparency_task.register_vec2("_canvas_size", &instance->_canvas_size);

        area->add_render_task(transparency_task);
        area->add_render_task(instance->_layer_shape);

        area->queue_render();
    }

    void FrameView::FramePreview::on_resize(GLArea* area, int w, int h, FramePreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_area.queue_render();
    }

    FrameView::FrameColumn::PreviewElement::PreviewElement(FrameView* owner)
        :  _preview(owner),
          _wrapper(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE),
          _wrapper_box(GTK_ORIENTATION_HORIZONTAL)
    {
        _frame_label.set_visible(false);
        _layer_label.set_margin_horizontal(state::margin_unit);

        _frame.set_label_align(0.5);
        _frame.set_child(_preview);
        _frame.set_label_widget(&_frame_label);

        _layer_label_spacer.set_size_request({state::margin_unit / 10 * 4, 0});
        _layer_label_spacer.set_margin_horizontal((state::margin_unit / 10 * 4) / 2);
        _wrapper_box.push_back(&_layer_label);
        _wrapper_box.push_back(&_layer_label_spacer);
        _wrapper_box.push_back(&_frame);
        _wrapper.push_back(&_wrapper_box);
    }

    void FrameView::FrameColumn::PreviewElement::set_layer_frame_index(size_t layer_i, size_t frame_i)
    {
        _frame_label.set_text(std::string("<tt>") + (frame_i < 10 ? "00" : (frame_i < 100 ? "0" : "")) + std::to_string(frame_i) + "</tt>");
        _layer_label.set_text(std::string("<tt><span size=\"120%\">") + (frame_i < 10 ? "0" : "") + std::to_string(layer_i) + "</span></tt>");

        _preview.set_texture(active_state->get_frame(layer_i, frame_i)->get_texture());
        _layer_label_spacer.set_visible(frame_i == 0);
        _layer_label.set_visible(frame_i == 0);
    }

    FrameView::FrameColumn::PreviewElement::operator Widget*()
    {
        return &_wrapper;
    }

    void FrameView::FrameColumn::PreviewElement::set_preview_size(size_t x)
    {
        _preview.set_preview_size(x);
    }

    void FrameView::FrameColumn::PreviewElement::set_opacity(float x)
    {
        _preview.set_opacity(x);
    }

    void FrameView::FrameColumn::PreviewElement::set_visible(bool x)
    {
        _preview.set_visible(x);
    }

    void FrameView::FrameColumn::PreviewElement::set_is_inbetween(bool b)
    {
        _preview.set_is_inbetween(b);
    }

    void FrameView::FrameColumn::PreviewElement::set_selected(bool b)
    {
        _frame_label.set_visible(b);
        _preview.set_selected(b);
    }

    FrameView::FrameColumn::FrameColumn::FrameColumn(FrameView* owner, size_t frame_i)
        : _owner(owner), _frame_i(frame_i)
    {
        set_n_layers(active_state->get_n_layers());
        _list_view.set_show_separators(true);
        _list_view.get_selection_model()->connect_signal_selection_changed([](SelectionModel*, size_t i, size_t n_items, FrameColumn* instance){
            active_state->set_current_layer_and_frame(instance->_list_view.get_n_items() - i - 1, instance->_frame_i);
        }, this);
    }

    FrameView::FrameColumn::operator Widget*()
    {
        return &_list_view;
    }

    void FrameView::FrameColumn::set_n_layers(size_t n)
    {
        _list_view.clear();

        while (_preview_elements.size() < n)
            _preview_elements.emplace_back(_owner);

        for (size_t i = 0; i < _preview_elements.size(); ++i)
        {
            auto& element = _preview_elements.at(i);
            element.set_layer_frame_index(i, _frame_i);
            _list_view.push_front(element);

            auto* layer = active_state->get_layer(i);
            element.set_visible(layer->get_is_visible());
            element.set_opacity(layer->get_opacity());
            element.set_is_inbetween(not layer->get_frame(_frame_i)->get_is_keyframe());
        }
    }

    void FrameView::FrameColumn::set_frame(size_t i)
    {
        _frame_i = i;

        for (size_t i = 0; i < _preview_elements.size(); ++i)
        {
            auto& element = _preview_elements.at(i);
            element.set_layer_frame_index(i, _frame_i);
        }
    }

    void FrameView::FrameColumn::set_preview_size(size_t size)
    {
        for (auto& preview : _preview_elements)
            preview.set_preview_size(size);
    }

    void FrameView::FrameColumn::set_is_inbetween(bool b)
    {
        for (auto& preview : _preview_elements)
            preview.set_is_inbetween(b);
    }

    void FrameView::FrameColumn::select_layer(size_t i)
    {
        for (size_t preview_i = 0; preview_i < _preview_elements.size(); ++preview_i)
            _preview_elements.at(preview_i).set_selected(preview_i == i);

        _list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _list_view.get_selection_model()->select(_list_view.get_n_items() - i - 1);
        _list_view.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    void FrameView::FrameColumn::set_layer_opacity(size_t i, float v)
    {
        _preview_elements.at(i).set_opacity(v);
    }

    void FrameView::FrameColumn::set_layer_visible(size_t i, bool v)
    {
        _preview_elements.at(i).set_visible(v);
    }

    FrameView::ControlBar::ControlBar(FrameView* owner)
            : _owner(owner), _fps_label("fps:")
    {
        // ACTIONS

        using namespace state::actions;

        frame_view_jump_to_start.set_function([](){
           active_state->set_current_layer_and_frame(active_state->get_current_frame_index(), 0);
        });

        frame_view_jump_to_end.set_function([](){
            active_state->set_current_layer_and_frame(active_state->get_current_frame_index(), active_state->get_n_frames() - 1);
        });

        frame_view_go_to_previous_frame.set_function([]()
        {
            auto current = active_state->get_current_frame_index();
            if (current == 0)
                active_state->set_current_layer_and_frame(active_state->get_current_frame_index(), active_state->get_n_frames() - 1);
            else
                active_state->set_current_layer_and_frame(active_state->get_current_frame_index(), current - 1);
        });

        frame_view_go_to_next_frame.set_function([]()
        {
            auto current = active_state->get_current_frame_index();
            if (current == active_state->get_n_frames() - 1)
                active_state->set_current_layer_and_frame(active_state->get_current_frame_index(), 0);
            else
                active_state->set_current_layer_and_frame(active_state->get_current_frame_index(), current + 1);
        });

        frame_view_toggle_onionskin_visible.set_function([](){
           active_state->set_onionskin_visible(not active_state->get_onionskin_visible());
        });

        frame_view_increase_n_onionskin_layers.set_function([](){
            auto current = active_state->get_n_onionskin_layers();
            active_state->set_n_onionskin_layers(current + 1);
        });

        frame_view_increase_n_onionskin_layers.set_function([](){
            auto current = active_state->get_n_onionskin_layers();
            if (current > 0)
                active_state->set_n_onionskin_layers(current - 1);
        });

        frame_view_play_pause.set_function([](){

        });

        for (auto* action : {
                &frame_view_toggle_onionskin_visible,
                &frame_view_increase_n_onionskin_layers,
                &frame_view_decrease_n_onionskin_layers,
                &frame_view_jump_to_start,
                &frame_view_jump_to_end,
                &frame_view_go_to_previous_frame,
                &frame_view_go_to_next_frame,
                &frame_view_play_pause,
                &frame_view_frame_move_right,
                &frame_view_frame_move_left,
                &frame_view_frame_new_left_of_current,
                &frame_view_frame_new_right_of_current,
                &frame_view_frame_delete,
                &frame_view_frame_make_keyframe_inbetween
        })
            state::add_shortcut_action(*action);

        // GUI
        _toggle_onionskin_visible_button.set_active(active_state->get_onionskin_visible());
        _toggle_onionskin_visible_button.set_child(&_toggle_onionskin_visible_icon);
        _toggle_onionskin_visible_button.connect_signal_toggled([](ToggleButton*, ControlBar* instance){
            frame_view_toggle_onionskin_visible.activate();
        }, this);

        _onionskin_n_layers_spin_button.set_value(active_state->get_n_onionskin_layers());
        _onionskin_n_layers_spin_button.connect_signal_value_changed([](SpinButton* scale, ControlBar* instance){
            active_state->set_n_onionskin_layers(scale->get_value());
        }, this);

        _jump_to_start_button.set_child(&_jump_to_start_icon);
        _jump_to_start_button.set_action(frame_view_jump_to_start);

        _jump_to_end_button.set_child(&_jump_to_end_icon);
        _jump_to_end_button.set_action(frame_view_jump_to_end);

        _go_to_previous_frame_button.set_child(&_go_to_previous_frame_icon);
        _go_to_previous_frame_button.set_action(frame_view_go_to_previous_frame);

        _go_to_next_frame_button.set_child(&_go_to_next_frame_icon);
        _go_to_next_frame_button.set_action(frame_view_go_to_next_frame);

        _play_pause_button.set_child(active_state->get_playback_active() ? &_pause_icon : &_play_icon);
        _play_pause_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            active_state->set_playback_active(button->get_active());
        }, this);

        _frame_move_right_button.set_child(&_frame_move_right_icon);
        _frame_move_right_button.set_action(frame_view_frame_move_right);

        _frame_move_left_button.set_child(&_frame_move_left_icon);
        _frame_move_left_button.set_action(frame_view_frame_move_right);

        _frame_new_left_of_current_button.set_child(&_frame_new_left_of_current_icon);
        _frame_new_left_of_current_button.set_action(frame_view_frame_new_left_of_current);

        _frame_new_right_of_current_button.set_child(&_frame_new_right_of_current_icon);
        _frame_new_right_of_current_button.set_action(frame_view_frame_new_right_of_current);

        _frame_delete_button.set_child(&_frame_delete_icon);
        _frame_delete_button.set_action(frame_view_frame_delete);

        bool is_keyframe = active_state->get_current_frame()->get_is_keyframe();
        _frame_make_keyframe_button.set_child(is_keyframe ? &_frame_is_not_keyframe_icon : &_frame_is_keyframe_icon);
        _frame_make_keyframe_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_frame_make_keyframe_inbetween.activate();
        }, this);

        // Tooltips

        auto toggle_onionskin_visible_tooltip = state::tooltips_file->get_value("frame_view", "toggle_onionskin_visible");
        _toggle_onionskin_visible_button.set_tooltip_text(toggle_onionskin_visible_tooltip);

        auto set_onionskin_n_layers_tooltip = state::tooltips_file->get_value("frame_view", "set_onionskin_n_layers");
        _onionskin_n_layers_spin_button.set_tooltip_text(set_onionskin_n_layers_tooltip);

        auto jump_to_start_tooltip = state::tooltips_file->get_value("frame_view", "jump_to_start");
        _jump_to_start_button.set_tooltip_text(jump_to_start_tooltip);

        auto jump_to_end_tooltip = state::tooltips_file->get_value("frame_view", "jump_to_end");
        _jump_to_end_button.set_tooltip_text(jump_to_end_tooltip);

        auto go_to_previous_frame_tooltip = state::tooltips_file->get_value("frame_view", "go_to_previous_frame");
        _go_to_previous_frame_button.set_tooltip_text(go_to_previous_frame_tooltip);

        auto go_to_next_frame_tooltip = state::tooltips_file->get_value("frame_view", "go_to_next_frame");
        _go_to_next_frame_button.set_tooltip_text(go_to_next_frame_tooltip);

        auto play_pause_tooltip = state::tooltips_file->get_value("frame_view", "play_pause");
        _play_pause_button.set_tooltip_text(play_pause_tooltip);

        auto frame_move_left_tooltip = state::tooltips_file->get_value("frame_view", "frame_move_left");
        _frame_move_left_button.set_tooltip_text(frame_move_left_tooltip);

        auto frame_move_right_tooltip = state::tooltips_file->get_value("frame_view", "frame_move_right");
        _frame_move_right_button.set_tooltip_text(frame_move_right_tooltip);

        auto frame_new_left_of_current_tooltip = state::tooltips_file->get_value("frame_view", "frame_new_left_of_current");
        _frame_new_left_of_current_button.set_tooltip_text(frame_new_left_of_current_tooltip);

        auto frame_new_right_of_current_tooltip = state::tooltips_file->get_value("frame_view", "frame_new_right_of_current");
        _frame_new_right_of_current_button.set_tooltip_text(frame_new_right_of_current_tooltip);

        auto frame_delete_tooltip = state::tooltips_file->get_value("frame_view", "frame_delete");
        _frame_delete_button.set_tooltip_text(frame_delete_tooltip);

        auto frame_make_keyframe_inbetween_tooltip = state::tooltips_file->get_value("frame_view", "frame_make_keyframe_inbetween");
        _frame_make_keyframe_button.set_tooltip_text(frame_make_keyframe_inbetween_tooltip);

        _tooltip.create_from("frame_view", state::tooltips_file, state::keybindings_file);

        // Menu

        _preview_size_spin_button.set_margin_start(state::margin_unit);
        _preview_size_spin_button.set_halign(GTK_ALIGN_END);
        _preview_size_spin_button.set_value(_preview_size);
        _preview_size_spin_button.connect_signal_value_changed([](SpinButton* scale, ControlBar* instance) {
            state::frame_view->set_preview_size(scale->get_value());
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

        auto playback_section = MenuModel();
        playback_section.add_action(jump_to_start_tooltip, frame_view_jump_to_start.get_id());
        playback_section.add_action(jump_to_end_tooltip, frame_view_jump_to_end.get_id());
        playback_section.add_action(go_to_previous_frame_tooltip, frame_view_go_to_previous_frame.get_id());
        playback_section.add_action(go_to_next_frame_tooltip, frame_view_go_to_next_frame.get_id());
        playback_section.add_action(play_pause_tooltip, frame_view_play_pause.get_id());
        _menu.add_section("Playback", &playback_section);

        auto create_section = MenuModel();
        create_section.add_action(frame_new_left_of_current_tooltip, frame_view_frame_new_left_of_current.get_id());
        create_section.add_action(frame_new_right_of_current_tooltip, frame_view_frame_new_right_of_current.get_id());
        create_section.add_action(frame_delete_tooltip, frame_view_frame_delete.get_id());
        _menu.add_section("Create / Delete", &create_section);

        auto position_section = MenuModel();
        position_section.add_action(frame_move_left_tooltip, frame_view_frame_move_left.get_id());
        position_section.add_action(frame_move_right_tooltip, frame_view_frame_move_right.get_id());
        _menu.add_section("Other", &position_section);

        auto other_section = MenuModel();
        other_section.add_action(frame_make_keyframe_inbetween_tooltip, frame_view_frame_make_keyframe_inbetween.get_id());
        other_section.add_action(toggle_onionskin_visible_tooltip, frame_view_toggle_onionskin_visible.get_id());
        _menu.add_section("Other", &other_section);

        _popover_menu.refresh_widgets();
        _menu_button.set_child(&_menu_button_label);
        _menu_button.set_tooltip_widget(_tooltip);
        _menu_button.set_popover(&_popover_menu);

        // Layout

        for (auto& image : {&_toggle_onionskin_visible_icon, &_jump_to_start_icon, &_jump_to_end_icon, &_go_to_previous_frame_icon, &_go_to_next_frame_icon, &_play_icon, &_pause_icon, &_frame_move_left_icon, &_frame_move_right_icon, &_frame_new_left_of_current_icon, &_frame_new_right_of_current_icon, &_frame_delete_icon, &_frame_is_keyframe_icon, &_frame_is_not_keyframe_icon})
            image->set_size_request(image->get_size());

        auto button_width = _play_pause_button.get_preferred_size().natural_size.x;
        _menu_button.set_size_request({4 * button_width, 0});

        {
            auto separator = SeparatorLine();
            separator.set_size_request({button_width, 0});
            separator.set_hexpand(false);
            _button_box.push_back(&separator);
        }

        _button_box.push_back(&_toggle_onionskin_visible_button);
        _button_box.push_back(&_onionskin_n_layers_spin_button);

        {
            auto separator = SeparatorLine();
            separator.set_size_request({button_width, 0});
            separator.set_hexpand(false);
            _button_box.push_back(&separator);
        }

        _button_box.push_back(&_frame_move_left_button);
        _button_box.push_back(&_frame_new_left_of_current_button);
        _button_box.push_back(&_frame_delete_button);
        _button_box.push_back(&_frame_make_keyframe_button);
        _button_box.push_back(&_frame_new_right_of_current_button);
        _button_box.push_back(&_frame_move_right_button);

        {
            auto separator = SeparatorLine();
            separator.set_size_request({button_width, 0});
            separator.set_hexpand(false);
            _button_box.push_back(&separator);
        }

        _button_box.push_back(&_jump_to_start_button);
        _button_box.push_back(&_go_to_previous_frame_button);
        _button_box.push_back(&_play_pause_button);
        _button_box.push_back(&_go_to_next_frame_button);
        _button_box.push_back(&_jump_to_end_button);

        {
            auto separator = SeparatorLine();
            separator.set_size_request({button_width * 0.5, 0});
            separator.set_hexpand(false);
            _button_box.push_back(&separator);
        }

        _fps_label_underlay.set_size_request({button_width, 0});
        _fps_label_underlay.set_hexpand(false);

        _fps_label_overlay.set_child(&_fps_label_underlay);
        _fps_label_overlay.add_overlay(&_fps_label);

        _button_box.push_back(&_fps_label_overlay);

        _fps_spin_button.set_tooltip_text(state::tooltips_file->get_value("frame_view", "set_fps"));
        _fps_spin_button.set_value(active_state->get_fps());
        _fps_spin_button.connect_signal_value_changed([](SpinButton* button, ControlBar* instance){
            active_state->set_fps(button->get_value());
        }, this);

        _button_box.push_back(&_fps_spin_button);

        {
            auto separator = SeparatorLine();
            separator.set_size_request({button_width, 0});
            separator.set_hexpand(true);
            _button_box.push_back(&separator);
        }

        _scrolled_window.set_child(&_button_box);
        _scrolled_window.set_hexpand(true);

        _main.push_back(&_menu_button);
        _main.push_back(&_scrolled_window);
    }

    void FrameView::ControlBar::set_onionskin_visible(bool b)
    {
        _toggle_onionskin_visible_button.set_signal_toggled_blocked(true);
        _toggle_onionskin_visible_button.set_active(b);
        _toggle_onionskin_visible_button.set_signal_toggled_blocked(false);
    }

    void FrameView::ControlBar::set_n_onionskin_layers(size_t x)
    {
        _onionskin_n_layers_spin_button.set_signal_value_changed_blocked(true);
        _onionskin_n_layers_spin_button.set_value(x);
        _onionskin_n_layers_spin_button.set_signal_value_changed_blocked(false);
    }

    void FrameView::ControlBar::set_playback_active(bool b)
    {
        _play_pause_button.set_signal_toggled_blocked(true);
        _play_pause_button.set_active(b);
        _play_pause_button.set_signal_toggled_blocked(false);

        if (b)
            _play_pause_button.set_child(&_pause_icon);
        else
            _play_pause_button.set_child(&_play_icon);
    }

    void FrameView::ControlBar::set_fps(float fps)
    {
        _fps_spin_button.set_signal_value_changed_blocked(true);
        _fps_spin_button.set_value(fps);
        _fps_spin_button.set_signal_value_changed_blocked(false);
    }

    FrameView::ControlBar::operator Widget*() {
        return &_main;
    }

    FrameView::FrameView()
    {
        on_layer_count_changed();

        _frame_column_list_view.get_selection_model()->connect_signal_selection_changed([](SelectionModel*, size_t i, size_t n_items, FrameView* instance){
            active_state->set_current_layer_and_frame(instance->_selected_layer_i, i);
        }, this);

        _frame_column_list_view.set_expand(false);
        _frame_colum_list_view_window_spacer.set_expand(true);
        _frame_column_list_view_window.set_expand(true);

        _frame_colum_list_view_window_box.push_back(&_frame_column_list_view);
        _frame_colum_list_view_window_box.push_back(&_frame_colum_list_view_window_spacer);

        _frame_column_list_view_window.set_child(&_frame_colum_list_view_window_box);
        _main.push_back(_control_bar);
        _main.push_back(&_frame_column_list_view_window);
    }

    FrameView::operator Widget*()
    {
        return &_main;
    }

    void FrameView::set_selection(size_t layer_i, size_t frame_i)
    {
        _selected_layer_i = layer_i;
        _selected_frame_i = frame_i;

        for (auto& column : _frame_columns)
            column.select_layer(layer_i);

        _frame_column_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _frame_column_list_view.get_selection_model()->select(frame_i);
        _frame_column_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    void FrameView::set_preview_size(size_t x)
    {
        _preview_size = x;

        for (auto& column : _frame_columns)
            column.set_preview_size(_preview_size);
    }

    void FrameView::on_layer_count_changed()
    {
        _frame_column_list_view.clear();
        _frame_columns.clear();

        for (size_t i = 0; i < active_state->get_n_frames(); ++i)
        {
            auto& column = _frame_columns.emplace_back(this, i);
            _frame_column_list_view.push_back(column);
        }
    }

    void FrameView::on_layer_frame_selection_changed()
    {
        set_selection(active_state->get_current_layer_index(), active_state->get_current_frame_index());

        using namespace state::actions;
        auto frame_i = active_state->get_current_frame_index();
        frame_view_jump_to_start.set_enabled(frame_i > 0);
        frame_view_jump_to_end.set_enabled(frame_i < active_state->get_n_frames() - 1);
    }

    void FrameView::on_layer_image_updated()
    {
        for (size_t i = 0; i < active_state->get_n_frames(); ++i)
            _frame_columns.at(i).set_frame(i);
    }

    void FrameView::on_layer_properties_changed()
    {
        for (size_t frame_i = 0; frame_i < active_state->get_n_frames(); ++frame_i)
        {
            auto& column = _frame_columns.at(frame_i);
            for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
            {
                auto* layer = active_state->get_layer(layer_i);
                column.set_layer_visible(layer_i, layer->get_is_visible());
                column.set_layer_opacity(layer_i, layer->get_opacity());
                column.set_is_inbetween(not layer->get_frame(frame_i)->get_is_keyframe());
            }
        }
    }

    void FrameView::on_playback_toggled()
    {
        _control_bar.set_playback_active(active_state->get_playback_active());
    }

    void FrameView::on_playback_fps_changed()
    {
        _control_bar.set_fps(active_state->get_fps());
    }

    void FrameView::on_layer_resolution_changed()
    {
        // TODO
    }

    void FrameView::on_onionskin_visibility_toggled()
    {
        _control_bar.set_onionskin_visible(active_state->get_onionskin_visible());
    }

    void FrameView::on_onionskin_layer_count_changed()
    {
        _control_bar.set_n_onionskin_layers(active_state->get_n_onionskin_layers());
    }


}
