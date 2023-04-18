//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/15/23
//

#pragma once

#include <include/gl_common.hpp>

namespace mousetrap
{
    /// @brief hardware-accelerated scaling mode for GPU-side textures
    enum class TextureScaleMode
    {
        /// @brief nearest neighbor scaling
        NEAREST = GL_NEAREST,

        /// @brief linear interpolation
        LINEAR = GL_LINEAR
    };
}
