// 
// Copyright 2022 Clemens Cords
// Created on 8/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>
#include <include/updatable.hpp>

namespace mousetrap
{
    // colors

    static inline HSVA current_color = HSVA(0.5, 0.5, 0.5, 1);

    static void signal_color_updated();

    static inline Updatable* color_picker = nullptr;
    static inline Updatable* hsv_triangle_select = nullptr;}

// ###

namespace mousetrap
{
    static void signal_color_updated()
    {
        if (color_picker != nullptr)
            color_picker->update();

        if (hsv_triangle_select != nullptr)
            hsv_triangle_select->update();
    }
}