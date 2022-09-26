// 
// Copyright 2022 Clemens Cords
// Created on 9/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    enum BlendMode
    {
        NORMAL,     // alpha blending
        ADD,        // src + dest
        SUBTRACT,   // src - dest
        REVERSE_SUBTRACT, // dest - src
        MULTIPLY,   // src * dest
        MIN,        // min(src, dest)
        MAX         // max(src, dest)
    };

    static void set_current_blend_mode(BlendMode);
    static BlendMode get_current_blend_mode();
}

#include <src/blend_mode.inl>
