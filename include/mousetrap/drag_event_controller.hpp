//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <mousetrap/event_controller.hpp>
#include <mousetrap/vector.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief event controller, handles click-dragging
    /// \signals
    /// \signal_drag_begin{DragEventController}
    /// \signal_drag{DragEventController}
    /// \signal_drag_end{DragEventController}
    class DragEventController : public SingleClickGesture,
        HAS_SIGNAL(DragEventController, drag_begin),
        HAS_SIGNAL(DragEventController, drag),
        HAS_SIGNAL(DragEventController, drag_end)
    {
        public:
            /// @brief construct, needs to be connected to a widget to start signal emission
            DragEventController();

            /// @brief get initial position for currently active drag
            Vector2f get_start_position();

            /// @brief get current distance from initial position of currently active drag
            Vector2f get_current_offset();
    };
}