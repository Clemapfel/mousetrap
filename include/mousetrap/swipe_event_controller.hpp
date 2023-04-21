//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/11/23
//

#pragma once

#include <mousetrap/event_controller.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    /// @brief event controller that listens for long press gestures
    class SwipeEventController : public SingleClickGesture,
    HAS_SIGNAL(SwipeEventController, swipe)
    {
        public:
            /// @brief construct
            /// @param orientation
            SwipeEventController(Orientation);

            /// @brief get current swipe velocity
            /// @return 2D velocity
            Vector2f get_velocty() const;
    };
}
