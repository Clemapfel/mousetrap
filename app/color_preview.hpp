// 
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/project_state.hpp>
#include <app/tooltip.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    class ColorPreview : public AppComponent, public signals::ColorSelectionChanged
    {
        public:
            ColorPreview();
            ~ColorPreview();

            operator Widget*() override;

        private:
            void on_color_selection_changed();

            void update(HSVA current, HSVA previous);
            static inline const float _previous_to_color_current_width_ratio = 0.15;

            GLArea _gl_area;
            static void on_gl_area_realize(Widget*, ColorPreview*);
            static void on_gl_area_resize(GLArea*, int, int, ColorPreview*);
            Vector2f _canvas_size;

            static inline Shader* transparency_tiling_shader = nullptr;

            Shape* _current_color_shape = nullptr;
            Shape* _previous_color_shape = nullptr;
            Shape* _frame_shape = nullptr;
            Shape* _transparency_tiling_shape = nullptr;

            Tooltip _tooltip;
    };

    namespace state
    {
        inline ColorPreview* color_preview;
    }
}
