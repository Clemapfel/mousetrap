//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/15/23
//

#pragma once

#include <include/gtk_common.hpp>

namespace mousetrap
{
    /// @brief orientation of an object
    enum class Orientation
    {
        /// @brief oriented along the x axis, left to right
        HORIZONTAL = GTK_ORIENTATION_HORIZONTAL,

        /// @brief oriented along the y axis, top to bottom
        VERTICAL = GTK_ORIENTATION_VERTICAL
    };
}