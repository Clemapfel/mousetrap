//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/11/23
//

#pragma once

#include <include/event_controller.hpp>

namespace mousetrap
{
    /// @brief event controller that listens for long press gestures
    class LongPressEventController : public SingleClickGesture,
        HAS_SIGNAL(LongPressEventController, pressed),
        HAS_SIGNAL(LongPressEventController, press_cancelled)
    {
        public:
            /// @brief construct
            LongPressEventController();

            /// @brief set the amount of time that has to pass until a long press is registered
            /// @param factor multiplier with the standard duration, 1 means no modification, 2 means twice as long of a delay, 0.5 half as a long
            void set_delay_factor(float);

            /// @brief get amount of time that has to pass until a long press is registered
            /// @return multiplier with the standard duration, 1 means no modification, 2 means twice as long of a delay, 0.5 half as a long
            float get_delay_factor() const;
    };
}
