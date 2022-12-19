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
                    void set_preview_size(size_t);

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
                    LayerRow(LayerView*, Layer*, size_t frame_i);

                    void update();
                    operator Widget*();

                    void set_layer(Layer*);
                    void set_frame(size_t);

                private:
                    LayerView* _owner;
                    Layer* _layer;
                    size_t _frame_i;

                    Box _outer_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit);

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

                    Entry _layer_name_entry;
                    static void on_layer_name_entry_activated(Entry*, LayerRow* instance);

                    DropDown _blend_mode_drop_down;
                    std::vector<Label*> _blend_mode_drop_down_label_items;
                    std::vector<Label*> _blend_mode_drop_down_list_items;

                    // blendmode -> {label item, list item}
                    static inline const std::vector<std::pair<BlendMode, std::pair<std::string, std::string>>> blend_mode_to_label = {
                        {BlendMode::NORMAL,           {"&#945;", "Alpha Blending"}},
                        {BlendMode::ADD,              {"&#43;", "Add"}},
                        {BlendMode::SUBTRACT,         {"&#8722;", "Subtract"}},
                        {BlendMode::REVERSE_SUBTRACT, {"&#8760;", "Reverse Subtract"}},
                        {BlendMode::MULTIPLY,         {"&#215;", "Multiply"}},
                        {BlendMode::MIN,              {"&#60;", "Minimum"}},
                        {BlendMode::MAX,              {"&#62;", "Maximum"}},
                    };


                    Box _visible_locked_buttons_box = Box(GTK_ORIENTATION_HORIZONTAL);
            };
            
            // control bar

            ImageDisplay _layer_move_up_icon = ImageDisplay(get_resource_path() + "icons/layer_move_up.png");
            Button _layer_move_up_button;
            static void on_layer_move_up_button_clicked(Button*, LayerView* instance);
            Action _layer_move_up_action = Action("layer_view.layer_move_up");

            ImageDisplay _layer_move_down_icon = ImageDisplay(get_resource_path() + "icons/layer_move_down.png");
            Button _layer_move_down_button;
            static void on_layer_move_down_button_clicked(Button*, LayerView* instance);
            Action _layer_move_down_action = Action("layer_view.layer_move_down");

            ImageDisplay _layer_create_icon = ImageDisplay(get_resource_path() + "icons/layer_create.png");
            Button _layer_create_button;
            static void on_layer_create_button_clicked(Button*, LayerView* instance);
            Action _layer_create_action = Action("layer_view.layer_create");

            ImageDisplay _layer_duplicate_icon = ImageDisplay(get_resource_path() + "icons/layer_duplicate.png");
            Button _layer_duplicate_button;
            static void on_layer_duplicate_button_clicked(Button*, LayerView* instance);
            Action _layer_duplicate_action = Action("layer_view.layer_duplicate");

            ImageDisplay _layer_delete_icon = ImageDisplay(get_resource_path() + "icons/layer_delete.png");
            Button _layer_delete_button;
            static void on_layer_delete_button_clicked(Button*, LayerView* instance);
            Action _layer_delete_action = Action("layer_view.layer_delete");

            ImageDisplay _layer_merge_down_icon = ImageDisplay(get_resource_path() + "icons/layer_merge_down.png");
            Button _layer_merge_down_button;
            static void on_layer_merge_down_button_clicked(Button*, LayerView* instance);
            Action _layer_merge_down_action = Action("layer_view.layer_merge_down");

            ImageDisplay _layer_flatten_all_icon = ImageDisplay(get_resource_path() + "icons/layer_flatten_all.png");
            Button _layer_flatten_all_button;
            static void on_layer_flatten_all_button_clicked(Button*, LayerView* instance);
            Action _layer_flatten_all_action = Action("layer_view.layer_flatten_all");

            Box _control_bar_box = Box(GTK_ORIENTATION_HORIZONTAL);

            // menu & actions

            MenuButton _header_menu_button;
            Label _header_menu_button_label = Label("Layers");

            MenuModel _menu;
            PopoverMenu _header_menu_button_popover = PopoverMenu(&_menu);

            size_t _preview_size = state::settings_file->get_value_as<int>("layer_view", "layer_preview_thumbnail_size");
            MenuModel _preview_size_submenu;

            Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _preview_size_label = Label("Preview Size (px): ");
            SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);
            static void on_preview_size_spin_button_value_changed(SpinButton*, LayerView* instance);
            
            // layout

            std::deque<LayerRow> _layer_rows;
            ListView _layer_rows_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_MULTIPLE);

            ScrolledWindow _layer_rows_scrolled_window;
            Box _layer_rows_scrolled_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _layer_rows_scrolled_window_spacer;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
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

        _area.queue_render();
    }

    void LayerView::LayerPreview::set_preview_size(size_t px)
    {
        auto height = px;
        auto width = state::layer_resolution.x / state::layer_resolution.y * height;
        _area.set_size_request({width, height});
    }

    // ###

    LayerView::LayerRow::LayerRow(LayerView* owner, Layer* layer, size_t frame_i)
        : _owner(owner), _layer(layer), _frame_i(frame_i), _layer_preview(layer, frame_i)
    {
        _layer_preview.set_preview_size(_owner->_preview_size);

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

        _layer_name_entry.set_text(_layer->name);
        _layer_name_entry.set_hexpand(true);
        _layer_name_entry.set_vexpand(false);
        _layer_name_entry.set_has_frame(false);
        _layer_name_entry.connect_signal_activate(on_layer_name_entry_activated, this);

        using blend_mode_drop_down_data = struct {BlendMode mode; LayerRow* instance;};

        for (auto& pair: blend_mode_to_label)
        {
            auto* label_item = _blend_mode_drop_down_label_items.emplace_back(new Label("<tt>" + pair.second.first + "</tt>"));
            auto* list_item = _blend_mode_drop_down_list_items.emplace_back(new Label(pair.second.second));

            list_item->set_halign(GTK_ALIGN_START);

            _blend_mode_drop_down.push_back(
                list_item,
                label_item,
                [](blend_mode_drop_down_data data) {
                    // TODO
                }, blend_mode_drop_down_data{pair.first, this}
            );
        }

        _blend_mode_drop_down.set_expand(false);
        _blend_mode_drop_down.set_valign(GTK_ALIGN_CENTER);
        _blend_mode_drop_down.set_halign(GTK_ALIGN_END);

        _outer_box.push_back(&_visible_locked_buttons_box);
        _outer_box.push_back(&_layer_preview_list_view);
        _outer_box.push_back(&_layer_name_entry);
        _outer_box.push_back(&_blend_mode_drop_down);
        _outer_box.set_margin_horizontal(state::margin_unit);
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
    {
        // TODO
    }

    void LayerView::LayerRow::on_is_locked_toggle_button_toggled(ToggleButton*, LayerRow* instance)
    {
        // TODO
    }

    void LayerView::LayerRow::on_layer_name_entry_activated(Entry*, LayerRow* instance)
    {
        // TODO
    }

    // ###

    LayerView::LayerView()
    {
        for (auto* layer : state::layers)
            _layer_rows.emplace_back(this, layer, state::current_frame);

        for (auto& row : _layer_rows)
            _layer_rows_list_view.push_back(row);

        _layer_rows_list_view.set_show_separators(true);

        _layer_rows_scrolled_window_box.push_back(&_layer_rows_list_view);
        _layer_rows_scrolled_window_box.push_back(&_layer_rows_scrolled_window_spacer);
        _layer_rows_scrolled_window_spacer.set_expand(true);

        _layer_rows_scrolled_window.set_child(&_layer_rows_scrolled_window_box);
        _layer_rows_scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _layer_rows_scrolled_window.set_propagate_natural_height(true);
        _layer_rows_scrolled_window.set_expand(true);

        // control bar

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
        {
            button->set_vexpand(false);
            button->set_hexpand(true);
            _control_bar_box.push_back(button);
        }

        _control_bar_box.set_hexpand(true);

        // menu

        _preview_size_spin_button.set_margin_start(state::margin_unit);
        _preview_size_spin_button.set_value(_preview_size);
        _preview_size_spin_button.connect_signal_value_changed(on_preview_size_spin_button_value_changed, this);
        _preview_size_box.push_back(&_preview_size_label);
        _preview_size_box.push_back(&_preview_size_spin_button);
        _preview_size_box.set_margin(state::margin_unit);

        auto settings_section = MenuModel();
        auto preview_size_submenu = MenuModel();
        preview_size_submenu.add_widget(&_preview_size_box);
        settings_section.add_submenu("Preview Size...", &preview_size_submenu);

        _menu.add_section("Settings", &settings_section);

        _header_menu_button_label.set_size_request({32, 32});
        _header_menu_button.set_child(&_header_menu_button_label);

        _header_menu_button_popover = PopoverMenu(&_menu);
        _header_menu_button.set_popover(&_header_menu_button_popover);

        _main.set_homogeneous(false);
        _main.push_back(&_header_menu_button);
        _main.push_back(&_layer_rows_scrolled_window);
        _main.push_back(&_control_bar_box);
    }

    void LayerView::on_layer_move_up_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_layer_move_down_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_layer_create_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_layer_duplicate_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_layer_delete_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_layer_merge_down_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_layer_flatten_all_button_clicked(Button*, LayerView* instance)
    {
        // TODO
    }

    void LayerView::on_preview_size_spin_button_value_changed(SpinButton*, LayerView* instance)
    {
        // TODO
    }

    LayerView::operator Widget*()
    {
        return &_main;
    }

    void LayerView::update()
    {
        for (auto& row : _layer_rows)
            row.update();
    }
}
