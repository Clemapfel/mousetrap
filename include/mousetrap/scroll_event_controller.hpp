//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <mousetrap/event_controller.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief event controller that handles the scrollbar, usually on a mouse
    /// \signals
    /// \signal_kinetic_scroll_decelerate{ScrollEventController}
    /// \signal_scroll_begin{ScrollEventController}
    /// \signal_scroll{ScrollEventController}
    /// \signal_scroll_end{ScrollEventController}
    class ScrollEventController : public EventController,
        HAS_SIGNAL(ScrollEventController, kinetic_scroll_decelerate),
        HAS_SIGNAL(ScrollEventController, scroll_begin),
        HAS_SIGNAL(ScrollEventController, scroll),
        HAS_SIGNAL(ScrollEventController, scroll_end)
    {
        public:
            /// @brief construct, needs to be attached to a widget to start emitting events
            /// @param emit_vertical should vertical scrolling trigger events
            /// @param emit_horizontal should horizontal scrolling trigger evets
            ScrollEventController(bool emit_vertical = true, bool emit_horizontal = true);
    };
}