// 
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    class ColorPreview : public AppComponent
    {
        public:
            ColorPreview();
            ~ColorPreview();

            operator Widget*() override;
            void update() override;

        private:
            void update(HSVA current, HSVA previous);
            static inline const float _previous_to_color_current_width_ratio = 0.15;

            GLArea _gl_area;
            static void on_gl_area_realize(Widget*, ColorPreview*);
            static void on_gl_area_resize(GLArea*, int, int, ColorPreview*);
            Vector2f _canvas_size;

            static inline Shader* transparency_tiling_shader = nullptr;

            Shape* _current_color_shape;
            Shape* _previous_color_shape;
            Shape* _frame_shape;
            Shape* _transparency_tiling_shape;

            Tooltip _tooltip;
    };

    namespace state
    {
        static inline ColorPreview* color_preview;
    }
}
