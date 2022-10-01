// 
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
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
            void update(size_t layer_i);

            static inline float thumbnail_height = 5 * state::margin_unit;
            static inline Shader* transparency_tiling_shader = nullptr;

            struct WholeFrameDisplay
            {
                WholeFrameDisplay(size_t frame_i);
                operator Widget*();

                void update();

                size_t _frame;

                Shape* _transparency_tiling_shape;
                GLArea _transparency_area;
                Vector2f* _canvas_size;
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

            struct LayerFrameDisplay
            {
                LayerFrameDisplay(size_t layer, size_t frame);
                ~LayerFrameDisplay();
                operator Widget*();

                void update();

                size_t _layer;
                size_t _frame;

                Vector2f _canvas_size;
                static void on_gl_area_realize(Widget*, LayerFrameDisplay*);
                static void on_gl_area_resize(GLArea*, int, int, LayerFrameDisplay*);

                GLArea _gl_area;
                Shape* _transparency_tiling_shape;
                Shape* _layer_shape;

                static inline Texture* selection_indicator_texture = nullptr;

                AspectFrame _aspect_frame;
                Overlay _main;
            };

            struct LayerControlBar
            {
                LayerControlBar(size_t layer_i);
                operator Widget*();

                void update();

                size_t _layer;
                Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                ImageDisplay _locked_toggle_button_icon_locked = ImageDisplay(get_resource_path() + "icons/layer_locked.png", state::icon_scale);
                ImageDisplay _locked_toggle_button_icon_not_locked = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png", state::icon_scale);
                ToggleButton _locked_toggle_button;
                static void on_locked_toggle_button_toggled(ToggleButton*, LayerControlBar*);

                ImageDisplay _visible_toggle_button_icon_visible = ImageDisplay(get_resource_path() + "icons/layer_visible.png", state::icon_scale);
                ImageDisplay _visible_toggle_button_icon_not_visible = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png", state::icon_scale);
                ToggleButton _visible_toggle_button;
                static void on_visible_toggle_button_toggled(ToggleButton*, LayerControlBar*);

                Box _visible_locked_indicator_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit * 0.5);

                Label _menu_button_title_label;
                ScrolledWindow _menu_button_title_label_viewport;
                MenuButton _menu_button;
                Popover _menu_button_popover;
                Box _menu_button_popover_box = Box (GTK_ORIENTATION_VERTICAL);

                Label _name_label = Label("Name");
                SeparatorLine _name_separator;
                Entry _name_entry;
                Box _name_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_name_entry_activate(Entry*, LayerControlBar* instance);

                Label _visible_label = Label("Visible");
                SeparatorLine _visible_separator;
                CheckButton _visible_check_button;
                Box _visible_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_visible_check_button_toggled(CheckButton*, LayerControlBar* instance);

                Label _locked_label = Label("Locked");
                SeparatorLine _locked_separator;
                CheckButton _locked_check_button;
                Box _locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_locked_check_button_toggled(CheckButton*, LayerControlBar* instance);

                Label _opacity_label = Label("Opacity");
                SeparatorLine _opacity_separator;
                Scale _opacity_scale = Scale(0, 1, 0.01);
                Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_opacity_scale_value_changed(Scale*, LayerControlBar* instance);

                Label _blend_mode_label = Label("Blend");
                SeparatorLine _blend_mode_separator;
                Box _blend_mode_box = Box(GTK_ORIENTATION_HORIZONTAL);

                DropDown _blend_mode_dropdown;
                static inline const std::vector<BlendMode> _blend_modes_in_order = {
                        BlendMode::NORMAL,
                        BlendMode::ADD,
                        BlendMode::SUBTRACT,
                        BlendMode::REVERSE_SUBTRACT,
                        BlendMode::MULTIPLY,
                        BlendMode::MIN,
                        BlendMode::MAX
                };
                std::vector<Label> _blend_mode_dropdown_label_items;
                std::vector<Label> _blend_mode_dropdown_list_items;

                using on_blend_mode_select_data = struct {BlendMode blend_mode; LayerControlBar* instance;};
                static void on_blend_mode_select(on_blend_mode_select_data*);
            };

            struct LayerRow
            {
                ListView list = ListView(GTK_ORIENTATION_HORIZONTAL);
                LayerControlBar* control_bar;
                std::deque<LayerFrameDisplay> frame_display;
            };

            std::deque<LayerRow> _layer_rows;
            ReorderableListView _layer_row_list_view = ReorderableListView(GTK_ORIENTATION_VERTICAL);

            std::deque<WholeFrameDisplay> _whole_frame_displays;
            ListView _whole_frame_display_list_view = ListView(GTK_ORIENTATION_HORIZONTAL);

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}

namespace mousetrap
{
    // WHOLE FRAME DISPLAY

    void LayerView::WholeFrameDisplay::on_transparency_area_realize(Widget*, WholeFrameDisplay* instance)
    {
        instance->_transparency_area.make_current();

        if (transparency_tiling_shader == nullptr)
        {
            transparency_tiling_shader = new Shader();
            transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        auto task = RenderTask(instance->_transparency_tiling_shape, transparency_tiling_shader);
        task.register_vec2("_canvas_size", instance->_canvas_size);

        instance->_transparency_area.add_render_task(task);
        instance->_transparency_area.queue_render();
    }

    void LayerView::WholeFrameDisplay::on_transparency_area_resize(GLArea*, int w, int h, WholeFrameDisplay* instance)
    {
        *instance->_canvas_size = {w, h};
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

    void LayerView::WholeFrameDisplay::update()
    {
        if (not _layer_area.get_is_realized())
            return;

        if (_layer_shapes.size() != state::layers.size())
        {
            _layer_shapes.clear();
            for (size_t i = 0; i < state::layers.size(); ++i)
            {
                auto& shape = _layer_shapes.emplace_back();
                shape.as_rectangle({0, 0}, {1, 1});
            }
        }

        _layer_area.clear_render_tasks();
        for (size_t i = 0; i < state::layers.size(); ++i)
        {
            auto* shape = &_layer_shapes.at(i);
            shape->set_texture(&state::layers.at(i).frames.at(_frame).texture);
            auto task = RenderTask(shape, nullptr, nullptr, state::layers.at(i).blend_mode);
            _layer_area.add_render_task(task);
        }
    }

    LayerView::WholeFrameDisplay::WholeFrameDisplay(size_t frame)
        : _frame(frame),
          _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y)),
          _label((frame <= 9 ? "00" : (frame <= 100 ? "0" : "")) + std::to_string(frame))
    {
        _canvas_size = new Vector2f(1, 1);

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
    }

    LayerView::WholeFrameDisplay::operator Widget*()
    {
        return &_main;
    }

    // SINGLE FRAME DISPLAY

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
        instance->_layer_shape->set_texture(&state::layers.at(instance->_layer).frames.at(instance->_frame).texture);

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

    LayerView::LayerFrameDisplay::LayerFrameDisplay(size_t layer, size_t frame)
            : _layer(layer),
              _frame(frame),
              _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y))
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);
        _gl_area.set_size_request({thumbnail_height, (thumbnail_height / float(state::layer_resolution.x)) * state::layer_resolution.y});
        _gl_area.set_expand(false);

        _aspect_frame.set_child(&_gl_area);

        _main.set_child(&_aspect_frame);
        _gl_area.queue_render();
    }

    LayerView::LayerFrameDisplay::~LayerFrameDisplay()
    {
        delete _transparency_tiling_shape;
        delete _layer_shape;
    }

    LayerView::LayerFrameDisplay::operator Widget*()
    {
        return &_main;
    }

    void LayerView::LayerFrameDisplay::update()
    {
        if (not _gl_area.get_is_realized())
            return;

        auto& layer = state::layers.at(_layer);
        _layer_shape->set_texture(&layer.frames.at(_frame).texture);

        if (not layer.is_visible)
            _gl_area.set_opacity(0.2);
        else
            _gl_area.set_opacity(1);
    }

    // CONTROL BAR

    void LayerView::LayerControlBar::update()
    {
        auto& layer = state::layers.at(_layer);

        _visible_toggle_button.set_all_signals_blocked(true);
        _visible_toggle_button.set_active(layer.is_visible);
        _visible_toggle_button.set_child(layer.is_visible ? &_visible_toggle_button_icon_visible : &_visible_toggle_button_icon_not_visible);
        _visible_toggle_button.set_all_signals_blocked(false);

        _visible_check_button.set_all_signals_blocked(true);
        _visible_check_button.set_is_checked(layer.is_visible);
        _visible_check_button.set_all_signals_blocked(false);

        _locked_toggle_button.set_all_signals_blocked(true);
        _locked_toggle_button.set_active(layer.is_locked);
        _locked_toggle_button.set_child(layer.is_locked ? &_locked_toggle_button_icon_locked : &_locked_toggle_button_icon_not_locked);
        _locked_toggle_button.set_all_signals_blocked(false);

        _locked_check_button.set_all_signals_blocked(true);
        _locked_check_button.set_is_checked(layer.is_locked);
        _locked_check_button.set_all_signals_blocked(false);

        _opacity_scale.set_all_signals_blocked(true);
        _opacity_scale.set_value(layer.opacity);
        _opacity_scale.set_all_signals_blocked(false);
    }

    LayerView::LayerControlBar::LayerControlBar(size_t layer_i)
        : _layer(layer_i)
    {
        auto icon_size = _locked_toggle_button_icon_locked.get_size();

        for (auto* icon: {&_locked_toggle_button_icon_locked, &_locked_toggle_button_icon_not_locked,
                          &_visible_toggle_button_icon_visible, &_visible_toggle_button_icon_not_visible})
        {
            icon->set_size_request(icon_size);
            icon->set_expand(false);
        }

        for (auto* button: {&_visible_toggle_button, &_locked_toggle_button})
        {
            button->set_has_frame(false);
            button->set_size_request(icon_size);
            button->set_expand(false);
            _visible_locked_indicator_box.push_back(button);
        }

        _locked_toggle_button.connect_signal_toggled(on_locked_toggle_button_toggled, this);
        _visible_toggle_button.connect_signal_toggled(on_visible_toggle_button_toggled, this);

        _name_box.push_back(&_name_label);
        _name_box.push_back(&_name_separator);
        _name_box.push_back(&_name_entry);
        _name_entry.set_text(state::layers.at(_layer).name);
        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_hexpand(true);
        _name_entry.connect_signal_activate(on_name_entry_activate, this);

        auto* entry_natural = gtk_requisition_new();
        auto* button_natural = gtk_requisition_new();

        gtk_widget_get_preferred_size(_name_entry.operator GtkWidget*(), nullptr, entry_natural);
        gtk_widget_get_preferred_size(_visible_check_button.operator GtkWidget*(), nullptr, button_natural);

        float right_margin = entry_natural->width * 0.5 - button_natural->width * 0.5;

        gtk_requisition_free(entry_natural);
        gtk_requisition_free(button_natural);

        _visible_box.push_back(&_visible_label);
        _visible_box.push_back(&_visible_separator);
        _visible_box.push_back(&_visible_check_button);
        _visible_check_button.set_halign(GTK_ALIGN_END);
        _visible_check_button.set_hexpand(true);
        _visible_check_button.set_margin_end(right_margin);
        _visible_check_button.connect_signal_toggled(on_visible_check_button_toggled, this);

        _locked_box.push_back(&_locked_label);
        _locked_box.push_back(&_locked_separator);
        _locked_box.push_back(&_locked_check_button);
        _locked_check_button.set_halign(GTK_ALIGN_END);
        _locked_check_button.set_hexpand(true);
        _locked_check_button.set_margin_end(right_margin);
        _locked_check_button.connect_signal_toggled(on_locked_check_button_toggled, this);

        _opacity_box.push_back(&_opacity_label);
        _opacity_box.push_back(&_opacity_separator);
        _opacity_box.push_back(&_opacity_scale);
        _opacity_scale.set_hexpand(true);
        _opacity_scale.set_value(state::layers.at(_layer).opacity);
        _opacity_scale.connect_signal_value_changed(on_opacity_scale_value_changed, this);

        auto add_dropdown_item = [&](BlendMode mode) -> void {

            std::string label;
            std::string list_item;
            std::string label_tooltip;
            std::string list_tooltip;

            if (mode == BlendMode::NORMAL)
            {
                list_item = "Normal";
                label = "Normal";
                label_tooltip = "Blend Mode: Alpha";
                list_tooltip = "<b>Alpha Blending</b>";
            }
            else if (mode == BlendMode::ADD)
            {
                list_item = "Add";
                label = "Add";
                label_tooltip = "Blend Mode: Additive";
                list_tooltip = "<b>Additive Blending</b>\n<tt>color = source.rgba + destination.rgba</tt>";
            }
            else if (mode == BlendMode::SUBTRACT)
            {
                list_item = "Subtract";
                label = "Subtract";
                label_tooltip = "Blend Mode: Subtractive";
                list_tooltip = "<b>Subtractive Blending</b>\n<tt>color = destination.rgba - source.rgba</tt>";
            }
            else if (mode == BlendMode::REVERSE_SUBTRACT)
            {
                list_item = "Reverse Subtract";
                label = "Reverse Subtract";
                label_tooltip = "Blend Mode: Subtractive Reversed";
                list_tooltip = "<b>Reversed Subtractive Blending</b>\n<tt>color = source.rgba - destination.rgba</tt>";
            }
            else if (mode == BlendMode::MULTIPLY)
            {
                list_item = "Multiply";
                label = "Multiply";
                label_tooltip = "Blend Mode: Multiplicative";
                list_tooltip = "<b>Multiplicative Blending</b>\n<tt>color = source.rgba * destination.rgba</tt>";
            }
            else if (mode == BlendMode::MIN)
            {
                list_item = "Minimum";
                label = "Min";
                label_tooltip = "Blend Mode: Minimum";
                list_tooltip = "<b>Minimum Blending</b>\n<tt>color = min(source.rgba, destination.rgba)</tt>";
            }
            else if (mode == BlendMode::MAX)
            {
                list_item = "Maximum";
                label = "Max";
                label_tooltip = "Blend Mode: Maximum";
                list_tooltip = "<b>Maximum Blending</b>\n<tt>color = max(source.rgba, destination.rgba)</tt>";
            }

            _blend_mode_dropdown_list_items.emplace_back(list_item);
            _blend_mode_dropdown_list_items.back().set_tooltip_text(list_tooltip);
            _blend_mode_dropdown_list_items.back().set_halign(GTK_ALIGN_START);

            _blend_mode_dropdown_label_items.emplace_back(label);
            _blend_mode_dropdown_label_items.back().set_tooltip_text(label_tooltip);

            _blend_mode_dropdown.push_back(
                    &_blend_mode_dropdown_list_items.back(),
                    &_blend_mode_dropdown_label_items.back(),
                    on_blend_mode_select,
                    new on_blend_mode_select_data{mode, this}
            );
        };

        _blend_mode_dropdown_label_items.reserve(_blend_modes_in_order.size());
        _blend_mode_dropdown_list_items.reserve(_blend_modes_in_order.size());

        for (auto mode: _blend_modes_in_order)
            add_dropdown_item(mode);

        _blend_mode_box.push_back(&_blend_mode_label);
        _blend_mode_box.push_back(&_blend_mode_separator);
        _blend_mode_box.push_back(&_blend_mode_dropdown);
        _blend_mode_dropdown.set_hexpand(true);
        _blend_mode_dropdown.set_halign(GTK_ALIGN_CENTER);

        for (auto* box: {&_name_box, &_opacity_box, &_visible_box, &_locked_box, &_blend_mode_box})
            _menu_button_popover_box.push_back(box);

        _menu_button_popover_box.set_homogeneous(true);
        _menu_button_popover.set_child(&_menu_button_popover_box);
        _menu_button.set_popover(&_menu_button_popover);
        _menu_button.set_has_frame(false);

        _menu_button_title_label.set_text(state::layers.at(_layer).name);
        _menu_button_title_label.set_halign(GTK_ALIGN_START);
        _menu_button_title_label.set_hexpand(false);
        _menu_button_title_label_viewport.set_child(&_menu_button_title_label);
        _menu_button_title_label_viewport.set_size_request({10 * state::margin_unit, 0});
        _menu_button_title_label_viewport.set_has_frame(false);
        _menu_button_title_label_viewport.set_policy(GTK_POLICY_EXTERNAL, GTK_POLICY_NEVER);
        _menu_button.set_child(&_menu_button_title_label_viewport);
        _menu_button.set_hexpand(false);

        _main.push_back(&_visible_locked_indicator_box);
        _main.push_back(&_menu_button);
    }

    LayerView::LayerControlBar::operator Widget*()
    {
        return &_main;
    }

    void LayerView::LayerControlBar::on_locked_toggle_button_toggled(ToggleButton* button, LayerControlBar* instance)
    {
        auto& layer = state::layers.at(instance->_layer);
        layer.is_locked = button->get_active();

        instance->update();
    }

    void LayerView::LayerControlBar::on_visible_toggle_button_toggled(ToggleButton* button, LayerControlBar* instance)
    {
        auto& layer = state::layers.at(instance->_layer);
        layer.is_visible = button->get_active();

        instance->update();
    }

    void LayerView::LayerControlBar::on_name_entry_activate(Entry* entry, LayerControlBar* instance)
    {
        auto& layer = state::layers.at(instance->_layer);
        layer.name = entry->get_text();

        instance->update();
    }

    void LayerView::LayerControlBar::on_locked_check_button_toggled(CheckButton* button, LayerControlBar* instance)
    {
        auto& layer = state::layers.at(instance->_layer);
        layer.is_locked = button->get_is_checked();

        instance->update();
    }

    void LayerView::LayerControlBar::on_visible_check_button_toggled(CheckButton* button, LayerControlBar* instance)
    {
        auto& layer = state::layers.at(instance->_layer);
        layer.is_visible = button->get_is_checked();

        instance->update();
    }

    void LayerView::LayerControlBar::on_opacity_scale_value_changed(Scale* scale, LayerControlBar* instance)
    {
        auto& layer = state::layers.at(instance->_layer);
        layer.opacity = scale->get_value();

        instance->update();
    }

    void LayerView::LayerControlBar::on_blend_mode_select(on_blend_mode_select_data* data)
    {
        auto& layer = state::layers.at(data->instance->_layer);
        layer.blend_mode = data->blend_mode;
    }

    LayerView::LayerView()
    {
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            _layer_rows.push_back(LayerRow{
                ListView(GTK_ORIENTATION_HORIZONTAL),
                new LayerControlBar(layer_i),
                std::deque<LayerFrameDisplay>()
            });

            auto& row = _layer_rows.back();
            row.list.push_back(row.control_bar->operator Widget*());

            for (size_t frame_i = 0; frame_i < state::n_frames; ++frame_i)
            {
                row.frame_display.emplace_back(layer_i, frame_i);
                row.list.push_back(row.frame_display.back());
            }

            _layer_row_list_view.push_back(&row.list);
        }

        for (size_t frame_i = 0; frame_i < state::n_frames; ++frame_i)
        {
            _whole_frame_displays.emplace_back(frame_i);
            _whole_frame_display_list_view.push_back(_whole_frame_displays.back());
        }

        _whole_frame_display_list_view.set_halign(GTK_ALIGN_END);

        _main.push_back(&_whole_frame_display_list_view);
        _main.push_back(&_layer_row_list_view);

        update();
    }

    void LayerView::update()
    {
        for (auto& row : _layer_rows)
        {
            row.control_bar->update();
            for (auto& display : row.frame_display)
                display.update();
        }

        for (auto& display : _whole_frame_displays)
            display.update();
    }

    LayerView::operator Widget*()
    {
        return &_main;
    }
}