// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/list_view.hpp>
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

        private:
            static inline Shader* _noop_shader = nullptr;
            static inline Shader* _transparency_tiling_shader = nullptr;

            struct LayerViewRow : public Widget
            {
                LayerViewRow(Layer*);
                operator GtkWidget*() override;

                Layer* layer;

                Box* main_box;

                GLArea* texture_area;
                Shape* shape;
                AspectFrame* texture_area_frame;

                Label* name;

                CheckButton* hidden_button;
                CheckButton* locked_button;

                Scale* opacity_scale;
                DropDown* blend_mode_dropdown;
                std::vector<Label*> blend_mode_dropdown_items;

                Vector2f* canvas_size;
                static void on_texture_area_realize(GtkGLArea*, LayerViewRow*);
                static void on_texture_area_resize(GtkGLArea*, int, int, LayerViewRow*);

                static void on_is_hidden_toggle(GtkCheckButton*, LayerViewRow*);
                static void on_is_locked_toggle(GtkCheckButton*, LayerViewRow*);

                using on_blendmode_selected_data = struct {LayerViewRow* instance; BlendMode mode;};
                static void on_blendmode_selected(void*);
            };

            ListView* _list_view;
            std::deque<LayerViewRow*> _list_view_rows;
    };
}

// ###

namespace mousetrap
{
    LayerView::LayerViewRow::operator GtkWidget*()
    {
        return main_box->operator GtkWidget*();
    }

    LayerView::LayerViewRow::LayerViewRow(Layer* layer)
        : layer(layer)
    {
        texture_area = new GLArea();
        texture_area->connect_signal("realize", on_texture_area_realize, this);
        texture_area->connect_signal("resize", on_texture_area_resize, this);

        texture_area_frame = new AspectFrame(state::layer_resolution.x / float(state::layer_resolution.y));
        texture_area_frame->set_child(texture_area);
        texture_area_frame->set_size_request({32, 32});

        name = new Label(layer->name);

        hidden_button = new CheckButton();
        locked_button = new CheckButton();

        hidden_button->set_tooltip_text("Layer Hidden");
        locked_button->set_tooltip_text("Layer Locked");

        hidden_button->connect_signal("toggled", on_is_hidden_toggle, this);
        locked_button->connect_signal("toggled", on_is_locked_toggle, this);

        opacity_scale = new Scale(0, 1, 0.01);
        blend_mode_dropdown = new DropDown();

        auto add_dropdown_item = [&](BlendMode mode) -> void {

            std::string label;

            if (mode == BlendMode::NORMAL)
                label = "Normal";
            else if (mode == BlendMode::ADD)
                label = "Add";
            else if (mode == BlendMode::SUBTRACT)
                label = "Subtract";
            else if (mode == BlendMode::MULTIPLY)
                label = "Multiply";

            blend_mode_dropdown_items.emplace_back(new Label(label));
            blend_mode_dropdown->push_back(
                blend_mode_dropdown_items.back(),
                label,
                on_blendmode_selected,
                new on_blendmode_selected_data{this, mode}
            );
        };

        for (auto mode : {BlendMode::NORMAL, BlendMode::ADD, BlendMode::SUBTRACT, BlendMode::MULTIPLY})
            add_dropdown_item(mode);

        main_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        main_box->push_back(hidden_button);
        main_box->push_back(texture_area_frame);
        main_box->push_back(name);
        main_box->push_back(opacity_scale);
        main_box->push_back(blend_mode_dropdown);
        main_box->push_back(locked_button);
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
        instance->shape = new Shape();
        instance->shape->as_rectangle({0, 0}, {1, 1});
        instance->shape->set_texture(instance->layer->texture);

        auto task = RenderTask(instance->shape, LayerView::_transparency_tiling_shader);
        task.register_vec2("_canvas_size", instance->canvas_size);

        instance->texture_area->add_render_task(task);
        instance->texture_area->add_render_task(instance->shape, LayerView::_noop_shader);

        gtk_gl_area_queue_render(area);
    }

    void LayerView::LayerViewRow::on_texture_area_resize(GtkGLArea* area, int w, int h, LayerViewRow* instance)
    {
        gtk_gl_area_make_current(area);
        instance->canvas_size->x = w;
        instance->canvas_size->y = h;
        gtk_gl_area_queue_render(area);
    }

    void LayerView::LayerViewRow::on_is_hidden_toggle(GtkCheckButton* button, LayerViewRow* instance)
    {
        if (gtk_check_button_get_active(button))
        {
            instance->layer->is_visible = false;
            instance->texture_area->set_opacity(0.5);
        }
        else
        {
            instance->layer->is_visible = true;
            instance->texture_area->set_opacity(1);
        }
    }

    void LayerView::LayerViewRow::on_is_locked_toggle(GtkCheckButton* button, LayerViewRow* instance)
    {
        if (gtk_check_button_get_active(button))
        {
            instance->layer->is_locked = true;
        }
        else
        {
            instance->layer->is_locked = false;
        }
    }

    void LayerView::LayerViewRow::on_blendmode_selected(void* data)
    {
        auto* instance = ((on_blendmode_selected_data*) data)->instance;
        auto mode = ((on_blendmode_selected_data*) data)->mode;

        instance->layer->blend_mode = mode;
    }

    LayerView::LayerView()
    {
        _list_view = new ListView(GTK_ORIENTATION_VERTICAL);

        auto image = Image();
        image.create_from_file(get_resource_path() + "mole.png");
        state::add_layer(image);

        _list_view_rows.emplace_back(new LayerViewRow(state::layers.back()));
        _list_view->push_back(_list_view_rows.back());
    }

    LayerView::operator GtkWidget*()
    {
        return _list_view->operator GtkWidget*();
    }
}