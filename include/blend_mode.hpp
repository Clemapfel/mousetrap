// 
// Copyright 2022 Clemens Cords
// Created on 9/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

namespace mousetrap
{
    enum BlendMode
    {
        NONE,       // no blending
        NORMAL,     // alpha blending
        ADD,        // src + dest
        SUBTRACT,   // src - dest
        REVERSE_SUBTRACT, // dest - src
        MULTIPLY,   // src * dest
        MIN,        // min(src, dest)
        MAX         // max(src, dest)
    };

    /// \param allow_alpha_blend: should resulting alpha be affected by blendmode
    void set_current_blend_mode(BlendMode, bool allow_alpha_blend = true);

    std::string blend_mode_to_string(BlendMode);
    BlendMode blend_mode_from_string(const std::string&);
}
