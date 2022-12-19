// 
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class LayerView : public AppComponent
    {
        public:
            LayerView();
            
            void update() override;
            operator Widget*() override;
            
        private:
            class LayerPreview
            {
                public:
                    LayerPreview(Layer*, size_t frame_i);
                    ~LayerPreview();

                    void update();
                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);

                private:
                    Layer* _layer;
                    size_t _frame_i;
                    
                    GLArea _area;

                    static inline Shader* _transparency_tiling_shader = nullptr;
                    Vector2f _canvas_size = Vector2f(1, 1);
                    
                    Shape* _transparency_tiling_shape;
                    Shape* _layer_shape;

                    static void on_realize(Widget*, LayerPreview*);
                    static void on_resize(GLArea*, int w, int h, LayerPreview*);
            };

            class LayerRow
            {
                public:
                    LayerRow(Layer*, size_t frame_i);

                    void update();
                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);

                private:
                    Layer* _layer;
                    size_t _frame_i;

                    Box _outer_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    LayerPreview _layer_preview;
                    Frame _layer_preview_frame;
                    ListView _layer_preview_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

                    ToggleButton _is_visible_toggle_button;
                    ImageDisplay _is_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_visible.png");
                    ImageDisplay _is_not_visible_icon = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png");
                    static void on_is_visible_toggle_button_toggled(ToggleButton*, LayerRow* instance);

                    ToggleButton _is_locked_toggle_button;
                    ImageDisplay _is_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_locked.png");
                    ImageDisplay _is_not_locked_icon = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png");
                    static void on_is_locked_toggle_button_toggled(ToggleButton*, LayerRow* instance);

                    Box _visible_locked_buttons_box = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            std::deque<LayerRow> _layer_rows;
            ListView _layer_rows_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_MULTIPLE);
    };
}

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
        update();
    }

    void LayerView::LayerPreview::set_frame(size_t i)
    {
        _frame_i = i;
        update();
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
        
        instance->update();
        
        area->queue_render();
    }

    void LayerView::LayerPreview::on_resize(GLArea* area, int w, int h, LayerPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_area.queue_render();
    }

    void LayerView::LayerPreview::update()
    {
        if (not _area.get_is_realized())
            return;

        _layer_shape->set_texture(_layer->frames.at(_frame_i).texture);
        _layer_shape->set_color(RGBA(1, 1, 1, _layer->opacity));

        auto height = state::settings_file->get_value_as<int>("layer_view", "layer_preview_thumbnail_size");
        auto width = state::layer_resolution.x / state::layer_resolution.y * height;
        _area.set_size_request({width, height});
        _area.queue_render();
    }

    // ###

    LayerView::LayerRow::LayerRow(Layer* layer, size_t frame_i)
        : _layer(layer), _frame_i(frame_i), _layer_preview(layer, frame_i)
    {
        _layer_preview_frame.set_child(_layer_preview);
        _layer_preview_frame.set_label_widget(nullptr);
        _layer_preview_list_view.push_back(&_layer_preview_frame);

        _visible_locked_buttons_box.push_back(&_is_visible_toggle_button);
        _visible_locked_buttons_box.push_back(&_is_locked_toggle_button);

        if (layer->is_visible)
        {
            _is_visible_toggle_button.set_active(true);
            _is_visible_toggle_button.set_child(&_is_visible_icon);
        }
        else
        {
            _is_visible_toggle_button.set_active(false);
            _is_visible_toggle_button.set_child(&_is_not_visible_icon);
        }

        if (layer->is_locked)
        {
            _is_locked_toggle_button.set_active(true);
            _is_locked_toggle_button.set_child(&_is_locked_icon);
        }
        else
        {
            _is_locked_toggle_button.set_active(false);
            _is_locked_toggle_button.set_child(&_is_not_locked_icon);
        }

        for (auto* button : {&_is_visible_toggle_button, &_is_locked_toggle_button})
        {
            button->set_expand(false);
            button->set_align(GTK_ALIGN_CENTER);
        }

        _is_visible_toggle_button.connect_signal_toggled(on_is_visible_toggle_button_toggled, this);
        _is_locked_toggle_button.connect_signal_toggled(on_is_locked_toggle_button_toggled, this);

        for (auto* icon : {&_is_visible_icon, &_is_not_visible_icon, &_is_locked_icon, &_is_not_locked_icon})
            icon->set_size_request(icon->get_size());

        _outer_box.push_back(&_visible_locked_buttons_box);
        _outer_box.push_back(&_layer_preview_list_view);
        _outer_box.set_margin(state::margin_unit);
    }

    LayerView::LayerRow::operator Widget*()
    {
        return &_outer_box;
    }

    void LayerView::LayerRow::update()
    {
        _layer_preview.update();
    }

    void LayerView::LayerRow::set_layer(Layer* layer)
    {
        _layer = layer;
        _layer_preview.set_layer(layer);
    }

    void LayerView::LayerRow::set_frame(size_t i)
    {
        _frame_i = i;
        _layer_preview.set_frame(i);
    }

    void LayerView::LayerRow::on_is_visible_toggle_button_toggled(ToggleButton*, LayerRow* instance)
    {}

    void LayerView::LayerRow::on_is_locked_toggle_button_toggled(ToggleButton*, LayerRow* instance)
    {}

    // ###

    LayerView::LayerView()
    {
        for (auto* layer : state::layers)
            _layer_rows.emplace_back(layer, state::current_frame);

        for (auto& row : _layer_rows)
            _layer_rows_list_view.push_back(row);
    }

    LayerView::operator Widget*()
    {
        return &_layer_rows_list_view;
    }

    void LayerView::update()
    {
        for (auto& row : _layer_rows)
            row.update();
    }
}
