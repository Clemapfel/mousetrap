//
// Created by clem on 3/19/23.
//

#include <mousetrap/focus_event_controller.hpp>

namespace mousetrap
{
    FocusEventController::FocusEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_focus_new())),
          CTOR_SIGNAL(FocusEventController, focus_gained),
          CTOR_SIGNAL(FocusEventController, focus_lost)
    {}

    FocusEventController::FocusEventController(detail::FocusEventControllerInternal* internal)
        : EventController(internal),
          CTOR_SIGNAL(FocusEventController, focus_gained),
          CTOR_SIGNAL(FocusEventController, focus_lost)
    {}

    bool FocusEventController::self_is_focused()
    {
        return gtk_event_controller_focus_contains_focus(GTK_EVENT_CONTROLLER_FOCUS(get_internal()));
    }

    bool FocusEventController::self_or_child_is_focused()
    {
        return gtk_event_controller_focus_is_focus(GTK_EVENT_CONTROLLER_FOCUS(get_internal()));
    }
}