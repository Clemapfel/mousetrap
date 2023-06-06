//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/11/23
//

#pragma once

#include <mousetrap/event_controller.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class SwipeEventController;
    namespace detail
    {
        using SwipeEventControllerInternal = EventControllerInternal;
        DEFINE_INTERNAL_MAPPING(SwipeEventController);
    }
    #endif
    
    /// @brief event controller that listens for long press gestures
    /// \signals
    /// \signal_swipe{SwipeEventController}
    class SwipeEventController : public SingleClickGesture,
        HAS_SIGNAL(SwipeEventController, swipe)
    {
        public:
            /// @brief construct
            /// @param orientation
            SwipeEventController(Orientation);

            /// @brief construct from internal
            SwipeEventController(detail::SwipeEventControllerInternal*);

            /// @brief get current swipe velocity
            /// @return 2D velocity
            Vector2f get_velocity() const;
    };
}
