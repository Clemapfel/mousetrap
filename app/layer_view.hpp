// 
// Copyright 2022 Clemens Cords
// Created on 9/28/22 by clem (mail@clemens-cords.com)
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

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class LayerView : public AppComponent
    {
        public:
            LayerView();

            operator Widget*() override;

            void update() override
            {};

        private:
            static inline float thumbnail_height = 50;

            struct LayerFrameDisplay
            {
                LayerFrameDisplay(Layer*, size_t frame_i, LayerView* owner);
                operator Widget*();

                size_t _frame_index;
                Layer* _layer;
                LayerView* _owner;

                static inline Shader* _transparency_tiling_shader = nullptr;
                Shape* _transparency_tiling_shape;
                Shape* _layer_shape;

                Vector2f _canvas_size;
                GLArea _gl_area;

                AspectFrame _aspect_frame;

                static void on_gl_area_realize(Widget*, LayerFrameDisplay*);
                static void on_gl_area_resize(GLArea*, int, int, LayerFrameDisplay*);
            };

            struct WholeFrameDisplay
            {
                WholeFrameDisplay(size_t frame_i, LayerView* owner);
                operator Widget*();

                size_t _frame;
                LayerView* _owner;

                static inline Shader* _transparency_tiling_shader = nullptr;
                Shape* _transparency_tiling_shape;
                GLArea _transparency_area;
                Vector2f _canvas_size;
                static void on_transparency_area_realize(Widget*, WholeFrameDisplay*);
                static void on_transparency_area_resize(GLArea*, int, int, WholeFrameDisplay*);

                GLArea _layer_area;
                std::vector<Shape*> _layer_shapes;
                static void on_layer_area_realize(Widget*, WholeFrameDisplay*);

                AspectFrame _aspect_frame;
                Overlay _overlay;
            };

            struct LayerRow
            {
                LayerRow(Layer*, LayerView* _owner);

                Layer* _layer;
                LayerView* _owner;

                ImageDisplay _locked_toggle_button_icon_locked = ImageDisplay(get_resource_path() + "icons/layer_locked.png", state::icon_scale);
                ImageDisplay _locked_toggle_button_icon_not_locked = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png", state::icon_scale);
                ToggleButton _locked_toggle_button;
                static void on_locked_toggle_button_toggled(ToggleButton*, LayerRow*);

                ImageDisplay _visible_toggle_button_icon_visible = ImageDisplay(get_resource_path() + "icons/layer_visible.png", state::icon_scale);
                ImageDisplay _visible_toggle_button_icon_not_visible = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png", state::icon_scale);
                ToggleButton _visible_toggle_button;
                static void on_visible_toggle_button_toggled(ToggleButton*, LayerRow*);

                Box _visible_locked_indicator_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit * 0.5);

                Label _menu_button_title_label;
                ScrolledWindow _menu_button_title_label_viewport;
                MenuButton _menu_button;
                Popover _menu_button_popover;
                Box _menu_button_popover_box = Box (GTK_ORIENTATION_VERTICAL);

                Label _name_label = Label("Name: ");
                SeparatorLine _name_separator;
                Entry _name_entry;
                Box _name_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_name_entry_activate(Entry*, LayerRow* instance);

                Label _visible_label = Label("Visible:");
                SeparatorLine _visible_separator;
                CheckButton _visible_check_button;
                Box _visible_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_visible_check_button_toggled(CheckButton*, LayerRow* instance);

                Label _locked_label = Label("Locked: ");
                SeparatorLine _locked_separator;
                CheckButton _locked_check_button;
                Box _locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_locked_check_button_toggled(CheckButton*, LayerRow* instance);

                Label _opacity_label = Label("Opacity: ");
                SeparatorLine _opacity_separator;
                Scale _opacity_scale = Scale(0, 1, 0.01);
                Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_opacity_scale_value_changed(Scale*, LayerRow* instance);

                Label _blend_mode_label = Label("Blend: ");
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

                using on_blend_mode_select_data = struct {BlendMode blend_mode; LayerRow* instance;};
                static void on_blend_mode_select(on_blend_mode_select_data*);

                ListView _main = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

                Label _index_label;
                std::vector<LayerFrameDisplay*> _layers;
            };

            std::vector<LayerRow*> _rows;
            ReorderableListView _row_list = ReorderableListView(GTK_ORIENTATION_VERTICAL);

            static void notify_layer_locked(bool locked, LayerRow*);
            static void notify_layer_visible(bool visible, LayerRow*);
            static void notify_layer_renamed(std::string new_name, LayerRow*);
            static void notify_layer_opacity_changed(float opacity, LayerRow*);
            static void notify_layer_blend_mode_changed(BlendMode, LayerRow*);

            std::vector<WholeFrameDisplay*> _header_frames;
            ListView _header_list = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}

namespace mousetrap
{
    /// LAYER FRAME DISPLAY

    void LayerView::LayerFrameDisplay::on_gl_area_realize(Widget* widget, LayerFrameDisplay* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_tiling_shader == nullptr)
        {
            _transparency_tiling_shader = new Shader();
            _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag",
                                                          ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});
        instance->_canvas_size = Vector2f(1, 1);

        instance->_layer_shape = new Shape();
        instance->_layer_shape->as_rectangle({0, 0}, {1, 1});
        instance->_layer_shape->set_texture(&instance->_layer->frames.at(instance->_frame_index).texture);

        auto task = RenderTask(instance->_transparency_tiling_shape, _transparency_tiling_shader);
        task.register_vec2("_canvas_size", &instance->_canvas_size);

        instance->_gl_area.add_render_task(task);
        instance->_gl_area.add_render_task(instance->_layer_shape);

        instance->_gl_area.set_size_request({state::layer_resolution.x * (state::layer_resolution.x / LayerView::thumbnail_height), LayerView::thumbnail_height});
    }

    void LayerView::LayerFrameDisplay::on_gl_area_resize(GLArea*, int w, int h, LayerFrameDisplay* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_gl_area.queue_render();
    }

    LayerView::LayerFrameDisplay::LayerFrameDisplay(Layer* layer, size_t frame_i, LayerView* owner)
            : _layer(layer), _frame_index(frame_i),
              _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y)), _owner(owner)
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);

        _aspect_frame.set_child(&_gl_area);
    }

    LayerView::LayerFrameDisplay::operator Widget*()
    {
        return &_aspect_frame;
    }

    // LAYER ROW

    void LayerView::LayerRow::on_locked_toggle_button_toggled(ToggleButton* button, LayerRow* instance)
    {
        LayerView::notify_layer_locked(button->get_active(), instance);
    }

    void LayerView::LayerRow::on_locked_check_button_toggled(CheckButton* button, LayerRow* instance)
    {
        LayerView::notify_layer_locked(button->get_is_checked(), instance);
    }

    void LayerView::notify_layer_locked(bool locked, LayerRow* row)
    {
        row->_locked_toggle_button.set_all_signals_blocked(true);
        row->_locked_check_button.set_all_signals_blocked(true);

        row->_locked_check_button.set_is_checked(locked);
        row->_locked_toggle_button.set_active(locked);

        if (locked)
            row->_locked_toggle_button.set_child(&row->_locked_toggle_button_icon_locked);
        else
            row->_locked_toggle_button.set_child(&row->_locked_toggle_button_icon_not_locked);

        row->_layer->is_locked = locked;

        row->_locked_toggle_button.set_all_signals_blocked(false);
        row->_locked_check_button.set_all_signals_blocked(false);
    }

    void LayerView::LayerRow::on_visible_toggle_button_toggled(ToggleButton* button, LayerRow* instance)
    {
        LayerView::notify_layer_visible(button->get_active(), instance);
    }

    void LayerView::LayerRow::on_visible_check_button_toggled(CheckButton* button, LayerRow* instance)
    {
        LayerView::notify_layer_visible(button->get_is_checked(), instance);
    }

    void LayerView::notify_layer_visible(bool visible, LayerRow* row)
    {
        row->_visible_toggle_button.set_all_signals_blocked(true);
        row->_visible_check_button.set_all_signals_blocked(true);

        row->_visible_check_button.set_is_checked(visible);
        row->_visible_toggle_button.set_active(visible);

        if (visible)
            row->_visible_toggle_button.set_child(&row->_visible_toggle_button_icon_visible);
        else
            row->_visible_toggle_button.set_child(&row->_visible_toggle_button_icon_not_visible);

        row->_layer->is_visible = visible;

        row->_visible_toggle_button.set_all_signals_blocked(false);
        row->_visible_check_button.set_all_signals_blocked(false);
    }

    void LayerView::LayerRow::on_name_entry_activate(Entry* entry, LayerRow* instance)
    {
        auto text = entry->get_text();
        if (text.empty())
            text = instance->_menu_button_title_label.get_text();

        LayerView::notify_layer_renamed(text, instance);
    }

    void LayerView::notify_layer_renamed(std::string new_name, LayerRow* row)
    {
        row->_name_entry.set_all_signals_blocked(true);
        row->_name_entry.set_text(new_name);
        row->_menu_button_title_label.set_text(new_name);
        row->_layer->name = new_name;
        row->_menu_button_title_label_viewport.set_tooltip_text(new_name);
        row->_name_entry.set_all_signals_blocked(false);
    }

    void LayerView::LayerRow::on_opacity_scale_value_changed(Scale* scale, LayerRow* instance)
    {
        LayerView::notify_layer_opacity_changed(scale->get_value(), instance);
    }

    void LayerView::notify_layer_opacity_changed(float opacity, LayerRow* row)
    {
        row->_opacity_scale.set_all_signals_blocked(true);

        row->_opacity_scale.set_value(opacity);
        // TODO: change icon opacity
        row->_layer->opacity = opacity;

        row->_opacity_scale.set_all_signals_blocked(false);
    }

    void LayerView::LayerRow::on_blend_mode_select(on_blend_mode_select_data* data)
    {
        LayerView::notify_layer_blend_mode_changed(data->blend_mode, data->instance);
    }

    void LayerView::notify_layer_blend_mode_changed(BlendMode mode, LayerRow* row)
    {
        row->_blend_mode_dropdown.set_all_signals_blocked(true);
        size_t index = 0;
        for (; index < row->_blend_modes_in_order.size(); ++index)
            if (row->_blend_modes_in_order.at(index) == mode)
                break;

        row->_layer->blend_mode = mode;
        row->_blend_mode_dropdown.set_selected(index);
        row->_blend_mode_dropdown.set_all_signals_blocked(false);
    }

    LayerView::LayerRow::LayerRow(Layer* layer, LayerView* owner)
        : _layer(layer), _menu_button_title_label(layer->name), _owner(owner)
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

        _locked_toggle_button.set_active(true);
        _locked_toggle_button.set_active(false);
        _visible_toggle_button.set_active(true);

        _name_box.push_back(&_name_label);
        _name_box.push_back(&_name_separator);
        _name_box.push_back(&_name_entry);
        _name_entry.set_text(layer->name);
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
        _opacity_scale.set_value(layer->opacity);
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

        _menu_button_title_label.set_text(layer->name);
        _menu_button_title_label.set_halign(GTK_ALIGN_START);
        _menu_button_title_label.set_hexpand(false);
        _menu_button_title_label_viewport.set_child(&_menu_button_title_label);
        _menu_button_title_label_viewport.set_size_request({10 * state::margin_unit, 0});
        _menu_button_title_label_viewport.set_has_frame(false);
        _menu_button_title_label_viewport.set_policy(GTK_POLICY_EXTERNAL, GTK_POLICY_NEVER);
        _menu_button.set_child(&_menu_button_title_label_viewport);

        _main.push_back(&_visible_locked_indicator_box);
        _main.push_back(&_menu_button);
        _menu_button.set_hexpand(false);
        _main.set_show_separators(true);
    }

    // WHOLE FRAME DISPLAY

    LayerView::WholeFrameDisplay::WholeFrameDisplay(size_t frame_i, LayerView* owner)
        : _frame(frame_i), _owner(owner), _aspect_frame(state::layer_resolution.x / float(state::layer_resolution.y))
    {
        _transparency_area.connect_signal_realize(on_transparency_area_realize, this);
        _transparency_area.connect_signal_resize(on_transparency_area_resize, this);
        _layer_area.connect_signal_realize(on_layer_area_realize, this);

        for (auto* area : {&_transparency_area, &_layer_area})
            area->set_size_request({state::layer_resolution.x * (state::layer_resolution.x / LayerView::thumbnail_height), LayerView::thumbnail_height});

        _overlay.set_child(&_transparency_area);
        _overlay.add_overlay(&_layer_area);
        _aspect_frame.set_child(&_overlay);
    }

    void LayerView::WholeFrameDisplay::on_transparency_area_realize(Widget* widget, WholeFrameDisplay* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_tiling_shader == nullptr)
        {
            _transparency_tiling_shader = new Shader();
            _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag",
                                                          ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});
        instance->_canvas_size = Vector2f(1, 1);

        auto task = RenderTask(instance->_transparency_tiling_shape, _transparency_tiling_shader);
        task.register_vec2("_canvas_size", &instance->_canvas_size);

        instance->_transparency_area.add_render_task(task);
    }

    void LayerView::WholeFrameDisplay::on_transparency_area_resize(GLArea*, int w, int h, WholeFrameDisplay* instance)
    {
        instance->_canvas_size = {w, h};
    }

    void LayerView::WholeFrameDisplay::on_layer_area_realize(Widget* widget, WholeFrameDisplay* display)
    {

    }

    LayerView::WholeFrameDisplay::operator Widget*()
    {
        return &_aspect_frame;
    }

    // LAYER VIEW

    LayerView::LayerView()
    {
        for (auto& layer : state::layers)
        {
            _rows.emplace_back(new LayerRow(&layer, this));

            auto* row = _rows.back();
            row->_layers.reserve(state::layers.size());

            for (size_t i = 0; i < row->_layer->frames.size(); ++i)
            {
                row->_layers.emplace_back(new LayerFrameDisplay(row->_layer, i, this));
                row->_main.push_back(row->_layers.back()->operator Widget*());
                row->_main.set_show_separators(true);
                row->_main.set_halign(GTK_ALIGN_END);
            }
            _row_list.push_back(&_rows.back()->_main);
        }
        _row_list.set_show_separators(true);

        for (size_t i = 0; i < state::n_frames; ++i)
        {
            _header_frames.emplace_back(new WholeFrameDisplay(i, this));
            _header_list.push_back(_header_frames.back()->operator Widget*());
        }
        _header_list.set_show_separators(true);

        auto* list = new ListView(GTK_ORIENTATION_HORIZONTAL);
        list->push_back(&_header_list);
        list->set_show_separators(true);
        list->set_halign(GTK_ALIGN_END);

        _main.push_back(list);
        _main.push_back(&_row_list);
    }

    LayerView::operator Widget*()
    {
        return &_main;
    }
}

