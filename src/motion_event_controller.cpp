//
// Created by clem on 3/18/23.
//

#include <mousetrap/motion_event_controller.hpp>

namespace mousetrap
{
    MotionEventController::MotionEventController()
        : EventController(gtk_event_controller_motion_new()),
          CTOR_SIGNAL(MotionEventController, motion_enter),
          CTOR_SIGNAL(MotionEventController, motion),
          CTOR_SIGNAL(MotionEventController, motion_leave)
    {}

    MotionEventController::MotionEventController(detail::MotionEventControllerInternal* internal)
        : EventController(internal),
          CTOR_SIGNAL(MotionEventController, motion_enter),
          CTOR_SIGNAL(MotionEventController, motion),
          CTOR_SIGNAL(MotionEventController, motion_leave)
    {}
}
