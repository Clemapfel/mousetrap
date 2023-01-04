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

            size_t _thumbnail_size = state::settings_file->get_value_as<int>("frame_view", "frame_preview_thumbnail_size");

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

                    Button _show_animation_preview_button;
                    ImageDisplay _show_animation_preview_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                    Action _show_animation_preview_action = Action("frame_view.show_animation_preview");
                    void on_show_animation_preview();

                    Button _jump_to_start_button;
                    ImageDisplay _jump_to_start_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                    Action _jump_to_start_action = Action("frame_view.jump_to_start");
                    void on_jump_to_start();

                    Button _jump_to_end_button;
                    ImageDisplay _jump_to_end_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
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
                    Action _play_action = Action("frame_view.play");
                    Action _pause_action = Action("frame_view.pause");
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
                    Action _frame_make_keyframe_action = Action("frame_view.frame_make_keyframe");
                    Action _frame_make_inbetween_action = Action("frame_view.frame_make_inbetween");
                    void on_frame_make_keyframe_inbetween();

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

        float width = (state::layer_resolution.y / state::layer_resolution.x) * owner->_thumbnail_size;
        _area.set_size_request({width, owner->_thumbnail_size});

        _inbetween_label_overlay.set_child(&_area);
        _inbetween_label_overlay.add_overlay(&_inbetween_label);

        set_is_inbetween(false);
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
                Label((_frame_i < 10 ? "00" : (_frame_i < 100 ? "0" : "")) + std::to_string(_frame_i)),
                Label(std::string("<span size=\"120%\">") + (_frame_i < 10 ? "0" : "") + std::to_string(layer_i) + "</span>")
            });

            element->frame_label.set_visible(false);
            element->layer_label.set_visible(false);
            element->layer_label.set_margin_horizontal(state::margin_unit);

            element->frame.set_label_align(0.5);
            element->frame.set_child(element->preview);
            element->frame.set_label_widget(&element->frame_label);

            element->wrapper.push_back(&element->layer_label);
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
            preview->layer_label.set_visible(b);
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

        _show_animation_preview_action.set_do_function([](ControlBar* instance) {
            instance->on_show_animation_preview();
        }, this);
        _show_animation_preview_action.add_shortcut(state::keybindings_file->get_value("frame_view", "show_animation_preview"));
        state::app->add_action(_show_animation_preview_action);
        _shortcut_controller.add_action(_show_animation_preview_action.get_id());

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

        _play_action.set_do_function([](ControlBar* instance) {
            instance->on_play_pause();
        }, this);
        _play_action.add_shortcut(state::keybindings_file->get_value("frame_view", "play_pause"));
        state::app->add_action(_play_action);
        _shortcut_controller.add_action(_play_action.get_id());

        _pause_action.set_do_function([](ControlBar* instance) {
            instance->on_play_pause();
        }, this);
        _pause_action.add_shortcut(state::keybindings_file->get_value("frame_view", "play_pause"));
        state::app->add_action(_pause_action);
        _shortcut_controller.add_action(_pause_action.get_id());

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

        _frame_make_keyframe_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_make_keyframe_inbetween();
        }, this);
        _frame_make_keyframe_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_make_keyframe_inbetween"));
        state::app->add_action(_frame_make_keyframe_action);
        _shortcut_controller.add_action(_frame_make_keyframe_action.get_id());

        _frame_make_inbetween_action.set_do_function([](ControlBar* instance) {
            instance->on_frame_make_keyframe_inbetween();
        }, this);
        _frame_make_inbetween_action.add_shortcut(state::keybindings_file->get_value("frame_view", "frame_make_keyframe_inbetween"));
        state::app->add_action(_frame_make_inbetween_action);
        _shortcut_controller.add_action(_frame_make_inbetween_action.get_id());

        // GUI
        _show_animation_preview_button.set_child(&_show_animation_preview_icon);
        _show_animation_preview_button.connect_signal_clicked([](Button*, ControlBar* instance){
            instance->_show_animation_preview_action.activate();
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
            state::playback_active ? instance->_pause_action.activate() : instance->_play_action.activate();
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

            bool is_keyframe = state::layers.at(state::current_layer)->frames.at(state::current_frame).is_keyframe;
            if (is_keyframe)
                instance->_frame_make_inbetween_action.activate();
            else
                instance->_frame_make_keyframe_action.activate();

        }, this);

        for (auto& image : {&_jump_to_start_icon, &_jump_to_end_icon, &_go_to_previous_frame_icon, &_go_to_next_frame_icon, &_play_icon, &_pause_icon, &_frame_move_left_icon, &_frame_move_right_icon, &_frame_new_left_of_current_icon, &_frame_new_right_of_current_icon, &_frame_delete_icon, &_frame_is_keyframe_icon, &_frame_is_not_keyframe_icon})
            image->set_size_request(image->get_size());

        _box.push_back(&_show_animation_preview_button);

        auto button_width = _play_pause_button.get_preferred_size().natural_size.x;
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

    void FrameView::ControlBar::on_jump_to_start()
    {
        // TODO
    }

    void FrameView::ControlBar::on_jump_to_end()
    {
        // TODO
    }

    void FrameView::ControlBar::on_go_to_previous_frame()
    {
        // TODO
    }

    void FrameView::ControlBar::on_go_to_next_frame()
    {
        // TODO
    }

    void FrameView::ControlBar::on_play_pause()
    {
        // TODO
    }

    void FrameView::ControlBar::on_frame_move_left()
    {
        // TODO
    }

    void FrameView::ControlBar::on_frame_move_right()
    {
        // TODO
    }

    void FrameView::ControlBar::on_frame_new_left_of_current()
    {
        // TODO
    }

    void FrameView::ControlBar::on_frame_new_right_of_current()
    {
        // TODO
    }

    void FrameView::ControlBar::on_frame_delete()
    {
        // TODO
    }

    void FrameView::ControlBar::on_frame_make_keyframe_inbetween()
    {
        // TODO
    }

    void FrameView::ControlBar::on_show_animation_preview()
    {
        // TODO
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
}