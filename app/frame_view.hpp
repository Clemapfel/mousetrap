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

                private:
                    FrameView* _owner;

                    size_t _frame_i;
                    ListView _list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
                    static void on_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items, FrameColumn* instance);

                    struct PreviewElement
                    {
                        FramePreview preview;
                        ListView wrapper;
                        Frame frame;
                        Label label;
                    };

                    std::vector<PreviewElement*> _preview_elements;
            };

            class ControlBar
            {
                public:
                    ControlBar(FrameView* owner);

                private:
                    Box _box;

                    Button _jump_to_start_button;
                    ImageDisplay _jump_to_start_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                    Action _jump_to_start_action = Action("frame_view.jump_to_start");

                    Button _jump_to_end_button;
                    ImageDisplay _jump_to_end_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                    Action _jump_to_end_action = Action("frame_view.jump_to_end");

                    Button _go_to_previous_frame_button;
                    ImageDisplay _go_to_previous_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_previous_frame.png");
                    Action _go_to_previous_frame_action = Action("frame_view.go_to_previous_frame");

                    Button _go_to_next_frame_button;
                    ImageDisplay _go_to_next_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_previous_frame.png");
                    Action _go_to_next_frame_action = Action("frame_view.go_to_next_frame");

                    Button _play_pause_button;
                    ImageDisplay _play_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_play.png");
                    ImageDisplay _pause_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_pause.png");
                    Action _play_action = Action("frame_view.play");
                    Action _pause_action = Action("frame_view.pause");

                    Button _frame_move_right_button;
                    ImageDisplay _frame_move_right_icon = ImageDisplay(get_resource_path() + "icons/frame_move_right.png");
                    Action _frame_move_right_action = Action("frame_view.frame_move_right");

                    Button _frame_move_left_button;
                    ImageDisplay _frame_move_left_icon = ImageDisplay(get_resource_path() + "icons/frame_move_left.png");
                    Action _frame_move_left_action = Action("frame_view.frame_move_left");

                    Button _frame_new_left_of_current_button;
                    ImageDisplay _frame_new_left_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_left_of_current.png");
                    Action _frame_new_left_of_current_action = Action("frame_view.frame_new_left_of_current");

                    Button _frame_new_right_of_current_button;
                    ImageDisplay _frame_new_right_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_right_of_current.png");
                    Action _frame_new_right_of_current_action = Action("frame_view.frame_new_right_of_current");

                    Button _frame_delete_button;
                    ImageDisplay _frame_delete_icon = ImageDisplay(get_resource_path() + "icons/frame_delete.png");
                    Action _frame_delete_action = Action("frame_view.frame_delete");

                    Button _make_keyframe_button;
                    ImageDisplay _frame_is_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    ImageDisplay _frame_is_not_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    Action _frame_make_keyframe_action = Action("frame_view.frame_make_keyframe");
                    Action _frame_make_inbetween_action = Action("frame_view.frame_make_inbetween");
            };

            size_t _selected_layer_i = 0;
            size_t _selected_frame_i = 0;
            void set_selection(size_t layer_i, size_t frame_i);

            std::vector<FrameColumn*> _frame_columns;

            ListView _frame_column_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
            static void on_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items, FrameView* instance);

            Box _frame_colum_list_view_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _frame_colum_list_view_window_spacer;

            ScrolledWindow _frame_column_list_view_window;

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
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

        for (auto* layer : state::layers)
        {
            auto* element = _preview_elements.emplace_back(new PreviewElement{
                FramePreview(owner, layer, _frame_i),
                ListView(),
                Frame(),
                Label((_frame_i < 10 ? "00" : (_frame_i < 100 ? "0" : "")) + std::to_string(_frame_i))
            });

            element->label.set_visible(false);

            element->frame.set_label_align(0.5);
            element->frame.set_child(element->preview);
            element->frame.set_label_widget(&element->label);
            element->wrapper.push_back(&element->frame);
            _list_view.push_back(&element->wrapper);
        }

        _list_view.get_selection_model()->connect_signal_selection_changed(on_selection_changed, this);
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
            _preview_elements.at(i)->label.set_visible(i == layer_i);
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

    void
    FrameView::on_selection_changed(SelectionModel*, size_t i, size_t n_items, FrameView* instance)
    {
        // override ui changing if user clicks 1px border between columns
        instance->set_selection(instance->_selected_layer_i, i);
    }
}