//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <mousetrap/event_controller.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.hpp"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class FocusEventController;
    namespace detail
    {
        using FocusEventControllerInternal = EventControllerInternal;
        DEFINE_INTERNAL_MAPPING(FocusEventController);
    }
    #endif

    /// @brief event controller that triggers when focus is moved to the widget
    /// \signals
    /// \signal_focus_gained{FocusEventController}
    /// \signal_focus_lost{FocusEventController}
    class FocusEventController : public EventController,
        HAS_SIGNAL(FocusEventController, focus_gained),
        HAS_SIGNAL(FocusEventController, focus_lost)
    {
        public:
            /// @brief construct
            FocusEventController();

            /// @brief construct from internal
            FocusEventController(detail::FocusEventControllerInternal*);

            /// @brief get whether focus is contained within self or any child
            /// @return bool
            bool self_or_child_is_focused();

            /// @brief get whether focus is contained within self only and not any child
            /// @return bool
            bool self_is_focused();
    };
}