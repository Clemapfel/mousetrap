// 
// Copyright 2022 Clemens Cords
// Created on 8/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>

namespace mousetrap
{
    static inline HSVA current_color = HSVA(0.5, 0.5, 0.5, 1);
    static inline HSVA last_color = HSVA(0.2, 0.8, 1, 1);

    class ColorPicker;
    static inline ColorPicker* color_picker = nullptr;
}