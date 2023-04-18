//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <include/gtk_common.hpp>

namespace mousetrap
{
    /// @brief signifies position of A relative to B
    enum class RelativePosition
    {
        /// @brief A is above B
        ABOVE = GTK_POS_TOP,

        /// @brief A is left of B
        LEFT_OF = GTK_POS_LEFT,

        /// @brief A is right of B
        RIGHT_OF = GTK_POS_RIGHT,

        /// @brief A is below B
        BELOW = GTK_POS_BOTTOM
    };
}
