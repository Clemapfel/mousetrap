// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/label.hpp>
#include <include/overlay.hpp>
#include <include/event_controller.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class PrimarySecondaryColorSwapper : public Widget
    {
        public:
            PrimarySecondaryColorSwapper();
            ~PrimarySecondaryColorSwapper();

            operator GtkWidget*();
            void update() override;

        private:

            static void on_gl_area_realize(GtkGLArea* self, PrimarySecondaryColorSwapper* instance);
            static void on_gl_area_resize(GtkGLArea* self, int, int, PrimarySecondaryColorSwapper* instance);
            void swap_colors();

            ClickEventController* _click_event_controller;
            static void on_click_release(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data);
            static void on_global_key_pressed(void* instance);

            MotionEventController* _motion_event_controller;
            static void on_motion_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data);

            AspectFrame* _frame;
            GLArea* _render_area;

            void initialize_render_area();

            Shape* _primary_color_shape = nullptr;
            Shape* _secondary_color_shape = nullptr;

            Shape* _primary_color_shape_frame = nullptr;
            Shape* _secondary_color_shape_frame = nullptr;

            Shape* _primary_color_shape_transparency_tiling = nullptr;
            Shape* _secondary_color_shape_transparency_tiling = nullptr;

            Shader* _transparency_tiling_shader = nullptr;
            Vector2f* _canvas_size;

            Label* _arrow;

            Overlay* _arrow_overlay;
    };
}

// ###

namespace mousetrap
{
    PrimarySecondaryColorSwapper::PrimarySecondaryColorSwapper()
    {
        _render_area = new GLArea();
        _render_area->connect_signal("realize", on_gl_area_realize, this);
        _render_area->connect_signal("resize", on_gl_area_resize, this);
        _render_area->set_cursor(GtkCursorType::POINTER);

        _arrow = new Label("");
        _arrow->set_use_markup(true);
        _arrow->set_halign(GTK_ALIGN_START);
        _arrow->set_valign(GTK_ALIGN_END);

        _arrow_overlay = new Overlay();
        _arrow_overlay->set_child(_arrow);
        _arrow_overlay->add_overlay(_render_area);

        _frame = new AspectFrame(1);
        _frame->set_margin(state::margin_unit);
        _frame->set_tooltip_text(state::shortcut_map->generate_control_tooltip("color_swapper", "Swap Primary and Secondary Color"));
        _frame->set_child(_arrow_overlay);

        _click_event_controller = new ClickEventController();
        _click_event_controller->connect_pressed(on_click_release, this);

        _motion_event_controller = new MotionEventController;
        _motion_event_controller->connect_enter(on_motion_enter, this);

        _render_area->add_controller(_click_event_controller);
        _render_area->add_controller(_motion_event_controller);

        state::main_window->register_global_shortcut(state::shortcut_map, "color_swapper.swap", on_global_key_pressed, this);
   }

    PrimarySecondaryColorSwapper::~PrimarySecondaryColorSwapper()
    {
        delete _primary_color_shape;
        delete _secondary_color_shape;
    }

    PrimarySecondaryColorSwapper::operator GtkWidget*()
    {
        return _frame->operator GtkWidget *();
    }

    void PrimarySecondaryColorSwapper::update()
    {
        if (_primary_color_shape != nullptr)
            _primary_color_shape->set_color(state::primary_color);

        if (_secondary_color_shape != nullptr)
            _secondary_color_shape->set_color(state::secondary_color);

        _render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::on_gl_area_realize(GtkGLArea* self, PrimarySecondaryColorSwapper* instance)
    {
        gtk_gl_area_make_current(self);
        instance->initialize_render_area();
        instance->_render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::on_gl_area_resize(GtkGLArea* self, int w, int h, PrimarySecondaryColorSwapper* instance)
    {
        instance->_canvas_size->x = w;
        instance->_canvas_size->y = h;

        auto min = std::min(w, h);
        std::string spacer = std::to_string(0.105 * min);
        std::string arrow_size = std::to_string(0.18 * min);
        instance->_arrow->set_text("<span font_size=\"" + spacer + "pt\"> </span><span font_size=\"" + arrow_size + "pt\"><tt>&#8635;</tt></span>");
        instance->_arrow->set_use_markup(true);
        instance->update();
        instance->_render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::initialize_render_area()
    {
        delete _primary_color_shape;
        delete _secondary_color_shape;
        delete _primary_color_shape_frame;
        delete _secondary_color_shape_frame;
        delete _primary_color_shape_transparency_tiling;
        delete _secondary_color_shape_transparency_tiling;

        _canvas_size = new Vector2f(1, 1);

        _transparency_tiling_shader = new Shader();
        _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        _primary_color_shape = new Shape();
        _secondary_color_shape = new Shape();
        _primary_color_shape_transparency_tiling = new Shape();
        _secondary_color_shape_transparency_tiling = new Shape();

        Vector2f size = {0.75, 0.75};

        _secondary_color_shape->as_rectangle({0.25, 0.25}, size);
        _primary_color_shape->as_rectangle({0, 0}, size);

        _secondary_color_shape_transparency_tiling->as_rectangle({0.25, 0.25}, size);
        _primary_color_shape_transparency_tiling->as_rectangle({0, 0}, size);

        _primary_color_shape_frame = new Shape;
        {
            auto top_left = _primary_color_shape->get_vertex_position(0);
            auto size = _primary_color_shape->get_size();
            std::vector<Vector2f> vertices = {
                    {top_left.x + 0.001, top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x + 0.001, top_left.y + size.y}
            };
            _primary_color_shape_frame->as_wireframe(vertices);
            _primary_color_shape_frame->set_color(RGBA(0, 0, 0, 1));
        }

        _secondary_color_shape_frame = new Shape;
        {
            auto top_left = _secondary_color_shape->get_vertex_position(0);
            auto size = _secondary_color_shape->get_size();
            std::vector<Vector2f> vertices = {
                    top_left,
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y - 0.001},
                    {top_left.x, top_left.y + size.y - 0.001}
            };
            _secondary_color_shape_frame->as_wireframe(vertices);
            _secondary_color_shape_frame->set_color(RGBA(0, 0, 0, 1));
        }

        _primary_color_shape->set_color(state::primary_color);
        _secondary_color_shape->set_color(state::secondary_color);

        auto primary_transparency_render_task = RenderTask(_primary_color_shape_transparency_tiling, _transparency_tiling_shader);
        primary_transparency_render_task.register_vec2("_canvas_size", _canvas_size);

        auto secondary_transparency_render_task = RenderTask(_secondary_color_shape_transparency_tiling, _transparency_tiling_shader);
        secondary_transparency_render_task.register_vec2("_canvas_size", _canvas_size);

        _render_area->add_render_task(secondary_transparency_render_task);
        _render_area->add_render_task(_secondary_color_shape);
        _render_area->add_render_task(_secondary_color_shape_frame);
        _render_area->add_render_task(primary_transparency_render_task);
        _render_area->add_render_task(_primary_color_shape);
        _render_area->add_render_task(_primary_color_shape_frame);

        _render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::swap_colors()
    {
        auto current_primary = state::primary_color;
        auto current_secondary = state::secondary_color;

        _primary_color_shape->set_color(current_secondary);
        _secondary_color_shape->set_color(current_primary);

        state::primary_color = current_secondary;
        state::secondary_color = current_primary;

        state::color_picker->update();
        state::verbose_color_picker->update();

        _render_area->queue_render();
    }

    void PrimarySecondaryColorSwapper::on_click_release(GtkGestureClick* self, gint n_press, gdouble x, gdouble y,
                                                        void* instance)
    {
        ((PrimarySecondaryColorSwapper*) instance)->swap_colors();
        gtk_widget_grab_focus(((PrimarySecondaryColorSwapper*) instance)->operator GtkWidget*());
    }

    void PrimarySecondaryColorSwapper::on_global_key_pressed(void* instance)
    {
        ((PrimarySecondaryColorSwapper*) instance)->swap_colors();
    }

    void PrimarySecondaryColorSwapper::on_motion_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* instance)
    {
        ((PrimarySecondaryColorSwapper*) instance)->_render_area->grab_focus();
    }
}

