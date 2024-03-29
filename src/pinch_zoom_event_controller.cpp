//
// Created by clem on 3/20/23.
//

#include <mousetrap/pinch_zoom_event_controller.hpp>

namespace mousetrap
{
    PinchZoomEventController::PinchZoomEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_zoom_new())),
          CTOR_SIGNAL(PinchZoomEventController, scale_changed)
    {}

    PinchZoomEventController::PinchZoomEventController(detail::PinchZoomEventControllerInternal* internal)
        : EventController(internal),
          CTOR_SIGNAL(PinchZoomEventController, scale_changed)
    {}

    float PinchZoomEventController::get_scale_delta()
    {
        return gtk_gesture_zoom_get_scale_delta(GTK_GESTURE_ZOOM(get_internal()));
    }
}
