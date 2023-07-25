//
// Created by clem on 3/19/23.
//

#include <mousetrap/scroll_event_controller.hpp>

namespace mousetrap
{
    ScrollEventController::ScrollEventController(bool allow_kinetic)
    : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_scroll_new([&]() {
        int flags = GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES;
        if (allow_kinetic)
            flags |= GTK_EVENT_CONTROLLER_SCROLL_KINETIC;
        return (GtkEventControllerScrollFlags) flags;
    }()))),
        CTOR_SIGNAL(ScrollEventController, kinetic_scroll_decelerate),
        CTOR_SIGNAL(ScrollEventController, scroll_begin),
        CTOR_SIGNAL(ScrollEventController, scroll),
        CTOR_SIGNAL(ScrollEventController, scroll_end)
    {}

    void ScrollEventController::set_kinetic_scrolling_enabled(bool b)
    {
        int flags = GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES;
        if (b)
            flags |= GTK_EVENT_CONTROLLER_SCROLL_KINETIC;

        gtk_event_controller_scroll_set_flags(GTK_EVENT_CONTROLLER_SCROLL(EventController::get_internal()), (GtkEventControllerScrollFlags) flags);
    }

    bool ScrollEventController::get_kinetic_scrolling_enabled() const
    {
        int flags = gtk_event_controller_scroll_get_flags(GTK_EVENT_CONTROLLER_SCROLL(EventController::get_internal()));
        return (flags & GTK_EVENT_CONTROLLER_SCROLL_KINETIC) != 0;
    }

    ScrollEventController::ScrollEventController(detail::ScrollEventControllerInternal* internal)
        : EventController(internal),
          CTOR_SIGNAL(ScrollEventController, kinetic_scroll_decelerate),
          CTOR_SIGNAL(ScrollEventController, scroll_begin),
          CTOR_SIGNAL(ScrollEventController, scroll),
          CTOR_SIGNAL(ScrollEventController, scroll_end)
    {}
}
