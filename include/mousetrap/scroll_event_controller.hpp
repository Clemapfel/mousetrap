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
    class ScrollEventController;
    namespace detail
    {
        using ScrollEventControllerInternal = EventControllerInternal;
        DEFINE_INTERNAL_MAPPING(ScrollEventController);
    }
    #endif
    
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
            /// @param allow_kinetic should kinetic scroll be enabled
            ScrollEventController(bool allow_kinetic = false);

            /// @brief set whether kinetic scrolling should be enabled
            /// @param b true if enabled, false otherwise
            void set_kinetic_scrolling_enabled(bool b);

            /// @brief get whether kinetic scrolling is enabled
            /// @return true if enabled, false otherwise
            bool get_kinetic_scrolling_enabled() const;

            /// @brief construct from internal
            ScrollEventController(detail::ScrollEventControllerInternal*);
    };
}