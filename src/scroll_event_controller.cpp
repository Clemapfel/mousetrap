//
// Created by clem on 3/19/23.
//

#include <include/scroll_event_controller.hpp>

namespace mousetrap
{
    ScrollEventController::ScrollEventController(bool emit_vertical, bool emit_horizontal)
    : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_scroll_new([&]() {

        if (emit_vertical and emit_horizontal)
            return GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES;
        else if (emit_vertical)
            return GTK_EVENT_CONTROLLER_SCROLL_VERTICAL;
        else if (emit_horizontal)
            return GTK_EVENT_CONTROLLER_SCROLL_HORIZONTAL;
        else
            return GTK_EVENT_CONTROLLER_SCROLL_NONE;
    }()))),
        CTOR_SIGNAL(ScrollEventController, kinetic_scroll_decelerate),
        CTOR_SIGNAL(ScrollEventController, scroll_begin),
        CTOR_SIGNAL(ScrollEventController, scroll),
        CTOR_SIGNAL(ScrollEventController, scroll_end)
    {}
}
