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
#include <include/frame.hpp>

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
            static inline const float thumbnail_margin_top_bottom = 0.5 * state::margin_unit;
            static inline const float thumbnail_margin_left_right = 0.25 * state::margin_unit;
            static inline Shader* transparency_tiling_shader = nullptr;

            struct WholeFrameDisplay
            {
                WholeFrameDisplay(size_t frame_i);
                ~WholeFrameDisplay();

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
                Frame _frame_widget = Frame();

                void generate_tooltip() {
                    _label.set_tooltip_text("Frame #" + std::to_string(_frame));
                    _overlay.set_tooltip_text("Composite for Frame #" + std::to_string(_frame));
                }
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
                Shape* _frame_shape;

                static inline Texture* selection_indicator_texture = nullptr;

                AspectFrame _aspect_frame;

                ImageDisplay _layer_frame_active_icon = ImageDisplay(get_resource_path() + "icons/layer_frame_active.png");
                Overlay _overlay;
                Frame _frame_widget = Frame();
                ListView _main = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

                void generate_tooltip() {
                    _frame_widget.set_tooltip_text("Layer \"" + state::layers.at(_layer)->name + "\", Frame #" + std::to_string(_frame));
                }
            };

            struct LayerPropertyOptions
            {
                LayerPropertyOptions(size_t layer_i, LayerView*);
                operator Widget*();

                void update();

                LayerView* _owner;
                size_t _layer;
                Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                ImageDisplay _locked_toggle_button_icon_locked = ImageDisplay(get_resource_path() + "icons/layer_locked.png", state::icon_scale);
                ImageDisplay _locked_toggle_button_icon_not_locked = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png", state::icon_scale);
                ToggleButton _locked_toggle_button;
                static void on_locked_toggle_button_toggled(ToggleButton*, LayerPropertyOptions*);

                ImageDisplay _visible_toggle_button_icon_visible = ImageDisplay(get_resource_path() + "icons/layer_visible.png", state::icon_scale);
                ImageDisplay _visible_toggle_button_icon_not_visible = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png", state::icon_scale);
                ToggleButton _visible_toggle_button;
                static void on_visible_toggle_button_toggled(ToggleButton*, LayerPropertyOptions*);

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
                static void on_name_entry_activate(Entry*, LayerPropertyOptions* instance);

                Label _visible_label = Label("Visible");
                SeparatorLine _visible_separator;
                CheckButton _visible_check_button;
                Box _visible_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_visible_check_button_toggled(CheckButton*, LayerPropertyOptions* instance);

                Label _locked_label = Label("Locked");
                SeparatorLine _locked_separator;
                CheckButton _locked_check_button;
                Box _locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_locked_check_button_toggled(CheckButton*, LayerPropertyOptions* instance);

                Label _opacity_label = Label("Opacity");
                SeparatorLine _opacity_separator;
                Scale _opacity_scale = Scale(0, 1, 0.01);
                Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
                static void on_opacity_scale_value_changed(Scale*, LayerPropertyOptions* instance);

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

                using on_blend_mode_select_data = struct {BlendMode blend_mode; LayerPropertyOptions* instance;};
                static void on_blend_mode_select(on_blend_mode_select_data*);

                void generate_tooltip()
                {
                    static auto generate_tooltip = [](std::string title, std::string description)
                    {
                        std::stringstream str;
                        str << title << "\n\n"
                            << "<span foreground=\"#BBBBBB\">" << description << "</span>";

                        return str.str();
                    };

                    _name_box.set_tooltip_text(generate_tooltip("Layer Name", "Layer names help to stay organized"));
                    _visible_box.set_tooltip_text(generate_tooltip("Layer Visibility", "Hidden layers will not appear in composite renders of the image"));
                    _locked_box.set_tooltip_text(generate_tooltip("Layer Locked", "Locked layers cannot be edited"));
                    _opacity_box.set_tooltip_text(generate_tooltip("Layer Opacity", "Alpha value of all pixels in a layer are multiplied with the layer opacity"));
                    _blend_mode_label.set_tooltip_text(generate_tooltip("Layer Blend Mode", "Blend mode governs how a layer behaves when rendered on top of another layer"));
                    _menu_button.set_tooltip_text("Layer Options");
                    _locked_toggle_button.set_tooltip_text("Toggle Layer Locked");
                    _visible_toggle_button.set_tooltip_text("Toggle Layer Visible");
                }
            };

            struct LayerRow
            {
                LayerPropertyOptions* control_bar;
                Box main = Box(GTK_ORIENTATION_HORIZONTAL);
                ScrolledWindow frame_display_list_viewport;
                ListView frame_display_list = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
                std::deque<LayerFrameDisplay> frame_display;
            };

            std::deque<LayerRow> _layer_rows;
            ListView _layer_row_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
            static void on_layer_row_list_view_reordered(ReorderableListView*, Widget* row_widget_moved, size_t previous_position, size_t next_position, LayerView* instance);

            std::deque<WholeFrameDisplay> _whole_frame_displays;
            ListView _whole_frame_display_list_view_inner = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
            ListView _whole_frame_display_list_view_outer = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);
            
            using on_layer_frame_view_selection_changed_data = struct {LayerView* instance; size_t layer;};
            static void on_layer_frame_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, on_layer_frame_view_selection_changed_data*);
            static void on_whole_frame_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView*);
            static void on_whole_layer_row_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView*);

            void set_layer_frame_selection(size_t layer_i, size_t frame_i);

            struct FrameControlBar
            {
                FrameControlBar();
                operator Widget*();

                ImageDisplay _frame_move_left_icon = ImageDisplay(get_resource_path() + "icons/frame_move_left.png");
                Button _frame_move_left_button;
                static void on_frame_move_left_button_clicked(Button*, FrameControlBar* instance);

                ImageDisplay _frame_create_left_of_this_icon = ImageDisplay(get_resource_path() + "icons/frame_create_left_of_this.png");
                Button _frame_create_left_of_this_button;
                static void on_frame_create_left_of_this_button_clicked(Button*, FrameControlBar* instance);

                ImageDisplay _frame_delete_icon = ImageDisplay(get_resource_path() + "icons/frame_delete.png");
                Button _frame_delete_button;
                static void on_frame_delete_button_clicked(Button*, FrameControlBar* instance);

                ImageDisplay _frame_duplicate_icon = ImageDisplay(get_resource_path() + "icons/frame_duplicate.png");
                Button _frame_duplicate_button;
                static void on_frame_duplicate_button_clicked(Button*, FrameControlBar* instance);

                ImageDisplay _frame_create_right_of_this_icon = ImageDisplay(get_resource_path() + "icons/frame_create_right_of_this.png");
                Button _frame_create_right_of_this_button;
                static void on_frame_create_right_of_this_button_clicked(Button*, FrameControlBar* instance);

                ImageDisplay _frame_move_right_icon = ImageDisplay(get_resource_path() + "icons/frame_move_right.png");
                Button _frame_move_right_button;
                static void on_frame_move_right_button_clicked(Button*, FrameControlBar* instance);

                Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                void generate_tooltip()
                {
                    _frame_move_left_button.set_tooltip_text("");
                    _frame_create_left_of_this_button.set_tooltip_text("");
                    _frame_delete_button.set_tooltip_text("");
                    _frame_duplicate_button.set_tooltip_text("");
                    _frame_create_right_of_this_button.set_tooltip_text("");
                    _frame_move_right_button.set_tooltip_text("");
                }
            };

            FrameControlBar _frame_control_bar;

            struct PlaybackControlBar
            {
                PlaybackControlBar();
                operator Widget*();

                ImageDisplay _playback_jump_to_start_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");
                Button _playback_jump_to_start_button;
                static void on_playback_jump_to_start_button_clicked(Button*, PlaybackControlBar* instance);

                ImageDisplay _playback_pause_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_pause.png");
                Button _playback_pause_button;
                static void on_playback_pause_button_clicked(Button*, PlaybackControlBar* instance);

                ImageDisplay _playback_play_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_play.png");
                Button _playback_play_button;
                static void on_playback_play_button_clicked(Button*, PlaybackControlBar* instance);

                ImageDisplay _playback_jump_to_end_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_end.png");
                Button _playback_jump_to_end_button;
                static void on_playback_jump_to_end_button_clicked(Button*, PlaybackControlBar* instance);

                Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                void generate_tooltip()
                {
                    _playback_jump_to_start_button.set_tooltip_text("");
                    _playback_jump_to_end_button.set_tooltip_text("");
                    _playback_play_button.set_tooltip_text("");
                    _playback_pause_button.set_tooltip_text("");
                }
            };

            PlaybackControlBar _playback_control_bar;

            struct LayerControlBar
            {
                LayerControlBar();
                operator Widget*();

                ImageDisplay _layer_move_up_icon = ImageDisplay(get_resource_path() + "icons/layer_move_up.png");
                Button _layer_move_up_button;
                static void on_layer_move_up_button_clicked(Button*, LayerControlBar* instance);

                ImageDisplay _layer_create_icon = ImageDisplay(get_resource_path() + "icons/layer_create.png");
                Button _layer_create_button;
                static void on_layer_create_button_clicked(Button*, LayerControlBar* instance);

                ImageDisplay _layer_duplicate_icon = ImageDisplay(get_resource_path() + "icons/layer_duplicate.png");
                Button _layer_duplicate_button;
                static void on_layer_duplicate_button_clicked(Button*, LayerControlBar* instance);

                ImageDisplay _layer_delete_icon = ImageDisplay(get_resource_path() + "icons/layer_delete.png");
                Button _layer_delete_button;
                static void on_layer_delete_button_clicked(Button*, LayerControlBar* instance);

                ImageDisplay _layer_move_down_icon = ImageDisplay(get_resource_path() + "icons/layer_move_down.png");
                Button _layer_move_down_button;
                static void on_layer_move_down_button_clicked(Button*, LayerControlBar* instance);

                ImageDisplay _layer_merge_down_icon = ImageDisplay(get_resource_path() + "icons/layer_merge_down.png");
                Button _layer_merge_down_button;
                static void on_layer_merge_down_button_clicked(Button*, LayerControlBar* instance);

                ImageDisplay _layer_flatten_all_icon = ImageDisplay(get_resource_path() + "icons/layer_flatten_all.png");
                Button _layer_flatten_all_button;
                static void on_layer_flatten_all_button_clicked(Button*, LayerControlBar* instance);

                Box _main = Box(GTK_ORIENTATION_VERTICAL);
                void generate_tooltip()
                {
                    _layer_move_up_button.set_tooltip_text("");
                    _layer_move_down_button.set_tooltip_text("");
                    _layer_create_button.set_tooltip_text("");
                    _layer_delete_button.set_tooltip_text("");
                    _layer_duplicate_button.set_tooltip_text("");
                    _layer_merge_down_button.set_tooltip_text("");
                    _layer_flatten_all_button.set_tooltip_text("");
                }
            };

            LayerControlBar _layer_control_bar;

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
            shape.set_texture(&state::layers.at(i)->frames.at(instance->_frame).texture);

            auto task = RenderTask(&shape, nullptr, nullptr, state::layers.at(i)->blend_mode);
            instance->_layer_area.add_render_task(task);
        }

        instance->update();
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
            auto* layer = state::layers.at(i);

            shape->set_visible(layer->is_visible);
            shape->set_color(RGBA(1, 1, 1, layer->opacity));
            shape->set_texture(&layer->frames.at(_frame).texture);
            auto task = RenderTask(shape, nullptr, nullptr, state::layers.at(i)->blend_mode);
            _layer_area.add_render_task(task);
        }

        _layer_area.queue_render();
        generate_tooltip();
    }

    LayerView::WholeFrameDisplay::~WholeFrameDisplay()
    {
        delete _transparency_tiling_shape;
        delete _canvas_size;
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

        _overlay.set_child(&_transparency_area);
        _overlay.add_overlay(&_layer_area);
        _aspect_frame.set_child(&_overlay);
        
        _frame_widget.set_child(&_aspect_frame);
        _frame_widget.set_label_align(0.5);
        _frame_widget.set_label_widget(&_label);

        _frame_widget.set_margin_start(thumbnail_margin_left_right);
        _frame_widget.set_margin_end(thumbnail_margin_left_right);
        _frame_widget.set_margin_top(thumbnail_margin_top_bottom);
        _frame_widget.set_margin_bottom(thumbnail_margin_top_bottom);

        generate_tooltip();
        _frame_widget.set_cursor(GtkCursorType::POINTER);
    }

    LayerView::WholeFrameDisplay::operator Widget*()
    {
        return &_frame_widget;
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
        instance->_layer_shape->set_texture(&state::layers.at(instance->_layer)->frames.at(instance->_frame).texture);

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
        _frame_widget.set_child(&_aspect_frame);
        _frame_widget.set_label_widget(nullptr);

        _overlay.set_child(&_frame_widget);
        _layer_frame_active_icon.set_align(GTK_ALIGN_END);
        _layer_frame_active_icon.set_visible(false);
        _layer_frame_active_icon.set_size_request(_layer_frame_active_icon.get_size());
        _layer_frame_active_icon.set_tooltip_text("Currently editing this frame");
        _overlay.add_overlay(&_layer_frame_active_icon);

        _main.push_back(&_overlay);
        _gl_area.queue_render();

        generate_tooltip();
        _frame_widget.set_cursor(GtkCursorType::POINTER);
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

        auto& layer = *state::layers.at(_layer);
        _layer_shape->set_texture(&layer.frames.at(_frame).texture);
        _layer_shape->set_color(RGBA(1, 1, 1, layer.opacity));

        if (not layer.is_visible)
            _gl_area.set_opacity(0.2);
        else
            _gl_area.set_opacity(1);

        generate_tooltip();
        _gl_area.queue_render();
    }

    // LAYER PROPERTY OPTIONS

    void LayerView::LayerPropertyOptions::update()
    {
        auto& layer = *state::layers.at(_layer);

        _visible_toggle_button.set_all_signals_blocked(true);
        _visible_toggle_button.set_active(not layer.is_visible);
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

        _blend_mode_dropdown.set_list_item_activation_blocked(true);
        size_t i = 0;
        for (; i < _blend_modes_in_order.size(); ++i)
            if (_blend_modes_in_order.at(i) == layer.blend_mode)
                break;

        _blend_mode_dropdown.set_selected(i);
        _blend_mode_dropdown.set_list_item_activation_blocked(false);

        _name_entry.set_all_signals_blocked(true);
        _name_entry.set_text(layer.name);
        _menu_button_title_label.set_text(layer.name);
        _name_entry.set_all_signals_blocked(false);
    }

    LayerView::LayerPropertyOptions::LayerPropertyOptions(size_t layer_i, LayerView* owner)
        : _layer(layer_i), _owner(owner)
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
        _name_entry.set_text(state::layers.at(_layer)->name);
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
        _opacity_scale.set_value(state::layers.at(_layer)->opacity);
        _opacity_scale.connect_signal_value_changed(on_opacity_scale_value_changed, this);

        _blend_mode_dropdown.set_list_item_activation_blocked(true);
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

        _blend_mode_dropdown.set_list_item_activation_blocked(false);

        for (auto* box: {&_name_box, &_opacity_box, &_visible_box, &_locked_box, &_blend_mode_box})
            _menu_button_popover_box.push_back(box);

        _menu_button_popover_box.set_homogeneous(true);
        _menu_button_popover.set_child(&_menu_button_popover_box);
        _menu_button.set_popover(&_menu_button_popover);
        _menu_button.set_has_frame(false);

        _menu_button_title_label.set_text(state::layers.at(_layer)->name);
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

        generate_tooltip();
        for (auto* widget : std::vector<Widget*>{
            &_locked_toggle_button,
            &_visible_toggle_button,
            &_menu_button,
            &_visible_check_button,
            &_locked_check_button,
            &_opacity_scale,
            &_blend_mode_box
        })
            widget->set_cursor(GtkCursorType::POINTER);

        for (auto& label : _blend_mode_dropdown_label_items)
            label.set_cursor(GtkCursorType::POINTER);

        update();
    }

    LayerView::LayerPropertyOptions::operator Widget*()
    {
        return &_main;
    }

    void LayerView::LayerPropertyOptions::on_locked_toggle_button_toggled(ToggleButton* button, LayerPropertyOptions* instance)
    {
        auto& layer = *state::layers.at(instance->_layer);
        layer.is_locked = button->get_active();

        instance->update();
    }

    void LayerView::LayerPropertyOptions::on_visible_toggle_button_toggled(ToggleButton* button, LayerPropertyOptions* instance)
    {
        auto& layer = *state::layers.at(instance->_layer);
        layer.is_visible = not button->get_active();

        instance->update();

        for (auto& display : instance->_owner->_layer_rows.at(instance->_layer).frame_display)
            display.update();

        for (auto& display : instance->_owner->_whole_frame_displays)
            display.update();
    }

    void LayerView::LayerPropertyOptions::on_name_entry_activate(Entry* entry, LayerPropertyOptions* instance)
    {
        auto& layer = *state::layers.at(instance->_layer);
        layer.name = entry->get_text();

        for (auto& display : instance->_owner->_layer_rows.at(instance->_layer).frame_display)
            display.update();

        instance->update();
    }

    void LayerView::LayerPropertyOptions::on_locked_check_button_toggled(CheckButton* button, LayerPropertyOptions* instance)
    {
        auto& layer = *state::layers.at(instance->_layer);
        layer.is_locked = button->get_is_checked();

        instance->update();
    }

    void LayerView::LayerPropertyOptions::on_visible_check_button_toggled(CheckButton* button, LayerPropertyOptions* instance)
    {
        auto& layer = *state::layers.at(instance->_layer);
        layer.is_visible = button->get_is_checked();

        instance->update();

        for (auto& display : instance->_owner->_layer_rows.at(instance->_layer).frame_display)
            display.update();

        for (auto& display : instance->_owner->_whole_frame_displays)
            display.update();
    }

    void LayerView::LayerPropertyOptions::on_opacity_scale_value_changed(Scale* scale, LayerPropertyOptions* instance)
    {
        auto& layer = *state::layers.at(instance->_layer);
        layer.opacity = scale->get_value();

        instance->update();

        for (auto& display : instance->_owner->_layer_rows.at(instance->_layer).frame_display)
            display.update();

        for (auto& display : instance->_owner->_whole_frame_displays)
            display.update();
    }

    void LayerView::LayerPropertyOptions::on_blend_mode_select(on_blend_mode_select_data* data)
    {
        auto& layer = *state::layers.at(data->instance->_layer);
        layer.blend_mode = data->blend_mode;

        for (auto& display : data->instance->_owner->_whole_frame_displays)
            display.update();
    }

    // FRAME CONTROL BAR

    void LayerView::FrameControlBar::on_frame_move_right_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_create_left_of_this_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_create_right_of_this_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_delete_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_move_left_button_clicked(Button*, FrameControlBar* instance)
    {}

    void LayerView::FrameControlBar::on_frame_duplicate_button_clicked(Button*, FrameControlBar* instance)
    {}

    LayerView::FrameControlBar::FrameControlBar()
    {
        for (auto* display : {&_frame_move_left_icon, &_frame_create_left_of_this_icon, &_frame_delete_icon, &_frame_duplicate_icon, &_frame_create_right_of_this_icon, &_frame_move_right_icon})
            display->set_size_request(display->get_size());

        _frame_move_left_button.set_child(&_frame_move_left_icon);
        _frame_move_left_button.connect_signal_clicked(on_frame_move_left_button_clicked, this);

        _frame_create_left_of_this_button.set_child(&_frame_create_left_of_this_icon);
        _frame_create_left_of_this_button.connect_signal_clicked(on_frame_create_left_of_this_button_clicked, this);

        _frame_delete_button.set_child(&_frame_delete_icon);
        _frame_delete_button.connect_signal_clicked(on_frame_delete_button_clicked, this);

        _frame_duplicate_button.set_child(&_frame_duplicate_icon);
        _frame_duplicate_button.connect_signal_clicked(on_frame_duplicate_button_clicked, this);

        _frame_create_right_of_this_button.set_child(&_frame_create_right_of_this_icon);
        _frame_create_right_of_this_button.connect_signal_clicked(on_frame_create_right_of_this_button_clicked, this);

        _frame_move_right_button.set_child(&_frame_move_right_icon);
        _frame_move_right_button.connect_signal_clicked(on_frame_move_right_button_clicked, this);

        _main.push_back(&_frame_move_left_button);
        _main.push_back(&_frame_create_left_of_this_button);
        _main.push_back(&_frame_duplicate_button);
        _main.push_back(&_frame_delete_button);
        _main.push_back(&_frame_create_right_of_this_button);
        _main.push_back(&_frame_move_right_button);

        generate_tooltip();
    }

    LayerView::FrameControlBar::operator Widget*()
    {
        return &_main;
    }

    // PLAYBACK CONTROL BAR

    void LayerView::PlaybackControlBar::on_playback_pause_button_clicked(Button*, PlaybackControlBar* instance)
    {}

    void LayerView::PlaybackControlBar::on_playback_play_button_clicked(Button*, PlaybackControlBar* instance)
    {}

    void LayerView::PlaybackControlBar::on_playback_jump_to_end_button_clicked(Button*, PlaybackControlBar* instance)
    {}

    void LayerView::PlaybackControlBar::on_playback_jump_to_start_button_clicked(Button*, PlaybackControlBar* instance)
    {}

    LayerView::PlaybackControlBar::PlaybackControlBar()
    {
        for (ImageDisplay* icon : {&_playback_jump_to_start_icon, &_playback_pause_icon, &_playback_play_icon, &_playback_jump_to_end_icon})
            icon->set_size_request(icon->get_size());

        _playback_jump_to_start_button.set_child(&_playback_jump_to_start_icon);
        _playback_jump_to_start_button.connect_signal_clicked(&on_playback_jump_to_start_button_clicked, this);

        _playback_pause_button.set_child(&_playback_pause_icon);
        _playback_pause_button.connect_signal_clicked(&on_playback_pause_button_clicked, this);

        _playback_play_button.set_child(&_playback_play_icon);
        _playback_play_button.connect_signal_clicked(&on_playback_play_button_clicked, this);

        _playback_jump_to_end_button.set_child(&_playback_jump_to_end_icon);
        _playback_jump_to_end_button.connect_signal_clicked(&on_playback_jump_to_end_button_clicked, this);

        for (auto* button : {&_playback_jump_to_start_button, &_playback_play_button, &_playback_pause_button, &_playback_jump_to_end_button})
            _main.push_back(button);

        generate_tooltip();
    }

    LayerView::PlaybackControlBar::operator Widget*()
    {
        return &_main;
    }

    // LAYER CONTROL BAR

    void LayerView::LayerControlBar::on_layer_move_up_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_move_down_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_create_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_delete_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_duplicate_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_merge_down_button_clicked(Button*, LayerControlBar* instance)
    {}

    void LayerView::LayerControlBar::on_layer_flatten_all_button_clicked(Button*, LayerControlBar* instance)
    {}

    LayerView::LayerControlBar::operator Widget*()
    {
        return &_main;
    }

    LayerView::LayerControlBar::LayerControlBar()
    {
        for (auto* display : {&_layer_move_up_icon, &_layer_create_icon, &_layer_duplicate_icon, &_layer_delete_icon, &_layer_move_down_icon, &_layer_merge_down_icon, &_layer_flatten_all_icon})
            display->set_size_request(display->get_size());

        _layer_move_up_button.set_child(&_layer_move_up_icon);
        _layer_move_up_button.connect_signal_clicked(on_layer_move_up_button_clicked, this);

        _layer_create_button.set_child(&_layer_create_icon);
        _layer_create_button.connect_signal_clicked(on_layer_create_button_clicked, this);

        _layer_duplicate_button.set_child(&_layer_duplicate_icon);
        _layer_duplicate_button.connect_signal_clicked(on_layer_duplicate_button_clicked, this);

        _layer_delete_button.set_child(&_layer_delete_icon);
        _layer_delete_button.connect_signal_clicked(on_layer_delete_button_clicked, this);

        _layer_move_down_button.set_child(&_layer_move_down_icon);
        _layer_move_down_button.connect_signal_clicked(on_layer_move_down_button_clicked, this);

        _layer_merge_down_button.set_child(&_layer_merge_down_icon);
        _layer_merge_down_button.connect_signal_clicked(on_layer_merge_down_button_clicked, this);

        _layer_flatten_all_button.set_child(&_layer_flatten_all_icon);
        _layer_flatten_all_button.connect_signal_clicked(on_layer_flatten_all_button_clicked, this);

        for (auto* button : {&_layer_move_up_button, &_layer_create_button, &_layer_duplicate_button, &_layer_delete_button, &_layer_move_down_button, &_layer_merge_down_button, &_layer_flatten_all_button})
            _main.push_back(button);

        generate_tooltip();
    }

    // LAYER VIEW

    void LayerView::set_layer_frame_selection(size_t layer_i, size_t frame_i)
    {
        {
            auto* model = _layer_row_list_view.get_selection_model();
            model->set_signal_selection_changed_blocked(true);
            model->select(layer_i);
            model->set_signal_selection_changed_blocked(false);
        }

        for (size_t i = 0; i < _layer_rows.size(); ++i)
        {
            auto& row = _layer_rows.at(i);
            auto* model = row.frame_display_list.get_selection_model();
            model->set_signal_selection_changed_blocked(true);
            model->select(frame_i);
            model->set_signal_selection_changed_blocked(false);

            for (auto& display : row.frame_display)
                display._layer_frame_active_icon.set_visible(false);
        }

        {
            auto* model = _whole_frame_display_list_view_inner.get_selection_model();
            model->set_signal_selection_changed_blocked(true);
            model->select(frame_i);
            model->set_signal_selection_changed_blocked(false);
        }

        _layer_rows.at(layer_i).frame_display.at(frame_i)._layer_frame_active_icon.set_visible(true);
    }

    void LayerView::on_layer_frame_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed,
                                                          on_layer_frame_view_selection_changed_data* data)
    {
        state::current_frame = first_item_position;
        state::current_layer = data->layer;
        data->instance->set_layer_frame_selection(state::current_layer, state::current_frame);
    }

    void LayerView::on_whole_frame_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed,
                                                          LayerView* instance)
    {
        state::current_frame = first_item_position;
        instance->set_layer_frame_selection(state::current_layer, state::current_frame);
    }

    void LayerView::on_whole_layer_row_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed,
                                                         LayerView* instance)
    {
        state::current_layer = first_item_position;
        instance->set_layer_frame_selection(state::current_layer, state::current_frame);
    }

    LayerView::LayerView()
    {
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            _layer_rows.push_back(LayerRow{
                new LayerPropertyOptions(layer_i, this)
            });

            auto& row = _layer_rows.back();
            row.main.push_back(row.control_bar->operator Widget*());

            for (size_t frame_i = 0; frame_i < state::n_frames; ++frame_i)
            {
                row.frame_display.emplace_back(layer_i, frame_i);
                row.frame_display_list.push_back(row.frame_display.back());
                row.frame_display_list.get_selection_model()->connect_signal_selection_changed(
                        on_layer_frame_view_selection_changed,
                        new on_layer_frame_view_selection_changed_data{this, layer_i});
            }

            /*
            row.frame_display_list_viewport.set_child(&row.frame_display_list);
            row.frame_display_list_viewport.set_propagate_natural_height(true);
            row.main.push_back(&row.frame_display_list_viewport);
            */

            row.main.push_back(&row.frame_display_list);
            _layer_row_list_view.push_back(&row.main);
        }

        _layer_row_list_view.get_selection_model()->connect_signal_selection_changed(
                on_whole_layer_row_selection_changed, this);

        for (size_t frame_i = 0; frame_i < state::n_frames; ++frame_i)
        {
            _whole_frame_displays.emplace_back(frame_i);
            _whole_frame_display_list_view_inner.push_back(_whole_frame_displays.back());
        }

        _whole_frame_display_list_view_inner.get_selection_model()->connect_signal_selection_changed(
                on_whole_frame_view_selection_changed, this);

        _whole_frame_display_list_view_outer.push_back(&_whole_frame_display_list_view_inner);
        _whole_frame_display_list_view_outer.set_halign(GTK_ALIGN_END);

        auto* _frame_playback_box = new Box(GTK_ORIENTATION_HORIZONTAL);

        _frame_control_bar.operator Widget*()->set_halign(GTK_ALIGN_END);
        _playback_control_bar.operator Widget*()->set_halign(GTK_ALIGN_END);
        auto* _frame_playback_box_separator = new SeparatorLine();
        _frame_playback_box_separator->set_halign(GTK_ALIGN_END);
        _frame_playback_box_separator->set_size_request({state::margin_unit * 4, 0});
        _frame_playback_box_separator->set_opacity(0);

        _frame_playback_box->push_back(_frame_control_bar);
        _frame_playback_box->push_back(_frame_playback_box_separator);
        _frame_playback_box->push_back(_playback_control_bar);
        _frame_playback_box->set_halign(GTK_ALIGN_END);

        auto* _layer_row_list_view_viewport = new ScrolledWindow();
        _layer_row_list_view_viewport->set_child(&_layer_row_list_view);
        _layer_row_list_view_viewport->set_expand(true);

        auto* hadjust = gtk_scrolled_window_get_hadjustment(_layer_row_list_view_viewport->operator _GtkScrolledWindow *());
        gtk_adjustment_set_value(hadjust, gtk_adjustment_get_upper(hadjust));

        auto* _layer_control_layer_row_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        //_layer_control_layer_row_box->push_back(_layer_control_bar);
        _layer_control_layer_row_box->push_back(_layer_row_list_view_viewport);

        _main.push_back(_layer_control_layer_row_box);
        //_main.push_back(&_whole_frame_display_list_view_outer);
        _main.push_back(_frame_playback_box);

        set_layer_frame_selection(state::current_layer, state::current_frame);
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