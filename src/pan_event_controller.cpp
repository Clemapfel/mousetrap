//
// Created by clem on 4/11/23.
//

#include <include/pan_event_controller.hpp>

namespace mousetrap
{
    PanEventController::PanEventController(Orientation orientation)
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_pan_new((GtkOrientation) orientation))),
          CTOR_SIGNAL(PanEventController, pan)
    {}

    Orientation PanEventController::get_orientation() const
    {
        return (Orientation) gtk_gesture_pan_get_orientation(GTK_GESTURE_PAN(_native));
    }

    void PanEventController::set_orientation(Orientation orientation)
    {
        gtk_gesture_pan_set_orientation(GTK_GESTURE_PAN(_native), (GtkOrientation) orientation);
    }
}
