//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <include/gtk_common.hpp>

namespace mousetrap
{
    /// @brief text justify mode
    enum class JustifyMode
    {
        /// @brief anchor lines on the left margin
        LEFT = GTK_JUSTIFY_LEFT,

        /// @brief anchor lines on the right margin
        RIGHT = GTK_JUSTIFY_RIGHT,

        /// @brief anchor centroid of lines in the center between left and right margins
        CENTER = GTK_JUSTIFY_CENTER,

        /// @brief spread each line such that it touches both the left and right margin if possible
        FILL = GTK_JUSTIFY_FILL
    };
}