//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <mousetrap/event_controller.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.hpp"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class PinchZoomEventController;
    namespace detail
    {
        using PinchZoomEventControllerInternal = EventControllerInternal;
        DEFINE_INTERNAL_MAPPING(PinchZoomEventController);
    }
    #endif
    
    /// @brief event controller, handles the two-finger pinch-zoom gestures
    /// \signals
    /// \signal_scale_changed{PinchZoomEventController}
    class PinchZoomEventController : public EventController,
        HAS_SIGNAL(PinchZoomEventController, scale_changed)
    {
        public:
            /// @brief construct, needs to be connected to a widget to start emitting events
            PinchZoomEventController();

            /// @brief construct from internal
            PinchZoomEventController(detail::PinchZoomEventControllerInternal*);

            /// @brief get difference between distance between the two finger at the start of the gesture, and distance between them currently
            float get_scale_delta();
    };
}