//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/18/23
//

#pragma once

#include <mousetrap/event_controller.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief handles cursor motion events
    /// \signals
    /// \signal_motion_enter{MotionEventController}
    /// \siganl_motion{MotionEventController}
    /// \signal_motion_leave{MotionEventController}
    class MotionEventController : public EventController,
        HAS_SIGNAL(MotionEventController, motion_enter),
        HAS_SIGNAL(MotionEventController, motion),
        HAS_SIGNAL(MotionEventController, motion_leave)
    {
        public:
            /// @brief ctor, needs to be connected to a widget to start emitting events
            MotionEventController();
    };
}