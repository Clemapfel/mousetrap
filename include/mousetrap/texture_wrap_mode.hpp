//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/15/23
//

#pragma once

#include <mousetrap/gl_common.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

namespace mousetrap
{
    /// @brief texture wrap mode, governs what is displayed when a vertex' texture coordinate is outside of [0, 1]
    enum class TextureWrapMode
    {
        /// @brief display RGBA(0, 0, 0, 0)
        ZERO = 0,

        /// @brief display RGBA(1, 1, 1, 1)
        ONE = 1,

        /// @brief repeat the texture along its boundaries, resulting in tiling
        REPEAT = GL_REPEAT,

        /// @brief mirror the texture along its boundaries
        MIRROR = GL_MIRRORED_REPEAT,

        /// @brief display the color of the closest valid texture coordinate
        STRETCH = GL_CLAMP_TO_EDGE
    };
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT