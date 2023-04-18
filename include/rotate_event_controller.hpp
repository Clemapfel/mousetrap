//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <include/event_controller.hpp>
#include <include/angle.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief event controller handling touch rotation, or "turn" events
    class RotateEventController : public EventController,
        HAS_SIGNAL(RotateEventController, rotation_changed)
    {
        public:
            /// @brief construct, needs to be attached to a widget to start emitting events
            RotateEventController();

            /// @brief get difference between angle at the start of the gesture and the angle currently
            /// @returns angle
            Angle get_angle_delta();
    };
}
