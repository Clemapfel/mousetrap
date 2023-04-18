//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/11/23
//

#pragma once

#include <include/event_controller.hpp>
#include <include/orientation.hpp>

namespace mousetrap
{
    /// @brief event controller that listens for long press gestures
    class SwipeEventController : public SingleClickGesture,
    HAS_SIGNAL(SwipeEventController, swipe)
    {
        public:
            /// @brief construct
            SwipeEventController(Orientation);

           /// @brief get current swipe velocity
           Vector2f get_velocty() const;
    };
}
