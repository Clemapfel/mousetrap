
//
// Copyright 2022 Clemens Cords
// Created on 9/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/scale.hpp>
#include <include/spin_button.hpp>
#include <include/gl_area.hpp>

namespace mousetrap
{
    enum class BrushShape
    {
        RECTANGLE,
        CIRCLE,
        CUSTOM
    };

    class BrushOptions : public Widget
    {
        public:
            BrushOptions();
            operator GtkWidget*() override;

        private:
            static inline constexpr size_t max_brush_size_on_scale = 50;
            static inline constexpr const char* brush_size_label_text = "Brush Size (px) "; // trailing space sic
            static inline constexpr const char* brush_shape_label_text = "Brush Shape "; // trailing space sic
            static inline constexpr const char* opacity_label_text = "Opacity "; // trailing space sic

            Label* _brush_size_label;
            SeparatorLine* _brush_size_label_separator;
            Box* _brush_size_label_box;

            Scale* _brush_size_scale;
            SpinButton* _brush_size_spin_button;
            Box* _brush_size_scale_box;

            static inline constexpr const char* circle_brush_shape_icon_id = "circle_brush_shape";
            static inline constexpr const char* rectangle_brush_shape_icon_id = "rectangle_brush_shape";
            static inline constexpr const char* custom_brush_shape_icon_id = "custom_brush_shape";

            ImageDisplay* _circle_brush_icon;
            ToggleButton* _circle_brush_toggle_button;

            ImageDisplay* _rectangle_brush_icon;
            ToggleButton* _rectangle_brush_toggle_button;

            ImageDisplay* _custom_brush_icon;
            ToggleButton* _custom_brush_toggle_button;

            Box* _brush_shape_hbox;

            Label* _brush_shape_label;
            SeparatorLine* _brush_shape_label_separator;
            Box* _brush_shape_label_box;

            Scale* _opacity_scale;
            SpinButton* _opacity_spin_button;
            Box* _opacity_scale_box;

            Label* _opacity_label;
            SeparatorLine* _opacity_label_separator;
            Box* _opacity_label_box;

            using on_brush_size_changed_data = BrushOptions*;
            static void on_brush_size_changed(void*, void* instance);

            using on_brush_shape_selected_data = struct{ BrushOptions* instance; BrushShape which; };
            static void on_brush_shape_selected(GtkToggleButton*, void*);

            using on_brush_opacity_changed_data = BrushOptions*;
            static void on_brush_opacity_changed(void*, void* instance);

            struct BrushPreviewCanvas : public Widget
            {
                BrushPreviewCanvas();
                void set_resolution(size_t);

                GLArea* canvas;
                AspectFrame* main;

                std::vector<Shape*> pixel_lines;

                Shape* transparency_tiling_shape;
                Vector2f* canvas_size;
                Shader* transparency_tiling_shader;

                Shape* brush_shape;
                Shape* outline_frame;

                operator GtkWidget*() override {
                    return main->operator GtkWidget*();
                }

                static void on_resize(GtkGLArea*, int, int, void*);
                static void on_realize(GtkGLArea*, void* instance);
            };

            BrushPreviewCanvas* _brush_preview;

            Box* _main;
    };
}

// ###

namespace mousetrap
{
    void BrushOptions::on_brush_size_changed(void* range_or_spin_button, void* data)
    {
        auto* instance = (BrushOptions*) data;

        size_t value;

        if (GTK_IS_RANGE(range_or_spin_button))
            value = std::round(gtk_range_get_value((GtkRange*) range_or_spin_button));
        else if (GTK_IS_SPIN_BUTTON(range_or_spin_button))
            value = std::round(gtk_spin_button_get_value((GtkSpinButton*) range_or_spin_button));

        instance->_brush_size_scale->set_all_signals_blocked(true);
        instance->_brush_size_spin_button->set_all_signals_blocked(true);

        instance->_brush_size_scale->set_value(value);
        instance->_brush_size_spin_button->set_value(value);

        if (value > instance->max_brush_size_on_scale)
            instance->_brush_size_scale->set_opacity(0.5);
        else
            instance->_brush_size_scale->set_opacity(1);

        instance->_brush_size_scale->set_all_signals_blocked(false);
        instance->_brush_size_spin_button->set_all_signals_blocked(false);

        instance->_brush_preview->set_resolution(value);
    }

    void BrushOptions::on_brush_opacity_changed(void* range_or_spin_button, void* data)
    {
        auto* instance = (BrushOptions*) data;

        float value;

        if (GTK_IS_RANGE(range_or_spin_button))
            value = gtk_range_get_value((GtkRange*) range_or_spin_button);
        else if (GTK_IS_SPIN_BUTTON(range_or_spin_button))
            value = gtk_spin_button_get_value((GtkSpinButton*) range_or_spin_button);

        instance->_opacity_scale->set_all_signals_blocked(true);
        instance->_opacity_spin_button->set_all_signals_blocked(true);

        instance->_opacity_scale->set_value(value);
        instance->_opacity_spin_button->set_value(value);

        instance->_opacity_scale->set_all_signals_blocked(false);
        instance->_opacity_spin_button->set_all_signals_blocked(false);

        instance->_brush_preview->brush_shape->set_color(RGBA(0, 0, 0, value));
        instance->_brush_preview->canvas->queue_render();
    }

    void BrushOptions::on_brush_shape_selected(GtkToggleButton* button, void* data)
    {
        auto which = ((on_brush_shape_selected_data *) data)->which;
        auto* instance = ((on_brush_shape_selected_data *) data)->instance;

        for (auto* button : {
            instance->_rectangle_brush_toggle_button,
            instance->_circle_brush_toggle_button,
            instance->_custom_brush_toggle_button
        })
            button->set_all_signals_blocked(true);

        if (which == BrushShape::RECTANGLE)
        {
            instance->_rectangle_brush_toggle_button->set_active(true);
            instance->_circle_brush_toggle_button->set_active(false);
            instance->_custom_brush_toggle_button->set_active(false);
        }
        else if (which == BrushShape::CIRCLE)
        {
            instance->_rectangle_brush_toggle_button->set_active(false);
            instance->_circle_brush_toggle_button->set_active(true);
            instance->_custom_brush_toggle_button->set_active(false);
        }
        else if (which == BrushShape::CUSTOM)
        {
            instance->_rectangle_brush_toggle_button->set_active(false);
            instance->_circle_brush_toggle_button->set_active(false);
            instance->_custom_brush_toggle_button->set_active(true);
        }

        for (auto* button : {
                instance->_rectangle_brush_toggle_button,
                instance->_circle_brush_toggle_button,
                instance->_custom_brush_toggle_button
        })
            button->set_all_signals_blocked(false);
    }

    void BrushOptions::BrushPreviewCanvas::on_resize(GtkGLArea* area, int x, int y, void* data)
    {
        std::cout << x << " " << y << std::endl;

        auto* instance = (BrushOptions::BrushPreviewCanvas*) data;
        instance->canvas_size->x = x;
        instance->canvas_size->y = y;

        gtk_gl_area_queue_render(area);
    }

    void BrushOptions::BrushPreviewCanvas::on_realize(GtkGLArea* area, void* data)
    {
        auto* instance = (BrushOptions::BrushPreviewCanvas*) data;
        gtk_gl_area_make_current(area);

        instance->transparency_tiling_shader = new Shader();
        instance->transparency_tiling_shader->create_from_file(get_resource_path() + "/shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        instance->canvas_size = new Vector2f(1, 1);

        instance->transparency_tiling_shape = new Shape();
        instance->transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->outline_frame = new Shape();
        auto eps = 0.001;
        instance->outline_frame->as_wireframe({
              {eps, 0},
              {eps, 1 - eps},
              {1 - eps, 1 - eps},
              {1 - eps, 0}
        });
        instance->outline_frame->set_color(RGBA(1, 1, 1, 1));

        instance->brush_shape = new Shape();
        instance->brush_shape->as_rectangle({0, 0}, {1, 1});
        instance->brush_shape->set_color(RGBA(0, 0, 0, 1));

        instance->set_resolution(2);
    }

    void BrushOptions::BrushPreviewCanvas::set_resolution(size_t resolution)
    {
        canvas->make_current();

        for (auto* ptr : pixel_lines)
            delete ptr;

        pixel_lines.clear();

        for (size_t i = 0; resolution < 30 and i < resolution; ++i)
        {
            pixel_lines.emplace_back(new Shape());
            pixel_lines.back()->as_line({0, float(i) / resolution}, {1, float(i) / resolution});
            pixel_lines.back()->set_color(RGBA(1, 1, 1, 1));

            pixel_lines.emplace_back(new Shape());
            pixel_lines.back()->as_line({float(i) / resolution, 0}, {float(i) / resolution, 1});
            pixel_lines.back()->set_color(RGBA(1, 1, 1, 1));
        }

        canvas->clear_render_tasks();

        auto transparency_tiling_task = RenderTask(transparency_tiling_shape, transparency_tiling_shader);
        transparency_tiling_task.register_vec2("_canvas_size", canvas_size);

        canvas->add_render_task(transparency_tiling_task);
        canvas->add_render_task(brush_shape);

        for (auto* line : pixel_lines)
            canvas->add_render_task(line);

        canvas->add_render_task(outline_frame);
        canvas->queue_render();
    }

    BrushOptions::BrushPreviewCanvas::BrushPreviewCanvas()
    {
        canvas = new GLArea();
        canvas->connect_signal("realize", on_realize, this);
        canvas->connect_signal("resize", on_resize, this);

        main = new AspectFrame(1);
        main->set_child(canvas);
        main->set_size_request({100, 100});
    }

    BrushOptions::BrushOptions()
    {
        _brush_size_scale = new Scale(1, 50, 1);
        _brush_size_scale->set_hexpand(true);
        _brush_size_scale->set_vexpand(false);
        _brush_size_scale->set_margin_end(state::margin_unit);
        _brush_size_scale->connect_signal("value-changed", on_brush_size_changed, this);

        _brush_size_spin_button = new SpinButton(1, 999, 1);
        _brush_size_spin_button->set_digits(0);
        _brush_size_spin_button->set_expand(false);
        _brush_size_spin_button->set_halign(GTK_ALIGN_END);
        _brush_size_spin_button->connect_signal("value-changed", on_brush_size_changed, this);

        _brush_size_scale_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_size_scale_box->push_back(_brush_size_scale);
        _brush_size_scale_box->push_back(_brush_size_spin_button);
        _brush_size_scale_box->set_vexpand(false);

        _brush_size_label = new Label(brush_size_label_text);
        _brush_size_label->set_halign(GTK_ALIGN_START);
        _brush_size_label->set_hexpand(false);
        _brush_size_label->set_margin_start(0.5 * state::margin_unit);

        _brush_size_label_separator = new SeparatorLine();
        _brush_size_label_separator->set_hexpand(true);

        _brush_size_label_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_size_label_box->push_back(_brush_size_label);
        _brush_size_label_box->push_back(_brush_size_label_separator);

        _circle_brush_icon = new ImageDisplay(get_resource_path() + "icons/" + circle_brush_shape_icon_id + ".png");
        _rectangle_brush_icon = new ImageDisplay(get_resource_path() + "icons/" + rectangle_brush_shape_icon_id + ".png");
        _custom_brush_icon = new ImageDisplay(get_resource_path() + "icons/" + custom_brush_shape_icon_id + ".png");

        for (auto* icon : {_circle_brush_icon, _rectangle_brush_icon, _custom_brush_icon})
            icon->set_size_request({32, 32});

        _rectangle_brush_toggle_button = new ToggleButton();
        _rectangle_brush_toggle_button->set_child(_rectangle_brush_icon);
        _rectangle_brush_toggle_button->connect_signal(
            "toggled",
            on_brush_shape_selected,
            new on_brush_shape_selected_data{this, BrushShape::RECTANGLE}
        );

        _circle_brush_toggle_button = new ToggleButton();
        _circle_brush_toggle_button->set_child(_circle_brush_icon);
        _circle_brush_toggle_button->connect_signal(
                "toggled",
                on_brush_shape_selected,
                new on_brush_shape_selected_data{this, BrushShape::CIRCLE}
        );

        _custom_brush_toggle_button = new ToggleButton();
        _custom_brush_toggle_button->set_child(_custom_brush_icon);
        _custom_brush_toggle_button->connect_signal(
                "toggled",
                on_brush_shape_selected,
                new on_brush_shape_selected_data{this, BrushShape::CUSTOM}
        );

        for (auto* button : {_rectangle_brush_toggle_button, _circle_brush_toggle_button, _custom_brush_toggle_button})
        {
            button->set_hexpand(true);
            button->set_margin_start(0.5 * state::margin_unit);
            button->set_margin_end(0.5 * state::margin_unit);
        }

        _brush_shape_hbox = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_shape_hbox->push_back(_rectangle_brush_toggle_button);
        _brush_shape_hbox->push_back(_circle_brush_toggle_button);
        _brush_shape_hbox->push_back(_custom_brush_toggle_button);
        _brush_shape_hbox->set_hexpand(true);

        _rectangle_brush_toggle_button->set_active(true);

        _brush_shape_label = new Label(brush_shape_label_text);
        _brush_shape_label->set_margin_start(0.5 * state::margin_unit);
        _brush_shape_label_separator = new SeparatorLine();
        _brush_shape_label_separator->set_hexpand(true);

        _brush_shape_label_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_shape_label_box->push_back(_brush_shape_label);
        _brush_shape_label_box->push_back(_brush_shape_label_separator);

        _opacity_label = new Label(opacity_label_text);
        _opacity_label->set_margin_start(0.5 * state::margin_unit);

        _opacity_label_separator = new SeparatorLine();
        _opacity_label_separator->set_hexpand(true);

        _opacity_label_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _opacity_label_box->push_back(_opacity_label);
        _opacity_label_box->push_back(_opacity_label_separator);

        _opacity_scale = new Scale(0, 1, 0.05);
        _opacity_scale->set_hexpand(true);
        _opacity_scale->set_vexpand(false);
        _opacity_scale->set_margin_end(state::margin_unit);
        _opacity_scale->set_value(1);
        _opacity_scale->connect_signal("value-changed", on_brush_opacity_changed, this);

        _opacity_spin_button = new SpinButton(0, 1, 0.05);
        _opacity_spin_button->set_digits(2);
        _opacity_spin_button->set_expand(false);
        _opacity_spin_button->set_vexpand(false);
        _opacity_spin_button->set_halign(GTK_ALIGN_END);
        _opacity_spin_button->set_value(1);
        _opacity_scale->connect_signal("value-changed", on_brush_opacity_changed, this);

        _opacity_scale_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _opacity_scale_box->push_back(_opacity_scale);
        _opacity_scale_box->push_back(_opacity_spin_button);

        _brush_preview = new BrushPreviewCanvas();

        _main = new Box(GTK_ORIENTATION_VERTICAL, state::margin_unit);
        _main->set_expand(true);
        _main->push_back(_brush_size_label_box);
        _main->push_back(_brush_size_scale_box);
        _main->push_back(_brush_shape_label_box);
        _main->push_back(_brush_shape_hbox);
        _main->push_back(_brush_preview);
        _main->push_back(_opacity_label_box);
        _main->push_back(_opacity_scale_box);
    }

    BrushOptions::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }
}