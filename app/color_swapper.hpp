// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/tooltip.hpp>
#include <app/canvas.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(color_swapper, swap);
    }

    class ColorSwapper : public AppComponent
    {
        public:
            ColorSwapper();
            ~ColorSwapper();

            operator Widget*() override;
            void update() override;

        private:
            static void on_gl_area_realize(Widget* self, ColorSwapper* instance);
            static void on_gl_area_resize(GLArea* self, int, int, ColorSwapper* instance);
            void swap_colors();

            ClickEventController _click_event_controller;
            static void on_click_release(ClickEventController* self, gint n_press, gdouble x, gdouble y, void* user_data);
            static void on_global_key_pressed(ColorSwapper* instance);

            MotionEventController _motion_event_controller;
            static void on_motion_enter(MotionEventController* self, gdouble x, gdouble y, void* user_data);

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            AspectFrame _frame = AspectFrame(1);
            GLArea _render_area;

            void initialize_render_area();

            Shape* _primary_color_shape = nullptr;
            Shape* _secondary_color_shape = nullptr;

            Shape* _primary_color_shape_frame = nullptr;
            Shape* _secondary_color_shape_frame = nullptr;

            Shape* _primary_color_shape_transparency_tiling = nullptr;
            Shape* _secondary_color_shape_transparency_tiling = nullptr;

            static inline Shader* _transparency_tiling_shader = nullptr;
            Vector2f _canvas_size;

            Label _arrow = Label("");
            Overlay _arrow_overlay = Overlay();

            Tooltip _tooltip;
    };

    namespace state
    {
        static inline ColorSwapper* color_swapper = nullptr;
    }
}
