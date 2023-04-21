//
// Copyright 2022 Clemens Cords
// Created on 9/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

namespace mousetrap
{
    /// @brief render blend mode
    enum BlendMode
    {
        /// @brief no blending
        NONE,

        /// @brief alpha blending, treat alpha value as emission
        NORMAL,

        /// @brief additive blending, out = source + destination
        ADD,

        /// @brief subtractive blending, out = source - destination
        SUBTRACT,

        /// @brief reverse subtractive blending, out = destination - source
        REVERSE_SUBTRACT,

        /// @brief multiplicative blending, out = destination * source
        MULTIPLY,

        /// @brief minimum blending, out = min(destination, source)
        MIN,

        /// @brief maximum blending, out = max(destination, source)
        MAX
    };

    /// @brief set blend mode of currently bound OpenGL context to blend mode
    /// @param blend_mode
    /// @param allow_alpha_blend if true, blendmode affects both the rgb and alpha component of each pixel, if false, only rgb is affected
    void set_current_blend_mode(BlendMode, bool allow_alpha_blend = true);

    /// @brief serialize blend mode
    /// @param blend_mode
    /// @return string
    std::string blend_mode_to_string(BlendMode);

    /// @brief deserialize blend mode
    /// @param string
    /// @return mousetrap::BlendMode
    BlendMode blend_mode_from_string(const std::string&);
}
