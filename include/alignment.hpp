//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/15/23
//

#pragma once

#include <include/gtk_common.hpp>

namespace mousetrap
{
    /// @brief alignment along horizontal or vertical axis
    enum class Alignment
    {
        /// @brief align to start of range, left if horizontal, top if vertical
        START = GTK_ALIGN_START,
        /// @brief align to center of range
        CENTER = GTK_ALIGN_CENTER,
        /// @brief align to end of range, right if horizontal, bottom if vertical
        END = GTK_ALIGN_END
    };
}
