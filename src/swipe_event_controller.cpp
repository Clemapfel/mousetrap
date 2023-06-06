//
// Created by clem on 4/11/23.
//

#include <mousetrap/swipe_event_controller.hpp>

namespace mousetrap
{
    SwipeEventController::SwipeEventController(Orientation orientation)
        : SingleClickGesture(GTK_GESTURE_SINGLE(gtk_gesture_swipe_new())),
          CTOR_SIGNAL(SwipeEventController, swipe)
    {}

    SwipeEventController::SwipeEventController(detail::SwipeEventControllerInternal* internal)
        : SingleClickGesture(GTK_GESTURE_SINGLE(internal)),
          CTOR_SIGNAL(SwipeEventController, swipe)
    {}

    Vector2f SwipeEventController::get_velocity() const
    {
        double x, y;
        if (gtk_gesture_swipe_get_velocity(GTK_GESTURE_SWIPE(get_internal()), &x, &y))
            return Vector2f(x, y);
        else
            return Vector2f(0, 0);
    }
}
