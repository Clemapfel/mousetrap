// 
// Copyright 2022 Clemens Cords
// Created on 9/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/image_display.hpp>
#include <include/toggle_button.hpp>
#include <include/entry.hpp>
#include <include/scale.hpp>
#include <include/check_button.hpp>
#include <include/get_resource_path.hpp>
#include <include/box.hpp>
#include <include/list_view.hpp>
#include <include/reorderable_list.hpp>
#include <include/viewport.hpp>
#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/label.hpp>
#include <include/separator_line.hpp>
#include <include/scrolled_window.hpp>
#include <include/menu_button.hpp>
#include <include/popover.hpp>
#include <include/dropdown.hpp>
#include <include/overlay.hpp>
#include <include/center_box.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class LayerView : public AppComponent
    {
        public:
            LayerView();

            operator Widget*() override;
            void update();


        private:
            static inline float thumbnail_height = 5 * state::margin_unit;
            static inline Shader* transparency_tiling_shader = nullptr;
            static inline float thumbnail_margin = 0.5 * state::margin_unit;

            void set_selected_frame(size_t);

            struct WholeFrameDisplay
            {
                WholeFrameDisplay(size_t frame_i);
                operator Widget*();

                size_t _frame;

                Shape _transparency_tiling_shape;
                GLArea _transparency_area;
                Vector2f _canvas_size;
                static void on_transparency_area_realize(Widget*, WholeFrameDisplay*);
                static void on_transparency_area_resize(GLArea*, int, int, WholeFrameDisplay*);

                GLArea _layer_area;
                std::deque<Shape> _layer_shapes;
                static void on_layer_area_realize(Widget*, WholeFrameDisplay*);

                AspectFrame _aspect_frame;
                Overlay _overlay;

                Label _label;
                Box _main = Box(GTK_ORIENTATION_VERTICAL);
            };

            std::deque<WholeFrameDisplay> _whole_frame_displays;
            ListView _whole_frame_row_inner = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);
            ListView _whole_frame_row = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

            struct LayerFrameDisplay
            {
                LayerFrameDisplay(Layer* layer, size_t frame);
                ~LayerFrameDisplay();
                operator Widget*();

                Layer* _layer;
                size_t _frame;

                Vector2f _canvas_size;
                static void on_gl_area_realize(Widget*, LayerFrameDisplay*);
                static void on_gl_area_resize(GLArea*, int, int, LayerFrameDisplay*);

                GLArea _gl_area;
                Shape* _transparency_tiling_shape;
                Shape* _layer_shape;

                static inline Texture* selection_indicator_texture = nullptr;

                AspectFrame _aspect_frame;


                CenterBox _main = CenterBox(GTK_ORIENTATION_HORIZONTAL);
            };

            struct LayerRow
            {
                LayerRow(Layer*);
                operator Widget*();

                Layer* _layer;

                std::deque<LayerFrameDisplay> _layer_frame_displays;
                ListView _layer_frame_row = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
            };

            std::deque<LayerRow> _layer_rows;
            ListView _layer_rows_list = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            static void on_list_view_item_activate(ListView*, size_t item_position, LayerView* instance);
            static void on_reorderable_list_view_item_activate(ReorderableListView*, size_t item_position, LayerView* instance);
            static void on_selection_model_selection_changed(GtkSelectionModel*, int position, int n_items, LayerView* instance);
    };
}

// ###

namespace mousetrap
{
    void LayerView::on_list_view_item_activate(ListView*, size_t item_position, LayerView* instance)
    {
        instance->set_selected_frame(item_position);
    }

    void LayerView::on_selection_model_selection_changed(GtkSelectionModel*, int position, int n_items, LayerView* instance)
    {
        gtk_selection_model_select_item(instance->_whole_frame_row_inner.get_selection_model(), position, true);
        for (auto& row : instance->_layer_rows)
            gtk_selection_model_select_item(row._layer_frame_row.get_selection_model(), position, true);
    }

    void LayerView::WholeFrameDisplay::on_transparency_area_realize(Widget*, WholeFrameDisplay* instance)
    {
        instance->_transparency_area.make_current();

        if (transparency_tiling_shader == nullptr)
        {
            transparency_tiling_shader = new Shader();
            transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = Shape();
        instance->_transparency_tiling_shape.as_rectangle({0, 0}, {1, 1});

        auto task = RenderTask(&instance->_transparency_tiling_shape, transparency_tiling_shader);
        task.register_vec2("_canvas_size", &instance->_canvas_size);

        instance->_transparency_area.add_render_task(task);
        instance->_transparency_area.queue_render();
    }

    void LayerView::WholeFrameDisplay::on_transparency_area_resize(GLArea*, int w, int h, WholeFrameDisplay* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_transparency_area.queue_render();
    }

    void LayerView::WholeFrameDisplay::on_layer_area_realize(Widget*, WholeFrameDisplay* instance)
    {
        instance->_layer_area.make_current();

        instance->_layer_shapes.clear();
        for (size_t i = 0; i < state::layers.size(); ++i)
        {
            auto& shape = instance->_layer_shapes.emplace_back();
            shape.as_rectangle({0, 0}, {1, 1});
            shape.set_texture(&state::layers.at(i).frames.at(instance->_frame).texture);

            auto task = RenderTask(&shape, nullptr, nullptr, state::layers.at(i).blend_mode);
            instance->_layer_area.add_render_task(task);
        }

        instance->_layer_area.queue_render();
    }

    LayerView::WholeFrameDisplay::WholeFrameDisplay(size_t frame)
        : _frame(frame),
         _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y)),
         _label((frame <= 9 ? "00" : (frame <= 100 ? "0" : "")) + std::to_string(frame))
    {
        _transparency_area.connect_signal_realize(on_transparency_area_realize, this);
        _transparency_area.connect_signal_resize(on_transparency_area_resize, this);
        _layer_area.connect_signal_realize(on_layer_area_realize, this);

        for (auto* area : {&_transparency_area, &_layer_area})
        {
            area->set_size_request({thumbnail_height, (thumbnail_height / float(state::layer_resolution.x)) * state::layer_resolution.y});
            area->set_expand(false);
        }

        _aspect_frame.set_child(&_transparency_area);
        _aspect_frame.set_expand(false);
        _overlay.set_child(&_aspect_frame);
        _overlay.add_overlay(&_layer_area);

        _main.push_back(&_overlay);
        _main.push_back(&_label);

        _main.set_margin_start(thumbnail_margin);
        _main.set_margin_end(thumbnail_margin);
    }

    LayerView::WholeFrameDisplay::operator Widget*()
    {
        return &_main;
    }

    LayerView::LayerFrameDisplay::~LayerFrameDisplay()
    {
        delete _transparency_tiling_shape;
        delete _layer_shape;
    }

    void LayerView::LayerFrameDisplay::on_gl_area_realize(Widget*, LayerFrameDisplay* instance)
    {
        instance->_gl_area.make_current();

        if (transparency_tiling_shader == nullptr)
        {
            transparency_tiling_shader = new Shader();
            transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->_layer_shape = new Shape();
        instance->_layer_shape->as_rectangle({0, 0}, {1, 1});
        instance->_layer_shape->set_texture(&instance->_layer->frames.at(instance->_frame).texture);

        if (selection_indicator_texture == nullptr)
        {
            selection_indicator_texture = new Texture();
            selection_indicator_texture->create_from_file(get_resource_path() + "frame/frame_vertical.png");
        }

        auto task = RenderTask(instance->_transparency_tiling_shape, transparency_tiling_shader);
        task.register_vec2("_canvas_size", &instance->_canvas_size);

        instance->_gl_area.add_render_task(task);
        instance->_gl_area.add_render_task(instance->_layer_shape);
        instance->_gl_area.queue_render();
    }

    void LayerView::LayerFrameDisplay::on_gl_area_resize(GLArea*, int w, int h, LayerFrameDisplay* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_gl_area.queue_render();
    }

    LayerView::LayerFrameDisplay::LayerFrameDisplay(Layer* layer, size_t frame)
        : _layer(layer),
          _frame(frame),
          _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y))
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);
        _gl_area.set_size_request({thumbnail_height, (thumbnail_height / float(state::layer_resolution.x)) * state::layer_resolution.y});
        _gl_area.set_expand(false);

        _aspect_frame.set_child(&_gl_area);

        _main.set_center_widget(&_aspect_frame);
        _gl_area.queue_render();

        _main.set_margin_start(thumbnail_margin);
        _main.set_margin_end(thumbnail_margin);
    }

    LayerView::LayerFrameDisplay::operator Widget*()
    {
        return &_main;
    }

    LayerView::LayerRow::LayerRow(Layer* layer)
        : _layer(layer)
    {
        for (size_t i = 0; i < state::n_frames; ++i)
        {
            _layer_frame_displays.emplace_back(layer, i);
            _layer_frame_row.push_back(_layer_frame_displays.back());
        }
    }

    LayerView::LayerRow::operator Widget*()
    {
        return &_layer_frame_row;
    }

    void LayerView::update()
    {}

    LayerView::operator Widget*()
    {
        return &_main;
    }

    LayerView::LayerView()
    {
        for (size_t i = 0; i < state::n_frames; ++i)
        {
            _whole_frame_displays.emplace_back(i);
            _whole_frame_row_inner.push_back(_whole_frame_displays.back());
        }

        _whole_frame_row_inner.set_show_separators(true);
        _whole_frame_row_inner.connect_signal_list_item_activate(on_list_view_item_activate, this);
        _whole_frame_row_inner.set_single_click_activate(true);
        _whole_frame_row.push_back(&_whole_frame_row_inner);
        _whole_frame_row.set_halign(GTK_ALIGN_END);
        _whole_frame_row.set_cursor(GtkCursorType::NOT_ALLOWED);

        for (auto& layer : state::layers)
        {
           _layer_rows.emplace_back(&layer);
           _layer_rows.back().operator Widget*()->set_halign(GTK_ALIGN_END);
           _layer_rows.back()._layer_frame_row.set_show_separators(true);
           _layer_rows.back()._layer_frame_row.connect_signal_list_item_activate(on_list_view_item_activate, this);
           _layer_rows.back()._layer_frame_row.set_single_click_activate(true);
           _layer_rows_list.push_back(_layer_rows.back());
        }

        _layer_rows_list.set_cursor(GtkCursorType::POINTER);

        _main.push_back(&_whole_frame_row);
        _main.push_back(&_layer_rows_list);

        set_selected_frame(2);
    }

    void LayerView::set_selected_frame(size_t i)
    {
        gtk_selection_model_select_item(_whole_frame_row_inner.get_selection_model(), i, true);
        for (auto& row : _layer_rows)
        {
            gtk_selection_model_select_item(row._layer_frame_row.get_selection_model(), i, true);
        }
    }
}