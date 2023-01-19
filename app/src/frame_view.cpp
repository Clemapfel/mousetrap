#include <app/frame_view.hpp>
#include <app/layer_view.hpp>
#include <app/animation_preview.hpp>

namespace mousetrap
{
    FrameView::FramePreview::FramePreview(FrameView* owner, Layer* layer, size_t frame_i)
        : _owner(owner), _layer(layer), _frame_i(frame_i), _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y))
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
        _aspect_frame.set_child(&_area);

        set_preview_size(owner->_preview_size);
        _inbetween_label_overlay.set_child(&_aspect_frame);
        _inbetween_label_overlay.add_overlay(&_inbetween_label);

        set_is_inbetween(false);
    }

    void FrameView::FramePreview::set_preview_size(size_t x)
    {
        float width = (state::layer_resolution.x / state::layer_resolution.y) * x;
        _area.set_size_request({width, x});

        _area.set_expand(true);
        _area.set_align(GTK_ALIGN_CENTER);
        if (state::layer_resolution.x > state::layer_resolution.y)
            _area.set_hexpand(false);
        else if (state::layer_resolution.y > state::layer_resolution.x)
            _area.set_vexpand(false);
    }

    void FrameView::FramePreview::set_is_inbetween(bool b)
    {
        _is_inbetween = b;

        if (_is_inbetween)
        {
            _area.set_opacity(0.25);
            _inbetween_label.set_visible(true);
        }
        else
        {
            _area.set_opacity(1);
            _inbetween_label.set_visible(false);
        }
    }

    void FrameView::FramePreview::set_layer(Layer* layer)
    {
        _layer = layer;
        update();
    }

    void FrameView::FramePreview::set_frame(size_t i)
    {
        _frame_i = i;
        update();
    }

    void FrameView::FramePreview::update()
    {
        if (not _area.get_is_realized())
            return;

        _layer_shape->set_texture(_layer->frames.at(_frame_i).texture);
        _layer_shape->set_color(RGBA(1, 1, 1, _layer->opacity));
        set_preview_size(_owner->_preview_size);
        _area.queue_render();
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

        area->clear_render_tasks();

        auto transparency_task = RenderTask(instance->_transparency_tiling_shape, _transparency_tiling_shader);
        transparency_task.register_vec2("_canvas_size", &instance->_canvas_size);

        area->add_render_task(transparency_task);
        area->add_render_task(instance->_layer_shape);

        instance->set_frame(instance->_frame_i);
        instance->set_layer(instance->_layer);

        area->queue_render();
    }

    void FrameView::FramePreview::on_resize(GLArea* area, int w, int h, FramePreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_area.queue_render();
    }

    // ###

    FrameView::FrameColumn::FrameColumn(FrameView* owner, size_t frame_i)
            : _owner(owner), _frame_i(frame_i)
    {
        _list_view.set_show_separators(true);

        for (size_t layer_i = 0; layer_i <state::layers.size() ; ++layer_i)
        {
            auto* layer = state::layers.at(layer_i);

            auto* element = _preview_elements.emplace_back(new PreviewElement{
                    FramePreview(owner, layer, _frame_i),
                    ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE),
                    Frame(),
                    Label(std::string("<tt>") + (_frame_i < 10 ? "00" : (_frame_i < 100 ? "0" : "")) + std::to_string(_frame_i) + "</tt>"),
                    Label(std::string("<tt><span size=\"120%\">") + (_frame_i < 10 ? "0" : "") + std::to_string(layer_i) + "</span></tt>"),
                    SeparatorLine(),
                    Box(GTK_ORIENTATION_HORIZONTAL)
            });

            element->frame_label.set_visible(false);
            element->layer_label.set_margin_horizontal(state::margin_unit);

            element->frame.set_label_align(0.5);
            element->frame.set_child(element->preview);
            element->frame.set_label_widget(&element->frame_label);

            element->layer_label_spacer.set_size_request({state::margin_unit / 10 * 4, 0});
            element->layer_label_spacer.set_margin_horizontal((state::margin_unit / 10 * 4) / 2);
            element->wrapper_box.push_back(&element->layer_label);
            element->wrapper_box.push_back(&element->layer_label_spacer);
            element->wrapper_box.push_back(&element->frame);
            element->wrapper.push_back(&element->wrapper_box);
            _list_view.push_back(&element->wrapper);
        }

        _list_view.get_selection_model()->connect_signal_selection_changed(on_selection_changed, this);
        set_is_first_frame(_frame_i == 0);
    }

    FrameView::FrameColumn::~FrameColumn()
    {
        for (auto* preview : _preview_elements)
            delete preview;
    }

    void FrameView::FrameColumn::set_frame(size_t i)
    {
        _frame_i = i;

        for (auto* e : _preview_elements)
            e->preview.set_frame(i);
    }

    void FrameView::FrameColumn::select_layer(size_t layer_i)
    {
        _list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _list_view.get_selection_model()->select(layer_i);
        _list_view.get_selection_model()->set_signal_selection_changed_blocked(false);

        for (size_t i = 0; i < _preview_elements.size(); ++i)
            _preview_elements.at(i)->frame_label.set_visible(i == layer_i);
    }

    void FrameView::FrameColumn::set_is_first_frame(bool b)
    {
        for (auto& preview : _preview_elements)
        {
            preview->layer_label_spacer.set_visible(b);
            preview->layer_label.set_visible(b);
        }
    }

    void FrameView::FrameColumn::set_preview_size(size_t x)
    {
        for (auto* preview : _preview_elements)
            preview->preview.set_preview_size(x);
    }

    void FrameView::FrameColumn::on_selection_changed(SelectionModel*, size_t i, size_t n_items,
            FrameColumn* instance)
    {
        instance->_owner->set_selection(i, instance->_frame_i);
        state::layer_view->update();
        state::animation_preview->update();
    }

    FrameView::FrameColumn::operator Widget*()
    {
        return &_list_view;
    }

    void FrameView::FrameColumn::update()
    {
        for (auto* e : _preview_elements)
            e->preview.update();
    }

    // ###

    FrameView::ControlBar::ControlBar(FrameView* owner)
            : _owner(owner)
    {
        // ACTIONS

        using namespace state::actions;

        frame_view_toggle_onionskin_visible.set_function([instance = this]() {
            instance->on_toggle_onionskin_visible();
        });

        frame_view_increase_n_onionskin_layers.set_function([instance = this]() -> void {
            auto current = state::onionskin_n_layers;
            instance->set_n_onionskin_layers(current + 1);
        });

        frame_view_decrease_n_onionskin_layers.set_function([instance = this]() -> void {
            auto current = state::onionskin_n_layers;
            instance->set_n_onionskin_layers(current - 1);
        });

        frame_view_jump_to_start.set_function([instance = this]() {
            instance->on_jump_to_start();
        });

        frame_view_jump_to_end.set_function([instance = this]() {
            instance->on_jump_to_end();
        });

        frame_view_go_to_previous_frame.set_function([instance = this]() {
            instance->on_go_to_previous_frame();
        });

        frame_view_go_to_next_frame.set_function([instance = this]() {
            instance->on_go_to_next_frame();
        });

        frame_view_play_pause.set_function([instance = this]() {
            instance->on_play_pause();
        });

        frame_view_frame_move_right.set_function([instance = this]() {
            instance->on_frame_move_right();
        });

        frame_view_frame_move_left.set_function([instance = this]() {
            instance->on_frame_move_left();
        });

        frame_view_frame_new_left_of_current.set_function([instance = this]() {
            instance->on_frame_new_left_of_current();
        });

        frame_view_frame_new_right_of_current.set_function([instance = this]() {
            instance->on_frame_new_right_of_current();
        });

        frame_view_frame_delete.set_function([instance = this]() {
            instance->on_frame_delete();
        });

        frame_view_frame_make_keyframe_inbetween.set_function([instance = this]() {
            instance->on_frame_make_keyframe_inbetween();
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
        _toggle_onionskin_visible_button.set_active(state::onionskin_visible);
        _toggle_onionskin_visible_button.set_child(&_toggle_onionskin_visible_icon);
        _toggle_onionskin_visible_button.connect_signal_toggled([](ToggleButton*, ControlBar* instance){
            frame_view_toggle_onionskin_visible.activate();
        }, this);

        _onionskin_n_layers_spin_button.set_value(state::onionskin_n_layers);
        _onionskin_n_layers_spin_button.connect_signal_value_changed(on_onionskin_spin_button_value_changed, this);

        _jump_to_start_button.set_child(&_jump_to_start_icon);
        _jump_to_start_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_jump_to_start.activate();
        }, this);

        _jump_to_end_button.set_child(&_jump_to_end_icon);
        _jump_to_end_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_jump_to_end.activate();
        }, this);

        _go_to_previous_frame_button.set_child(&_go_to_previous_frame_icon);
        _go_to_previous_frame_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_go_to_previous_frame.activate();
        }, this);

        _go_to_next_frame_button.set_child(&_go_to_next_frame_icon);
        _go_to_next_frame_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_go_to_next_frame.activate();
        }, this);

        _play_pause_button.set_child(state::playback_active ? &_pause_icon : &_play_icon);
        _play_pause_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_play_pause.activate();
        }, this);

        _frame_move_right_button.set_child(&_frame_move_right_icon);
        _frame_move_right_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_frame_move_right.activate();
        }, this);

        _frame_move_left_button.set_child(&_frame_move_left_icon);
        _frame_move_left_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_frame_move_left.activate();
        }, this);

        _frame_new_left_of_current_button.set_child(&_frame_new_left_of_current_icon);
        _frame_new_left_of_current_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_frame_new_left_of_current.activate();
        }, this);

        _frame_new_right_of_current_button.set_child(&_frame_new_right_of_current_icon);
        _frame_new_right_of_current_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_frame_new_right_of_current.activate();
        }, this);

        _frame_delete_button.set_child(&_frame_delete_icon);
        _frame_delete_button.connect_signal_clicked([](Button*, ControlBar* instance){
            frame_view_frame_delete.activate();
        }, this);

        bool is_keyframe = state::layers.at(state::current_layer)->frames.at(state::current_frame).is_keyframe;
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

        auto playback_section = MenuModel();
        playback_section.add_action(jump_to_start_tooltip, frame_view_jump_to_start.get_id());
        playback_section.add_action(jump_to_end_tooltip, frame_view_jump_to_end.get_id());
        playback_section.add_action(go_to_previous_frame_tooltip, frame_view_go_to_previous_frame.get_id());
        playback_section.add_action(go_to_next_frame_tooltip, frame_view_go_to_next_frame.get_id());
        playback_section.add_action(play_pause_tooltip, frame_view_play_pause.get_id());
        playback_section.add_action(toggle_onionskin_visible_tooltip, frame_view_toggle_onionskin_visible.get_id());
        _menu.add_section("Playback", &playback_section);

        auto create_section = MenuModel();
        create_section.add_action(frame_new_left_of_current_tooltip, frame_view_frame_new_left_of_current.get_id());
        create_section.add_action(frame_new_right_of_current_tooltip, frame_view_frame_new_right_of_current.get_id());
        create_section.add_action(frame_delete_tooltip, frame_view_frame_delete.get_id());
        _menu.add_section("Create / Delete", &create_section);

        auto other_section = MenuModel();
        other_section.add_action(frame_move_left_tooltip, frame_view_frame_move_left.get_id());
        other_section.add_action(frame_move_right_tooltip, frame_view_frame_move_right.get_id());
        other_section.add_action(frame_make_keyframe_inbetween_tooltip, frame_view_frame_make_keyframe_inbetween.get_id());
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

        _box.push_back(&_menu_button);

        auto onion_separator = SeparatorLine();
        onion_separator.set_size_request({button_width, 0});
        onion_separator.set_hexpand(false);

        _box.push_back(&onion_separator);
        _box.push_back(&_toggle_onionskin_visible_button);
        _box.push_back(&_onionskin_n_layers_spin_button);

        auto separator_start = SeparatorLine();
        separator_start.set_size_request({button_width, 0});
        separator_start.set_hexpand(false);
        _box.push_back(&separator_start);

        _box.push_back(&_frame_move_left_button);
        _box.push_back(&_frame_new_left_of_current_button);
        _box.push_back(&_frame_delete_button);
        _box.push_back(&_frame_make_keyframe_button);
        _box.push_back(&_frame_new_right_of_current_button);
        _box.push_back(&_frame_move_right_button);

        auto separator_left = SeparatorLine();
        separator_left.set_size_request({button_width, 0});
        separator_left.set_hexpand(false);
        _box.push_back(&separator_left);

        _box.push_back(&_jump_to_start_button);
        _box.push_back(&_go_to_previous_frame_button);
        _box.push_back(&_play_pause_button);
        _box.push_back(&_go_to_next_frame_button);
        _box.push_back(&_jump_to_end_button);

        auto separator_right = SeparatorLine();
        separator_right.set_size_request({button_width, 0});
        separator_right.set_hexpand(true);
        _box.push_back(&separator_right);
    }

    FrameView::ControlBar::operator Widget*() {
        return &_box;
    }

    void FrameView::ControlBar::on_preview_size_spin_button_value_changed(SpinButton* scale, ControlBar* instance)
    {
        instance->_owner->set_preview_size(scale->get_value());
    }

    void FrameView::ControlBar::on_jump_to_start()
    {
        std::cout << "[TODO] animation jump to start" << std::endl;
    }

    void FrameView::ControlBar::on_jump_to_end()
    {
        std::cout << "[TODO] animation jump to end" << std::endl;
    }

    void FrameView::ControlBar::on_go_to_previous_frame()
    {
        std::cout << "[TODO] animation go to previous frame" << std::endl;
    }

    void FrameView::ControlBar::on_go_to_next_frame()
    {
        std::cout << "[TODO] animation go to next frame" << std::endl;
    }

    void FrameView::ControlBar::on_play_pause()
    {
        std::cout << "[TODO] animation play pause" << std::endl;
    }

    void FrameView::ControlBar::on_frame_move_left()
    {
        std::cout << "[TODO] animation frame move left" << std::endl;
    }

    void FrameView::ControlBar::on_frame_move_right()
    {
        std::cout << "[TODO] animation frame move right" << std::endl;
    }

    void FrameView::ControlBar::on_frame_new_left_of_current()
    {
        std::cout << "[TODO] frame new left" << std::endl;
    }

    void FrameView::ControlBar::on_frame_new_right_of_current()
    {
        std::cout << "[TODO] frame new right" << std::endl;
    }

    void FrameView::ControlBar::on_frame_delete()
    {
        std::cout << "[TODO] frame delete" << std::endl;
    }

    void FrameView::ControlBar::on_frame_make_keyframe_inbetween()
    {
        std::cout << "[TODO] frame keyframe inbetween" << std::endl;
    }

    void FrameView::ControlBar::on_toggle_onionskin_visible()
    {
        state::onionskin_visible = not state::onionskin_visible;

        _toggle_onionskin_visible_button.set_signal_toggled_blocked(true);
        _toggle_onionskin_visible_button.set_active(state::onionskin_visible);
        _toggle_onionskin_visible_button.set_signal_toggled_blocked(false);
    }

    void FrameView::ControlBar::set_n_onionskin_layers(size_t n)
    {
        state::onionskin_n_layers = std::min(state::current_frame, n);
        _onionskin_n_layers_spin_button.set_signal_value_changed_blocked(true);
        _onionskin_n_layers_spin_button.set_value(n);
        _onionskin_n_layers_spin_button.set_signal_value_changed_blocked(false);
    }

    void FrameView::ControlBar::on_onionskin_spin_button_value_changed(SpinButton* scale, ControlBar* instance)
    {
        auto v = scale->get_value();
        state::onionskin_n_layers = v;
    }

    // ###

    FrameView::FrameView()
    {
        for (size_t i = 0; i < state::n_frames; ++i)
        {
            auto* column = _frame_columns.emplace_back(new FrameColumn(this, i));
            _frame_column_list_view.push_back(column->operator Widget*());
        }

        _frame_column_list_view.get_selection_model()->connect_signal_selection_changed(on_selection_changed, this);

        _frame_column_list_view.set_expand(false);
        _frame_colum_list_view_window_spacer.set_expand(true);
        _frame_column_list_view_window.set_expand(true);

        _frame_colum_list_view_window_box.push_back(&_frame_column_list_view);
        _frame_colum_list_view_window_box.push_back(&_frame_colum_list_view_window_spacer);

        _frame_column_list_view_window.set_child(&_frame_colum_list_view_window_box);
        _main.push_back(_control_bar);
        _main.push_back(&_frame_column_list_view_window);
    }

    void FrameView::update()
    {
        set_selection(state::current_layer, state::current_frame);
    }

    FrameView::operator Widget*()
    {
        return &_main;
    }

    void FrameView::set_selection(size_t layer_i, size_t frame_i)
    {
        _selected_layer_i = layer_i;
        _selected_frame_i = frame_i;

        for (auto* column : _frame_columns)
            column->select_layer(layer_i);

        _frame_column_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _frame_column_list_view.get_selection_model()->select(frame_i);
        _frame_column_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);

        state::current_frame = _selected_frame_i;
        state::current_layer = _selected_layer_i;

        state::layer_view->update();
        state::animation_preview->update();
    }

    void FrameView::on_selection_changed(SelectionModel*, size_t i, size_t n_items, FrameView* instance)
    {
        // override ui changing if user clicks 1px border between columns
        instance->set_selection(instance->_selected_layer_i, i);
    }

    void FrameView::set_preview_size(size_t x)
    {
        _preview_size = x;

        for (auto* column : _frame_columns)
            column->set_preview_size(_preview_size);
    }

    size_t FrameView::get_preview_size() const
    {
        return _preview_size;
    }
}