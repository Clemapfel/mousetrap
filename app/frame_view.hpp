// 
// Copyright 2022 Clemens Cords
// Created on 12/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    class FrameView : public AppComponent
    {
        public:
            FrameView();

            void update() override;
            operator Widget*() override;

            size_t get_preview_size() const;
            void set_preview_size(size_t);

        private:
            class FramePreview
            {
                public:
                    FramePreview(FrameView* owner, Layer* layer, size_t frame_i);

                    void update();
                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);
                    void set_is_inbetween(bool);
                    void set_preview_size(size_t);

                private:
                    FrameView* _owner;
                    Layer* _layer;
                    size_t _frame_i;

                    GLArea _area;

                    static inline Shader* _transparency_tiling_shader = nullptr;
                    Vector2f _canvas_size = Vector2f(1, 1);

                    Shape* _transparency_tiling_shape = nullptr;
                    Shape* _layer_shape = nullptr;

                    static void on_realize(Widget*, FramePreview*);
                    static void on_resize(GLArea*, int w, int h, FramePreview*);

                    bool _is_inbetween;
                    Label _inbetween_label = Label("&#8943;");
                    Overlay _inbetween_label_overlay;
            };

            size_t _preview_size = state::settings_file->get_value_as<int>("frame_view", "frame_preview_size");

            class FrameColumn
            {
                public:
                    FrameColumn(FrameView*, size_t frame_i);
                    ~FrameColumn();

                    operator Widget*();
                    void update();

                    void set_frame(size_t i);
                    void select_layer(size_t i);

                    void set_is_first_frame(bool);
                    void set_preview_size(size_t);

                private:
                    FrameView* _owner;

                    size_t _frame_i;
                    ListView _list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
                    static void on_selection_changed(SelectionModel*, size_t i, size_t n_items, FrameColumn* instance);

                    struct PreviewElement
                    {
                        FramePreview preview;
                        ListView wrapper;
                        Frame frame;
                        Label frame_label;
                        Label layer_label;
                        SeparatorLine layer_label_spacer;
                    };

                    std::vector<PreviewElement*> _preview_elements;
            };

            class ControlBar
            {
                public:
                    ControlBar(FrameView* owner);
                    operator Widget*();

                private:
                    ShortcutController _shortcut_controller = ShortcutController(state::app);

                    FrameView* _owner;

                    Button _toggle_onionskin_visible_button;
                    ImageDisplay _toggle_onionskin_visible_icon = ImageDisplay(get_resource_path() + "icons/toggle_onionskin_visible.png");
                    Action _toggle_onionskin_visible_action = Action("frame_view.toggle_onionskin_visible");
                    void on_toggle_onionskin_visible();

                    Button _jump_to_start_button;
                    ImageDisplay _jump_to_start_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                    Action _jump_to_start_action = Action("frame_view.jump_to_start");
                    void on_jump_to_start();

                    Button _jump_to_end_button;
                    ImageDisplay _jump_to_end_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_end.png");
                    Action _jump_to_end_action = Action("frame_view.jump_to_end");
                    void on_jump_to_end();

                    Button _go_to_previous_frame_button;
                    ImageDisplay _go_to_previous_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_previous_frame.png");
                    Action _go_to_previous_frame_action = Action("frame_view.go_to_previous_frame");
                    void on_go_to_previous_frame();

                    Button _go_to_next_frame_button;
                    ImageDisplay _go_to_next_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_next_frame.png");
                    Action _go_to_next_frame_action = Action("frame_view.go_to_next_frame");
                    void on_go_to_next_frame();

                    Button _play_pause_button;
                    ImageDisplay _play_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_play.png");
                    ImageDisplay _pause_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_pause.png");
                    Action _play_pause_action = Action("frame_view.play_pause");
                    void on_play_pause();

                    Button _frame_move_right_button;
                    ImageDisplay _frame_move_right_icon = ImageDisplay(get_resource_path() + "icons/frame_move_right.png");
                    Action _frame_move_right_action = Action("frame_view.frame_move_right");
                    void on_frame_move_right();

                    Button _frame_move_left_button;
                    ImageDisplay _frame_move_left_icon = ImageDisplay(get_resource_path() + "icons/frame_move_left.png");
                    Action _frame_move_left_action = Action("frame_view.frame_move_left");
                    void on_frame_move_left();

                    Button _frame_new_left_of_current_button;
                    ImageDisplay _frame_new_left_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_left_of_current.png");
                    Action _frame_new_left_of_current_action = Action("frame_view.frame_new_left_of_current");
                    void on_frame_new_left_of_current();

                    Button _frame_new_right_of_current_button;
                    ImageDisplay _frame_new_right_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_right_of_current.png");
                    Action _frame_new_right_of_current_action = Action("frame_view.frame_new_right_of_current");
                    void on_frame_new_right_of_current();

                    Button _frame_delete_button;
                    ImageDisplay _frame_delete_icon = ImageDisplay(get_resource_path() + "icons/frame_delete.png");
                    Action _frame_delete_action = Action("frame_view.frame_delete");
                    void on_frame_delete();

                    Button _frame_make_keyframe_button;
                    ImageDisplay _frame_is_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    ImageDisplay _frame_is_not_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    Action _frame_make_keyframe_inbetween_action = Action("frame_view.frame_make_keyframe_inbetween");
                    void on_frame_make_keyframe_inbetween();

                    size_t _preview_size = state::settings_file->get_value_as<int>("layer_view", "layer_preview_preview_size");
                    MenuModel _preview_size_submenu;

                    Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _preview_size_label = Label("Preview Size (px): ");
                    SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);
                    static void on_preview_size_spin_button_value_changed(SpinButton*, ControlBar* instance);
                    
                    MenuButton _menu_button;
                    Label _menu_button_label = Label("Frames");
                    MenuModel _menu;
                    PopoverMenu _popover_menu = PopoverMenu(&_menu);

                    Tooltip _tooltip;

                    Box _box = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            ControlBar _control_bar = ControlBar(this);

            size_t _selected_layer_i = 0;
            size_t _selected_frame_i = 0;
            void set_selection(size_t layer_i, size_t frame_i);

            std::vector<FrameColumn*> _frame_columns;

            ListView _frame_column_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
            static void on_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items, FrameView* instance);

            Box _frame_colum_list_view_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _frame_colum_list_view_window_spacer;

            ScrolledWindow _frame_column_list_view_window;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}

// ###

namespace mousetrap
{
    FrameView::FramePreview::FramePreview(FrameView* owner, Layer* layer, size_t frame_i)
        : _owner(owner), _layer(layer), _frame_i(frame_i)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);

        set_preview_size(owner->_preview_size);
        _inbetween_label_overlay.set_child(&_area);
        _inbetween_label_overlay.add_overlay(&_inbetween_label);

        set_is_inbetween(false);
    }

    void FrameView::FramePreview::set_preview_size(size_t x)
    {
        float width = (state::layer_resolution.y / state::layer_resolution.x) * x;
        _area.set_size_request({width, x});
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
                SeparatorLine()
            });

            element->frame_label.set_visible(false);
            element->layer_label.set_visible(false);
            element->layer_label_spacer.set_visible(false);
            element->layer_label.set_margin_horizontal(state::margin_unit);

            element->frame.set_label_align(0.5);
            element->frame.set_child(element->preview);
            element->frame.set_label_widget(&element->frame_label);

            element->layer_label_spacer.set_size_request({state::margin_unit / 10 * 4, 0});
            element->wrapper.push_back(&element->layer_label_spacer);

            element->wrapper.push_back(&element->frame);
            _list_view.push_back(&element->wrapper);
        }

        _list_view.get_selection_model()->connect_signal_selection_changed(on_selection_changed, this);
        if (_frame_i == 0)
            set_is_first_frame(true);
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

        _toggle_onionskin_visible_action.set_do_function([](ControlBar* instance) {
            instance->on_toggle_onionskin_visible();
        }, this);
        _toggle_onionskin_visible_action.add_shortcut(state::keybindings_file->get_value("frame_view", "toggle_onionskin_visible"));
        state::app->add_action(_toggle_onionskin_visible_action);
        _shortcut_controller.add_action(_toggle_onionskin_visible_action.get_id());

        _jump_to_start_action.set_do_function([](ControlBar* instance) {
            instance->on_jump_to_start();
        }, this);
        _jump_to_start_action.add_shortcut(state::keybindings_file->get_value("frame_view", "jump_to_start"));
        state::app->add_action(_jump_to_start_action);
        _shortcut_controller.add_action(_jump_to_start_action.get_id());

        _jump_to_end_action.set_do_function([](ControlBar* instance) {
            instance->on_jump_to_end();
        }, this);
        _jump_to_end_action.add_shortcut(state::keybindings_file->get_value("frame_view", "jump_to_end"));
        state::app->add_action(_jump_to_end_action);
        _shortcut_controller.add_action(_jump_to_end_action.get_id());

        _go_to_previous_frame_action.set_do_function([](ControlBar* instance) {
            instance->on_go_to_previous_frame();
        }, this);
        _go_to_previous_frame_action.add_shortcut(state::keybindings_file->get_value("frame_view", "go_to_previous_frame"));
        state::app->add_action(_go_to_previous_frame_action);
        _shortcut_controller.add_action(_go_to_previous_frame_action.get_id());

        _go_to_next_frame_action.set_do_function([](ControlBar* instance) {
            instance->on_go_to_next_frame();
        }, this);
        _go_to_next_frame_action.add_shortcut(state::keybindings_file->get_value("frame_view", "go_to_next_frame"));
        state::app->add_action(_go_to_next_frame_action);
        _shortcut_controller.add_action(_go_to_next_frame_action.get_id());

        _play_pause_action.set_do_function([](ControlBar* instance) {
            instance->on_play_pause();
        }, this);
        _play_pause_action.add_shortcut(state::keybindings_file->get_value("frame_view", "play_pause"));
        state::app->add_action(_play_pause_action);
        _shortcut_controller.add_action(_play_pause_action.get_id());

        _frame_move_right_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_move_right();
        }, this);
        _frame_move_right_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_move_right"));
        state::app->add_action(_frame_move_right_action);
        _shortcut_controller.add_action(_frame_move_right_action.get_id());

        _frame_move_left_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_move_left();
        }, this);
        _frame_move_left_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_move_left"));
        state::app->add_action(_frame_move_left_action);
        _shortcut_controller.add_action(_frame_move_left_action.get_id());

        _frame_new_left_of_current_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_new_left_of_current();
        }, this);
        _frame_new_left_of_current_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_new_left_of_current"));
        state::app->add_action(_frame_new_left_of_current_action);
        _shortcut_controller.add_action(_frame_new_left_of_current_action.get_id());

        _frame_new_right_of_current_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_new_right_of_current();
        }, this);
        _frame_new_right_of_current_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_new_right_of_current"));
        state::app->add_action(_frame_new_right_of_current_action);
        _shortcut_controller.add_action(_frame_new_right_of_current_action.get_id());

        _frame_delete_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_delete();
        }, this);
        _frame_delete_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_delete"));
        state::app->add_action(_frame_delete_action);
        _shortcut_controller.add_action(_frame_delete_action.get_id());

        _frame_make_keyframe_inbetween_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_make_keyframe_inbetween();
        }, this);
        _frame_make_keyframe_inbetween_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_make_keyframe_inbetween"));
        state::app->add_action(_frame_make_keyframe_inbetween_action);
        _shortcut_controller.add_action(_frame_make_keyframe_inbetween_action.get_id());

        // GUI
        _toggle_onionskin_visible_button.set_child(&_toggle_onionskin_visible_icon);
        _toggle_onionskin_visible_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_toggle_onionskin_visible_action.activate();
        }, this);

        _jump_to_start_button.set_child(&_jump_to_start_icon);
        _jump_to_start_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_jump_to_start_action.activate();
        }, this);

        _jump_to_end_button.set_child(&_jump_to_end_icon);
        _jump_to_end_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_jump_to_end_action.activate();
        }, this);

        _go_to_previous_frame_button.set_child(&_go_to_previous_frame_icon);
        _go_to_previous_frame_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_go_to_previous_frame_action.activate();
        }, this);

        _go_to_next_frame_button.set_child(&_go_to_next_frame_icon);
        _go_to_next_frame_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_go_to_next_frame_action.activate();
        }, this);

        _play_pause_button.set_child(state::playback_active ? &_pause_icon : &_play_icon);
        _play_pause_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_play_pause_action.activate();
        }, this);

        _frame_move_right_button.set_child(&_frame_move_right_icon);
        _frame_move_right_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_frame_move_right_action.activate();
        }, this);

        _frame_move_left_button.set_child(&_frame_move_left_icon);
        _frame_move_left_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_frame_move_left_action.activate();
        }, this);

        _frame_new_left_of_current_button.set_child(&_frame_new_left_of_current_icon);
        _frame_new_left_of_current_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_frame_new_left_of_current_action.activate();
        }, this);

        _frame_new_right_of_current_button.set_child(&_frame_new_right_of_current_icon);
        _frame_new_right_of_current_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_frame_new_right_of_current_action.activate();
        }, this);

        _frame_delete_button.set_child(&_frame_delete_icon);
        _frame_delete_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_frame_delete_action.activate();
        }, this);

        bool is_keyframe = state::layers.at(state::current_layer)->frames.at(state::current_frame).is_keyframe;
        _frame_make_keyframe_button.set_child(is_keyframe ? &_frame_is_not_keyframe_icon : &_frame_is_keyframe_icon);
        _frame_make_keyframe_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_frame_make_keyframe_inbetween_action.activate();
        }, this);
        
        // Tooltips

        auto toggle_onionskin_visible_tooltip = state::tooltips_file->get_value("frame_view", "toggle_onionskin_visible");
        _toggle_onionskin_visible_button.set_tooltip_text(toggle_onionskin_visible_tooltip);

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
        playback_section.add_action(jump_to_start_tooltip, _jump_to_start_action.get_id());
        playback_section.add_action(jump_to_end_tooltip, _jump_to_end_action.get_id());
        playback_section.add_action(go_to_previous_frame_tooltip, _go_to_previous_frame_action.get_id());
        playback_section.add_action(go_to_next_frame_tooltip, _go_to_next_frame_action.get_id());
        playback_section.add_action(play_pause_tooltip, _play_pause_action.get_id());
        playback_section.add_action(toggle_onionskin_visible_tooltip, _toggle_onionskin_visible_action.get_id());
        _menu.add_section("Playback", &playback_section);

        auto create_section = MenuModel();
        create_section.add_action(frame_new_left_of_current_tooltip, _frame_new_left_of_current_action.get_id());
        create_section.add_action(frame_new_right_of_current_tooltip, _frame_new_right_of_current_action.get_id());
        create_section.add_action(frame_delete_tooltip, _frame_delete_action.get_id());
        _menu.add_section("Create / Delete", &create_section);

        auto other_section = MenuModel();
        other_section.add_action(frame_move_left_tooltip, _frame_move_left_action.get_id());
        other_section.add_action(frame_move_right_tooltip, _frame_move_right_action.get_id());
        other_section.add_action(frame_make_keyframe_inbetween_tooltip, _frame_make_keyframe_inbetween_action.get_id());
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
        _box.push_back(&_toggle_onionskin_visible_button);

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

        _box.add_controller(&_shortcut_controller);
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
        std::cout << "[TODO] show animation preview" << std::endl;
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