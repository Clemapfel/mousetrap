//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#include <include/rotate_event_controller.hpp>

namespace mousetrap
{
    RotateEventController::RotateEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_rotate_new())),
          CTOR_SIGNAL(RotateEventController, rotation_changed)
    {}

    Angle RotateEventController::get_angle_delta()
    {
        return radians(gtk_gesture_rotate_get_angle_delta(GTK_GESTURE_ROTATE(_native)));
    }
}