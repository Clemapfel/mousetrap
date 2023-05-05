//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/24/23
//

#pragma once

#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    /// @brief can switch between horizontal and vertical orientation
    class Orientable
    {
        public:
            /// @brief get orientation
            /// @return orientation
            virtual Orientation get_orientation() const = 0;

            /// @brief set orientation
            /// @param orientation
            virtual void set_orientation(Orientation orientation) = 0;
    };
}