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
            ListView _whole_frame_row_inner = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
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

            struct LayerRow;
            struct LayerControlBar
            {
                LayerControlBar(size_t layer_i, LayerView*);
                operator Widget*();

                size_t _layer;
                LayerView* _owner;
                LayerRow* _holder;

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

                Label _name_label = Label("Name: ");
                SeparatorLine _name_separator;
                Entry _name_entry;
                Box _name_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_name_entry_activate(Entry*, LayerControlBar* instance);

                Label _visible_label = Label("Visible:");
                SeparatorLine _visible_separator;
                CheckButton _visible_check_button;
                Box _visible_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_visible_check_button_toggled(CheckButton*, LayerControlBar* instance);

                Label _locked_label = Label("Locked: ");
                SeparatorLine _locked_separator;
                CheckButton _locked_check_button;
                Box _locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_locked_check_button_toggled(CheckButton*, LayerControlBar* instance);

                Label _opacity_label = Label("Opacity: ");
                SeparatorLine _opacity_separator;
                Scale _opacity_scale = Scale(0, 1, 0.01);
                Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_opacity_scale_value_changed(Scale*, LayerControlBar* instance);

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

                using on_blend_mode_select_data = struct {BlendMode blend_mode; LayerControlBar* instance;};
                static void on_blend_mode_select(on_blend_mode_select_data*);

                Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            void notify_layer_locked(bool locked, size_t layer_i);
            void notify_layer_visible(bool visible, size_t layer_i);
            void notify_layer_renamed(std::string new_name, size_t layer_i);
            void notify_layer_opacity_changed(float opacity, size_t layer_i);
            void notify_layer_blend_mode_changed(BlendMode, size_t layer_i);

            struct LayerRow
            {
                LayerRow(size_t layer_i, LayerView*);
                operator Widget*();

                Layer* _layer;

                std::deque<LayerFrameDisplay> _layer_frame_displays;
                LayerControlBar _control_bar;
                ListView _layer_frame_row = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
            };

            std::deque<LayerRow> _layer_rows;
            ListView _layer_rows_list = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);

            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            static void on_whole_frame_row_selection_changed(SelectionModel*, int position, int n_items, LayerView* instance);

            using on_layer_row_frame_selection_changed_data = struct { LayerView* instance; size_t row_index; };
            static void on_layer_row_frame_selection_changed(SelectionModel*, int position, int n_items, on_layer_row_frame_selection_changed_data* instance);

            void set_selected(size_t layer_i, size_t frame_i);
    };
}

// ###

namespace mousetrap
{
    LayerView::LayerControlBar::LayerControlBar(size_t layer_i, LayerView* owner)
        : _layer(layer_i), _menu_button_title_label(state::layers.at(layer_i).name), _owner(owner)
    {
        auto* layer = &state::layers.at(layer_i);

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
    }

    LayerView::LayerControlBar::operator Widget*()
    {
        return &_main;
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

    void LayerView::LayerControlBar::on_locked_toggle_button_toggled(ToggleButton* button, LayerControlBar* instance)
    {
        instance->_owner->notify_layer_locked(button->get_active(), instance->_layer);
    }

    void LayerView::LayerControlBar::on_locked_check_button_toggled(CheckButton* button, LayerControlBar* instance)
    {
        instance->_owner->notify_layer_locked(button->get_is_checked(), instance->_layer);
    }

    void LayerView::notify_layer_locked(bool locked, size_t row_i)
    {
        if (_layer_rows.size() <= row_i)
            return;

        auto* row = &_layer_rows.at(row_i);

        row->_control_bar._locked_toggle_button.set_all_signals_blocked(true);
        row->_control_bar._locked_check_button.set_all_signals_blocked(true);

        row->_control_bar._locked_check_button.set_is_checked(locked);
        row->_control_bar._locked_toggle_button.set_active(locked);

        if (locked)
            row->_control_bar._locked_toggle_button.set_child(&row->_control_bar._locked_toggle_button_icon_locked);
        else
            row->_control_bar._locked_toggle_button.set_child(&row->_control_bar._locked_toggle_button_icon_not_locked);

        row->_layer->is_locked = locked;

        row->_control_bar._locked_toggle_button.set_all_signals_blocked(false);
        row->_control_bar._locked_check_button.set_all_signals_blocked(false);
    }

    void LayerView::LayerControlBar::on_visible_toggle_button_toggled(ToggleButton* button, LayerControlBar* instance)
    {
        instance->_owner->notify_layer_visible(button->get_active(), instance->_layer);
    }

    void LayerView::LayerControlBar::on_visible_check_button_toggled(CheckButton* button, LayerControlBar* instance)
    {
        instance->_owner->notify_layer_visible(button->get_is_checked(), instance->_layer);
    }

    void LayerView::notify_layer_visible(bool visible, size_t row_i)
    {
        if (_layer_rows.size() <= row_i)
            return;

        auto* row = &_layer_rows.at(row_i);

        row->_control_bar._visible_toggle_button.set_all_signals_blocked(true);
        row->_control_bar._visible_check_button.set_all_signals_blocked(true);

        row->_control_bar._visible_check_button.set_is_checked(visible);
        row->_control_bar._visible_toggle_button.set_active(visible);

        if (visible)
            row->_control_bar._visible_toggle_button.set_child(&row->_control_bar._visible_toggle_button_icon_visible);
        else
            row->_control_bar._visible_toggle_button.set_child(&row->_control_bar._visible_toggle_button_icon_not_visible);

        row->_layer->is_visible = visible;

        row->_control_bar._visible_toggle_button.set_all_signals_blocked(false);
        row->_control_bar._visible_check_button.set_all_signals_blocked(false);
    }

    void LayerView::LayerControlBar::on_name_entry_activate(Entry* entry, LayerControlBar* instance)
    {
        auto text = entry->get_text();
        if (text.empty())
            text = instance->_menu_button_title_label.get_text();

        instance->_owner->notify_layer_renamed(text, instance->_layer);
    }

    void LayerView::notify_layer_renamed(std::string new_name, size_t row_i)
    {
        if (_layer_rows.size() <= row_i)
            return;

        auto* row = &_layer_rows.at(row_i);

        row->_control_bar._name_entry.set_all_signals_blocked(true);
        row->_control_bar._name_entry.set_text(new_name);
        row->_control_bar._menu_button_title_label.set_text(new_name);
        row->_layer->name = new_name;
        row->_control_bar._menu_button_title_label_viewport.set_tooltip_text(new_name);
        row->_control_bar._name_entry.set_all_signals_blocked(false);
    }

    void LayerView::LayerControlBar::on_opacity_scale_value_changed(Scale* scale, LayerControlBar* instance)
    {
        instance->_owner->notify_layer_opacity_changed(scale->get_value(), instance->_layer);
    }

    void LayerView::notify_layer_opacity_changed(float opacity, size_t row_i)
    {
        if (_layer_rows.size() <= row_i)
            return;

        auto* row = &_layer_rows.at(row_i);

        row->_control_bar._opacity_scale.set_all_signals_blocked(true);

        row->_control_bar._opacity_scale.set_value(opacity);
        row->_layer->opacity = opacity;

        row->_control_bar._opacity_scale.set_all_signals_blocked(false);
    }

    void LayerView::LayerControlBar::on_blend_mode_select(on_blend_mode_select_data* data)
    {
        data->instance->_owner->notify_layer_blend_mode_changed(data->blend_mode, data->instance->_layer);
    }

    void LayerView::notify_layer_blend_mode_changed(BlendMode mode, size_t row_i)
    {
        if (_layer_rows.size() <= row_i)
            return;

        auto* row = &_layer_rows.at(row_i);

        row->_control_bar._blend_mode_dropdown.set_all_signals_blocked(true);
        size_t index = 0;
        for (; index < row->_control_bar._blend_modes_in_order.size(); ++index)
            if (row->_control_bar._blend_modes_in_order.at(index) == mode)
                break;

        row->_layer->blend_mode = mode;
        row->_control_bar._blend_mode_dropdown.set_selected(index);
        row->_control_bar._blend_mode_dropdown.set_all_signals_blocked(false);
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

    LayerView::LayerRow::LayerRow(size_t layer_i, LayerView* owner)
        : _layer(&state::layers.at(layer_i)), _control_bar(layer_i, owner)
    {
        _control_bar._holder = this;
        _layer_frame_row.push_back(_control_bar);

        for (size_t i = 0; i < state::n_frames; ++i)
        {
            _layer_frame_displays.emplace_back(_layer, i);
            _layer_frame_row.push_back(_layer_frame_displays.back());
        }
    }

    LayerView::LayerRow::operator Widget*()
    {
        return &_layer_frame_row;
    }

    void LayerView::on_whole_frame_row_selection_changed(SelectionModel*, int position, int n_items, LayerView* instance)
    {
        size_t layer_i = instance->_layer_rows_list.get_selection_model()->get_selection().at(0);
        instance->set_selected(layer_i, position);
    }

    void LayerView::on_layer_row_frame_selection_changed(SelectionModel*, int position, int n_items, on_layer_row_frame_selection_changed_data* data)
    {
        auto* instance = data->instance;
        auto row_i = data->row_index;

        instance->set_selected(row_i, position);
    }

    void LayerView::set_selected(size_t layer_i, size_t frame_i)
    {
        for (size_t i = 0; i < _layer_rows.size(); ++i)
        {
            auto* model = _layer_rows.at(i)._layer_frame_row.get_selection_model();

            model->set_all_signals_blocked(true);

            for (auto i : model->get_selection())
                model->unselect(i);

            if (i == layer_i)
                model->select(frame_i);

            model->set_all_signals_blocked(false);
        }
        _layer_rows_list.get_selection_model()->select(layer_i, true);

        auto* whole_frame_model = _whole_frame_row_inner.get_selection_model();
        whole_frame_model->set_all_signals_blocked(true);
        whole_frame_model->select(frame_i);
        whole_frame_model->set_all_signals_blocked(false);

        state::current_frame = frame_i;
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
        _whole_frame_row_inner.get_selection_model()->connect_signal_selection_changed(on_whole_frame_row_selection_changed, this);

        _whole_frame_row.push_back(&_whole_frame_row_inner);
        _whole_frame_row.set_halign(GTK_ALIGN_END);
        _whole_frame_row.set_cursor(GtkCursorType::POINTER);

        for (size_t i = 0; i < state::layers.size(); ++i)
        {
           _layer_rows.push_back(LayerRow(i, this));
           _layer_rows.back().operator Widget*()->set_halign(GTK_ALIGN_END);
           _layer_rows.back()._layer_frame_row.set_show_separators(true);
           _layer_rows.back()._layer_frame_row.get_selection_model()->connect_signal_selection_changed(on_layer_row_frame_selection_changed, new on_layer_row_frame_selection_changed_data{this, _layer_rows.size()-1});
           _layer_rows_list.push_back(_layer_rows.back());
        }

        _layer_rows_list.set_cursor(GtkCursorType::POINTER);

        _main.push_back(&_whole_frame_row);
        _main.push_back(&_layer_rows_list);
    }
}