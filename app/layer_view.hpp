//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/column_view.hpp>
#include <include/box.hpp>
#include <include/gl_area.hpp>
#include <include/scale.hpp>
#include <include/label.hpp>
#include <include/toggle_button.hpp>
#include <include/image_display.hpp>
#include <include/dropdown.hpp>
#include <include/get_resource_path.hpp>
#include <include/check_button.hpp>
#include <include/aspect_frame.hpp>

#include <app/global_state.hpp>
#include <app/layer.hpp>

namespace mousetrap
{
    class LayerView : public Widget
    {
        public:
            LayerView();
            operator GtkWidget*() override;

            static void signal_layer_name_updated(void*, LayerView*);

        private:
            static inline Shader* _noop_shader = nullptr;
            static inline Shader* _transparency_tiling_shader = nullptr;

            static inline size_t _layer_thumbnail_size = 64;

            struct LayerViewRow
            {
                LayerViewRow(Layer*, LayerView* parent);

                LayerView* parent;
                Layer* layer;

                GLArea* texture_area;
                Shape* transparency_tiling_shape;
                Shape* layer_texture_shape;
                AspectFrame* texture_area_frame;

                Entry* name;
                FocusEventController* name_focus_controller;

                Box* visible_button_box;
                Label* visible_button_label;
                CheckButton* visible_button;
                static inline const char* layer_hidden_tooltip = "Layer is Hidden";
                static inline const char* layer_visible_tooltip = "Layer is Visible";

                Box* locked_button_box;
                Label* locked_button_label;
                CheckButton* locked_button;
                static inline const char* layer_locked_tooltip = "Layer is Locked";
                static inline const char* layer_unlocked_tooltip = "Layer is Unlocked";

                Scale* opacity_scale;
                DropDown* blend_mode_dropdown;
                std::vector<Label*> blend_mode_dropdown_label_items;
                std::vector<Label*> blend_mode_dropdown_list_items;

                std::vector<SeparatorLine*> separators;

                ListView* main;

                Vector2f* canvas_size;
                static void on_texture_area_realize(GtkGLArea*, LayerViewRow*);
                static void on_texture_area_resize(GtkGLArea*, int, int, LayerViewRow*);

                static void on_is_visible_toggle(GtkCheckButton*, LayerViewRow*);
                static void on_is_locked_toggle(GtkCheckButton*, LayerViewRow*);

                static void on_opacity_select(GtkScale*, LayerViewRow*);

                static void on_layer_name_activate(GtkEntry*, LayerViewRow*);
                static void on_layer_name_focus_lost(GtkEventControllerFocus* self, void*);

                using on_blendmode_selected_data = struct {LayerViewRow* instance; BlendMode mode;};
                static void on_blendmode_selected(void*);
            };

            ListView* _row_view;
            std::deque<LayerViewRow*> _rows;

            bool initial_update_happened = false;
    };
}

// ###

namespace mousetrap
{
    LayerView::LayerViewRow::LayerViewRow(Layer* layer, LayerView* parent)
        : layer(layer), parent(parent)
    {
        texture_area = new GLArea();
        texture_area->connect_signal("realize", on_texture_area_realize, this);
        texture_area->connect_signal("resize", on_texture_area_resize, this);

        texture_area_frame = new AspectFrame(state::layer_resolution.x / float(state::layer_resolution.y));
        texture_area_frame->set_child(texture_area);

        name = new Entry();
        gtk_editable_set_width_chars(GTK_EDITABLE(name->operator GtkWidget*()), std::string("Layer_9999").size());
        name->set_has_frame(false);
        name->set_text(layer->name);
        name->connect_signal("activate", on_layer_name_activate, this);

        name_focus_controller = new FocusEventController();
        name_focus_controller->connect_leave(on_layer_name_focus_lost, this);
        name->add_controller(name_focus_controller);

        visible_button = new CheckButton();
        visible_button->set_is_checked(layer->is_visible);

        visible_button_label = new Label("");
        visible_button_label->set_use_markup(true);
        visible_button_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        visible_button_box->push_back(visible_button_label);
        visible_button_box->push_back(visible_button);

        locked_button = new CheckButton();
        locked_button->set_is_checked(layer->is_locked);

        locked_button_label = new Label("&#128274;");
        locked_button_label->set_use_markup(true);
        locked_button_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        locked_button_box->push_back(locked_button_label);
        locked_button_box->push_back(locked_button);

        visible_button->connect_signal("toggled", on_is_visible_toggle, this);
        locked_button->connect_signal("toggled", on_is_locked_toggle, this);

        opacity_scale = new Scale(0, 1, 0.01);
        opacity_scale->set_value(layer->opacity);
        opacity_scale->connect_signal("value-changed", on_opacity_select, this);

        blend_mode_dropdown = new DropDown();

        auto add_dropdown_item = [&](BlendMode mode) -> void {

            std::string label;
            std::string list_item;
            std::string tooltip;

            if (mode == BlendMode::NORMAL)
            {
                list_item = "Normal";
                label = "Normal    ";
                tooltip = "Traditional Alpha Blending";
            }
            else if (mode == BlendMode::ADD)
            {
                list_item = "Add";
                label = "Add         ";
                tooltip = "source.rgba + destination.rgba";
            }
            else if (mode == BlendMode::SUBTRACT)
            {
                list_item = "Subtract";
                label = "Subtract  ";
                tooltip = "source.rgba - destination.rgba";
            }
            else if (mode == BlendMode::MULTIPLY)
            {
                list_item = "Multiply";
                label = "Multiply   ";
                tooltip = "source.rgba * destination.rgba";
            }
            else if (mode == BlendMode::MIN)
            {
                list_item = "Minimum ";
                label = "Minimum ";
                tooltip = "min(source.rgba, destination.rgba)";
            }
            else if (mode == BlendMode::MAX)
            {
                list_item = "Maximum";
                label = "Maximum ";
                tooltip = "max(source.rgba, destination.rgba)";
            }

            blend_mode_dropdown_list_items.emplace_back(new Label(list_item));
            blend_mode_dropdown_list_items.back()->set_use_markup(true);
            blend_mode_dropdown_list_items.back()->set_tooltip_text(tooltip);

            blend_mode_dropdown_label_items.emplace_back(new Label(label));
            blend_mode_dropdown_label_items.back()->set_use_markup(true);

            blend_mode_dropdown->push_back(
                blend_mode_dropdown_list_items.back(),
                blend_mode_dropdown_label_items.back(),
                on_blendmode_selected,
                new on_blendmode_selected_data{this, mode}
            );
        };

        for (auto mode : {BlendMode::NORMAL, BlendMode::ADD, BlendMode::SUBTRACT, BlendMode::MULTIPLY, BlendMode::MIN, BlendMode::MAX})
            add_dropdown_item(mode);

        visible_button->set_margin_start(0.5 * state::margin_unit);
        visible_button->set_margin_end(0.5 * state::margin_unit);
        visible_button_box->set_tooltip_text(layer->is_visible ? layer_visible_tooltip : layer_hidden_tooltip);

        locked_button->set_margin_start(0.5 * state::margin_unit);
        locked_button->set_margin_end(state::margin_unit);
        locked_button_box->set_tooltip_text(layer->is_locked ? layer_locked_tooltip : layer_unlocked_tooltip);

        texture_area_frame->set_vexpand(true);
        texture_area_frame->set_size_request({(float(_layer_thumbnail_size) / state::layer_resolution.y) * state::layer_resolution.x, _layer_thumbnail_size});
        texture_area_frame->set_tooltip_text("Layer Preview");

        name->set_margin_start(2 * state::margin_unit);
        name->set_margin_end(2 * state::margin_unit);
        name->set_tooltip_text("Layer Name");

        opacity_scale->set_margin_start(state::margin_unit);
        opacity_scale->set_margin_end(0.5 * state::margin_unit);
        opacity_scale->set_size_request({100, 0});
        opacity_scale->set_tooltip_text("Layer Opacity");

        blend_mode_dropdown->set_margin_start(0.5 * state::margin_unit);
        blend_mode_dropdown->set_margin_end(state::margin_unit);
        blend_mode_dropdown->set_tooltip_text("Layer Blend Mode");

        main = new ListView(GTK_ORIENTATION_HORIZONTAL);
        main->set_show_separators(true);

        auto add_separator = [&]() {

            separators.emplace_back(new SeparatorLine());
            separators.back()->set_vexpand(true);
            separators.back()->set_size_request({2, 0});
            main->push_back(separators.back());
        };

        main->push_back(visible_button_box);
        main->push_back(texture_area_frame);
        main->push_back(name);
        main->push_back(opacity_scale);
        main->push_back(blend_mode_dropdown);
        main->push_back(locked_button_box);
    }

    void LayerView::LayerViewRow::on_texture_area_realize(GtkGLArea* area, LayerViewRow* instance)
    {
        gtk_gl_area_make_current(area);

        if (LayerView::_noop_shader == nullptr)
            LayerView::_noop_shader = new Shader();

        if (LayerView::_transparency_tiling_shader == nullptr)
        {
            LayerView::_transparency_tiling_shader = new Shader();
            LayerView::_transparency_tiling_shader->create_from_file(
                get_resource_path() + "shaders/transparency_tiling.frag",
                ShaderType::FRAGMENT
            );
        }

        instance->canvas_size = new Vector2f(1, 1);
        instance->layer_texture_shape = new Shape();
        instance->layer_texture_shape->as_rectangle({0, 0}, {1, 1});
        instance->layer_texture_shape->set_texture(instance->layer->texture);

        instance->transparency_tiling_shape = new Shape();
        instance->transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        auto task = RenderTask(instance->transparency_tiling_shape, LayerView::_transparency_tiling_shader);
        task.register_vec2("_canvas_size", instance->canvas_size);

        instance->texture_area->add_render_task(task);
        instance->texture_area->add_render_task(instance->layer_texture_shape, LayerView::_noop_shader);

        gtk_gl_area_queue_render(area);
    }

    void LayerView::LayerViewRow::on_texture_area_resize(GtkGLArea* area, int w, int h, LayerViewRow* instance)
    {
        gtk_gl_area_make_current(area);
        instance->canvas_size->x = w;
        instance->canvas_size->y = h;
        gtk_gl_area_queue_render(area);
    }

    void LayerView::LayerViewRow::on_is_visible_toggle(GtkCheckButton* button, LayerViewRow* instance)
    {
        static const float hidden_opacity = 0.2;

        if (gtk_check_button_get_active(button))
        {
            instance->layer->is_visible = true;
            instance->locked_button_box->set_opacity(1);
            instance->texture_area_frame->set_opacity(1);
            instance->name->set_opacity(1);
            instance->opacity_scale->set_opacity(1);
            instance->blend_mode_dropdown->set_opacity(1);
            instance->visible_button_box->set_tooltip_text(layer_visible_tooltip);
        }
        else
        {
            instance->layer->is_visible = false;
            instance->locked_button_box->set_opacity(hidden_opacity);
            instance->texture_area_frame->set_opacity(hidden_opacity);
            instance->name->set_opacity(hidden_opacity);
            instance->opacity_scale->set_opacity(hidden_opacity);
            instance->blend_mode_dropdown->set_opacity(hidden_opacity);
            instance->visible_button_box->set_tooltip_text(layer_hidden_tooltip);
        }
    }

    void LayerView::LayerViewRow::on_is_locked_toggle(GtkCheckButton* button, LayerViewRow* instance)
    {
        static const char* unlocked_lock = "&#128275;";
        static const char* locked_lock = "&#128274;";

        if (gtk_check_button_get_active(button))
        {
            instance->layer->is_locked = true;
            instance->opacity_scale->set_visible(false);
            instance->blend_mode_dropdown->set_visible(false);
            instance->locked_button_label->set_text(locked_lock);
            instance->locked_button_box->set_tooltip_text(layer_locked_tooltip);
            instance->locked_button_label->set_use_markup(true);
        }
        else
        {
            instance->layer->is_locked = false;
            instance->opacity_scale->set_visible(true);
            instance->blend_mode_dropdown->set_visible(true);
            instance->locked_button_label->set_text(locked_lock);
            instance->locked_button_box->set_tooltip_text(layer_unlocked_tooltip);
            instance->locked_button_label->set_use_markup(true);
        }
    }

    void LayerView::LayerViewRow::on_blendmode_selected(void* data)
    {
        auto* instance = ((on_blendmode_selected_data*) data)->instance;
        auto mode = ((on_blendmode_selected_data*) data)->mode;

        instance->layer->blend_mode = mode;
    }

    void LayerView::LayerViewRow::on_opacity_select(GtkScale* scale, LayerViewRow* instance)
    {
        auto value = gtk_range_get_value(GTK_RANGE(scale));
        instance->layer->opacity = value;
        instance->layer_texture_shape->set_color(RGBA(1, 1, 1, value));
        instance->texture_area->queue_render();
    }

    void LayerView::LayerViewRow::on_layer_name_activate(GtkEntry* entry, LayerViewRow* instance)
    {
        auto* buffer = gtk_entry_get_buffer(entry);
        instance->layer->name = gtk_entry_buffer_get_text(buffer);
        gtk_editable_set_text(GTK_EDITABLE(entry), instance->layer->name.c_str());
        std::cout << instance->layer->name << std::endl;
    }

    void LayerView::LayerViewRow::on_layer_name_focus_lost(GtkEventControllerFocus* self, void* data)
    {
        auto* instance = (LayerViewRow*) data;
        instance->name->set_text(instance->layer->name);
    }

    LayerView::LayerView()
    {
        _row_view = new ListView(GTK_ORIENTATION_VERTICAL);
        _row_view->set_show_separators(true);
        _row_view->set_hexpand(true);

        auto image = Image();
        image.create_from_file(get_resource_path() + "mole.png");
        state::add_layer(image);

        for (size_t i = 0; i < 4; ++i)
        {
            _rows.emplace_back(new LayerViewRow(state::layers.back(), this));
            _row_view->push_back(_rows.back()->main);
        }
    }

    LayerView::operator GtkWidget*()
    {
        return _row_view->operator GtkWidget*();
    }

    void LayerView::signal_layer_name_updated(void*, LayerView* instance)
    {
        size_t max = 0;
        for (auto& row : instance->_rows)
        {
            auto width = gtk_widget_get_allocated_width(row->name->operator GtkWidget*());
            max = std::max<int>(max, width);
        }

        for (auto& row : instance->_rows)
            row->name->set_size_request({max, 0});
    }
}