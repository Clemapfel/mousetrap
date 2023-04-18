//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <include/event_controller.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief event controller handles button pressed
    class ClickEventController : public SingleClickGesture,
        HAS_SIGNAL(ClickEventController, click_pressed),
        HAS_SIGNAL(ClickEventController, click_released),
        HAS_SIGNAL(ClickEventController, click_stopped)
    {
        public:
            /// @brief construct controller, needs to be connected to a widget to function
            ClickEventController();


    };
}