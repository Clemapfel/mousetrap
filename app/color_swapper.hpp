// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/app_component.hpp>
#include <app/project_state.hpp>
#include <app/tooltip.hpp>
#include <app/canvas.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(color_swapper, swap);
    }

    class ColorSwapper : public AppComponent, public signals::ColorSelectionChanged
    {
        public:
            ColorSwapper();
            ~ColorSwapper();

            operator Widget*() override;

        protected:
            void on_color_selection_changed() override;

        private:
            static void on_gl_area_realize(Widget* self, ColorSwapper* instance);
            static void on_gl_area_resize(GLArea* self, int, int, ColorSwapper* instance);

            ClickEventController _click_event_controller;
            static void on_click_release(ClickEventController* self, gint n_press, gdouble x, gdouble y, ColorSwapper* instance);

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
        inline ColorSwapper* color_swapper = nullptr;
    }
}
