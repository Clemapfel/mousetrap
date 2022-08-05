// 
// Copyright 2022 Clemens Cords
// Created on 8/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>

namespace mousetrap
{
    static inline HSVA current_color = HSVA(0.5, 0.5, 0.5, 1);

    struct Updatable
    {
        virtual void update() = 0;
    };

    static inline Updatable* color_picker = nullptr;
    static inline Updatable* hsv_triangle_select = nullptr;

    class HsvTriangleSelect;
    static HsvTriangleSelect* get_hsv_triangle_select()
    {
        return (HsvTriangleSelect*) hsv_triangle_select;
    }

    class ColorPicker;
    static ColorPicker* get_color_picker()
    {
        return (ColorPicker*) color_picker;
    }

    static void signal_color_updated()
    {
        color_picker->update();
        hsv_triangle_select->update();
    }
}