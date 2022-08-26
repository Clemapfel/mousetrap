// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/label.hpp>
#include <include/overlay.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class PrimarySecondaryColorSwapper : public Widget
    {
        public:
            PrimarySecondaryColorSwapper();
            ~PrimarySecondaryColorSwapper();

            GtkWidget* get_native();

        private:
            void update();

            static void on_gl_area_realize(GtkGLArea* self, PrimarySecondaryColorSwapper* instance);
            static void on_gl_area_resize(GtkGLArea* self, int, int, PrimarySecondaryColorSwapper* instance);
            void swap_colors();

            static void on_pressed(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, PrimarySecondaryColorSwapper* instance);
            static void on_key_pressed(GtkEventControllerKey* self,
                                       guint keyval,
                                       guint keycode,
                                       GdkModifierType state,
                                       PrimarySecondaryColorSwapper* user_data);
            AspectFrame* _frame;
            GLArea* _render_area;

            void initialize_render_area();

            Shape* _primary_color_shape = nullptr;
            Shape* _secondary_color_shape = nullptr;

            Shape* _primary_color_shape_frame = nullptr;
            Shape* _secondary_color_shape_frame = nullptr;

            Label* _arrow;

            Overlay* _arrow_overlay;

            static inline const std::string _tooltip = {
                "<u>Primary / Secondary Color</u>\n"
                "<tt>Left-Click</tt> : Swap Colors\n"
                "<tt>Right-Click</tt> : Start Color Selection"
            };

            GtkEventController* _button_event_controller;
    };

    namespace state
    {
        static inline PrimarySecondaryColorSwapper* primary_secondary_color_swapper_instance = nullptr;
    }
}

// ###

namespace mousetrap
{
    PrimarySecondaryColorSwapper::PrimarySecondaryColorSwapper()
    {
        _render_area = new GLArea();
        _render_area->connect_signal("realize", on_gl_area_realize, this);
        _render_area->connect_signal("resize", on_gl_area_resize, this);

        _arrow = new Label("UNINITIALIZED");
        _arrow->set_use_markup(true);
        _arrow->set_halign(GTK_ALIGN_START);
        _arrow->set_valign(GTK_ALIGN_END);

        _arrow_overlay = new Overlay();
        _arrow_overlay->set_over(_render_area);
        _arrow_overlay->set_under(_arrow);

        _frame = new AspectFrame(1);
        _frame->set_margin(state::margin_unit);
        _frame->set_tooltip_text(_tooltip);
        _frame->add(_arrow_overlay->get_native());

        _button_event_controller = GTK_EVENT_CONTROLLER(gtk_gesture_click_new());
        _button_event_controller = GTK_EVENT_CONTROLLER(gtk_event_controller_key_new());
        gtk_widget_add_controller(_render_area->get_native(), GTK_EVENT_CONTROLLER(_button_event_controller));
        //g_signal_connect(_button_event_controller, "pressed", G_CALLBACK(on_pressed), this);
        g_signal_connect(_button_event_controller, "key-pressed", G_CALLBACK(on_key_pressed), this);
    }

    PrimarySecondaryColorSwapper::~PrimarySecondaryColorSwapper()
    {
        delete _primary_color_shape;
        delete _secondary_color_shape;
    }

    GtkWidget* PrimarySecondaryColorSwapper::get_native()
    {
        return _frame->get_native();
    }

    void PrimarySecondaryColorSwapper::update()
    {
        if (_primary_color_shape != nullptr)
            _primary_color_shape->set_color(state::primary_color);

        if (_secondary_color_shape != nullptr)
            _secondary_color_shape->set_color(state::secondary_color);
    }

    void PrimarySecondaryColorSwapper::on_gl_area_realize(GtkGLArea* self, PrimarySecondaryColorSwapper* instance)
    {
        gtk_gl_area_make_current(self);
        instance->initialize_render_area();
        instance->on_gl_area_resize(self, 1, 1, instance);
        gtk_gl_area_queue_render(self);
    }

    void PrimarySecondaryColorSwapper::on_gl_area_resize(GtkGLArea* self, int w, int h, PrimarySecondaryColorSwapper* instance)
    {
        auto min = std::min(w, h);
        std::string spacer = std::to_string(0.105 * min);
        std::string arrow_size = std::to_string(0.18 * min);

        instance->_arrow->set_text("<span font_size=\"" + spacer + "pt\"> </span><span font_size=\"" + arrow_size + "pt\"><tt>&#8635;</tt></span>");
        instance->_arrow->set_use_markup(true);

        gtk_gl_area_queue_render(self);
    }

    void PrimarySecondaryColorSwapper::initialize_render_area()
    {
        delete _primary_color_shape;
        delete _secondary_color_shape;
        delete _primary_color_shape_frame;
        delete _secondary_color_shape_frame;

        _primary_color_shape = new Shape();
        _secondary_color_shape = new Shape();

        Vector2f size = {0.75, 0.75};

        _secondary_color_shape->as_rectangle({0.25, 0.25}, size);
        _primary_color_shape->as_rectangle({0, 0}, size);

        _primary_color_shape_frame = new Shape;
        {
            auto vertices = std::vector<Vector2f>();
            for (size_t i = 0; i < _primary_color_shape->get_n_vertices(); ++i)
                vertices.push_back(_primary_color_shape->get_vertex_position(i));

            _primary_color_shape_frame->as_wireframe(vertices);
            _primary_color_shape_frame->set_color(RGBA(0, 0, 0, 1));
        }

        _secondary_color_shape_frame = new Shape;
        {
            auto vertices = std::vector<Vector2f>();
            for (size_t i = 0; i < _secondary_color_shape->get_n_vertices(); ++i)
                vertices.push_back(_secondary_color_shape->get_vertex_position(i));

            _secondary_color_shape_frame->as_wireframe(vertices);
            _secondary_color_shape_frame->set_color(RGBA(0, 0, 0, 1));
        }

        _primary_color_shape->set_color(state::primary_color);
        _secondary_color_shape->set_color(state::secondary_color);

        _render_area->add_render_task(_secondary_color_shape);
        _render_area->add_render_task(_secondary_color_shape_frame);
        _render_area->add_render_task(_primary_color_shape);
        _render_area->add_render_task(_primary_color_shape_frame);

        _render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::swap_colors()
    {
        auto current_primary = state::primary_color;
        auto current_secondary = state::secondary_color;

        set_secondary_color(current_primary);
        set_primary_color(current_secondary);

        _primary_color_shape->set_color(current_secondary);
        _secondary_color_shape->set_color(current_primary);

        _render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::on_pressed(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, PrimarySecondaryColorSwapper* instance)
    {
        instance->swap_colors();
    }

    void PrimarySecondaryColorSwapper::on_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode,
                                                      GdkModifierType state, PrimarySecondaryColorSwapper* instance)
    {
        std::cout << keyval << std::endl;
        instance->swap_colors();
    }

}

