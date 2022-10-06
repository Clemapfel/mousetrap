// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
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
#include <include/frame.hpp>
#include <include/scrollbar.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class LayerView : public AppComponent
    {
        public:
            LayerView();
            operator Widget*() override;
            void update() override;

        private:
            static inline float thumbnail_height = 5 * state::margin_unit;
            static inline const float thumbnail_margin_top_bottom = 0.5 * state::margin_unit;
            static inline const float thumbnail_margin_left_right = 0.25 * state::margin_unit;
            static inline Shader* transparency_tiling_shader = nullptr;

            class LayerFrameDisplay
            {
                public:
                    LayerFrameDisplay(size_t layer, size_t frame, LayerView* owner);
                    ~LayerFrameDisplay();

                    operator Widget*();
                    void update();
                    void update_selection();

                    size_t _layer;
                    size_t _frame;
                    LayerView* _owner;

                    Vector2f _canvas_size;

                    static void on_gl_area_realize(Widget*, LayerFrameDisplay*);
                    static void on_gl_area_resize(GLArea*, int, int, LayerFrameDisplay*);

                    GLArea _gl_area;
                    Shape* _transparency_tiling_shape;
                    Shape* _layer_shape;

                    static inline Texture* selection_indicator_texture = nullptr;

                    AspectFrame _aspect_frame;

                    ImageDisplay _layer_frame_active_icon = ImageDisplay(get_resource_path() + "icons/layer_frame_active.png");
                    Overlay _overlay;
                    Label _frame_widget_label;
                    Frame _frame_widget = Frame();
                    ListView _main = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);
            };

            class LayerRow
            {
                public:
                    LayerRow(size_t layer, LayerView* owner);

                    operator Widget*();
                    void update();
                    void update_selection();

                private:
                    size_t _layer;
                    LayerView* _owner;

                    std::deque<LayerFrameDisplay> _layer_frame_displays;
                    ListView _layer_frame_display_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
                    static void on_layer_frame_display_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerRow* instance);
            };

            size_t _selected_frame;
            size_t _selected_layer;
            void set_selection(size_t layer_i, size_t frame_i);

            std::deque<LayerRow> _layer_rows;
            ListView _layer_row_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);

            static void on_layer_row_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView*);
    };
}

#include <app/layer_view/layer_frame_display.inl>
#include <app/layer_view/layer_row.inl>

namespace mousetrap
{
    LayerView::operator Widget*()
    {
        return &_layer_row_list_view;
    }

    void LayerView::on_layer_row_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView* instance)
    {
        instance->set_selection(first_item_position, instance->_selected_frame);
    }

    void LayerView::set_selection(size_t layer_i, size_t frame_i)
    {
        _selected_frame = frame_i;
        _selected_layer = layer_i;

        for (auto& row : _layer_rows)
            row.update_selection();

        _layer_row_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _layer_row_list_view.get_selection_model()->select(layer_i);
        _layer_row_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    LayerView::LayerView()
    {
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            _layer_rows.emplace_back(layer_i, this);
            _layer_rows.back().update();
        }

        for (auto& row : _layer_rows)
            _layer_row_list_view.push_back(row);

        _layer_row_list_view.get_selection_model()->connect_signal_selection_changed(
                on_layer_row_list_view_selection_changed, this);
    }

    void LayerView::update()
    {}
}