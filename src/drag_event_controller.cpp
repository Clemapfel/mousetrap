//
// Created by clem on 3/20/23.
//

#include <include/drag_event_controller.hpp>

namespace mousetrap
{
    DragEventController::DragEventController()
    : SingleClickGesture(GTK_GESTURE_SINGLE(gtk_gesture_drag_new())),
      CTOR_SIGNAL(DragEventController, drag_begin),
      CTOR_SIGNAL(DragEventController, drag),
      CTOR_SIGNAL(DragEventController, drag_end)
    {}

    Vector2f DragEventController::get_start_position()
    {
        double x, y;
        gtk_gesture_drag_get_start_point(GTK_GESTURE_DRAG(_native), &x, &y);
        return Vector2f(x, y);
    }

    Vector2f DragEventController::get_current_offset()
    {
        double x, y;
        gtk_gesture_drag_get_offset(GTK_GESTURE_DRAG(_native), &x, &y);
        return Vector2f(x, y);
    }
}
