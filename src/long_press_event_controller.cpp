//
// Created by clem on 4/11/23.
//

#include <include/long_press_event_controller.hpp>

namespace mousetrap
{
    LongPressEventController::LongPressEventController()
        : SingleClickGesture(GTK_GESTURE_SINGLE(gtk_gesture_long_press_new())),
         CTOR_SIGNAL(LongPressEventController, pressed),
         CTOR_SIGNAL(LongPressEventController, press_cancelled)
    {}

    void LongPressEventController::set_delay_factor(float v)
    {
        gtk_gesture_long_press_set_delay_factor(GTK_GESTURE_LONG_PRESS(_native), v);
    }

    float LongPressEventController::get_delay_factor() const
    {
        return gtk_gesture_long_press_get_delay_factor(GTK_GESTURE_LONG_PRESS(_native));
    }
}
