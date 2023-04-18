//
// Created by clem on 4/11/23.
//

#include <include/swipe_event_controller.hpp>

namespace mousetrap
{
    SwipeEventController::SwipeEventController(Orientation orientation)
        : SingleClickGesture(GTK_GESTURE_SINGLE(gtk_gesture_swipe_new())),
          CTOR_SIGNAL(SwipeEventController, swipe)
    {}

    Vector2f SwipeEventController::get_velocty() const
    {
        double x, y;
        if (gtk_gesture_swipe_get_velocity(GTK_GESTURE_SWIPE(_native), &x, &y))
            return Vector2f(x, y);
        else
            return Vector2f(0, 0);
    }
}
