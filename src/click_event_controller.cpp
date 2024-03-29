//
// Created by clem on 3/19/23.
//

#include <mousetrap/click_event_controller.hpp>

namespace mousetrap
{
    ClickEventController::ClickEventController()
        : SingleClickGesture(GTK_GESTURE_SINGLE(gtk_gesture_click_new())),
          CTOR_SIGNAL(ClickEventController, click_pressed),
          CTOR_SIGNAL(ClickEventController, click_released),
          CTOR_SIGNAL(ClickEventController, click_stopped)
    {}

    ClickEventController::ClickEventController(detail::ClickEventControllerInternal* internal)
        : SingleClickGesture(GTK_GESTURE_SINGLE(internal)),
          CTOR_SIGNAL(ClickEventController, click_pressed),
          CTOR_SIGNAL(ClickEventController, click_released),
          CTOR_SIGNAL(ClickEventController, click_stopped)
    {}
}
