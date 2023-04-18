//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/11/23
//

#pragma once

#include <include/event_controller.hpp>
#include <include/orientation.hpp>

namespace mousetrap
{
    /// @brief event controller that listens for long press gestures
    class PanEventController : public EventController,
        HAS_SIGNAL(PanEventController, pan)
    {
        public:
            /// @brief construct
            PanEventController(Orientation);

            /// @brief set recognized swipe orientation
            /// @param orientation
            void set_orientation(Orientation);

            /// @brief get recognized swipe orientation
            /// @return orientation
            Orientation get_orientation() const;
    };
}
