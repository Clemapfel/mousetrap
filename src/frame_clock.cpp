//
// Created by clem on 11/10/22.
//

#include <include/frame_clock.hpp>

namespace mousetrap
{
    FrameClock::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    FrameClock::FrameClock(GdkFrameClock* native)
        : HasUpdateSignal<FrameClock>(this), _native(native)
    {}

    Time FrameClock::get_frame_time()
    {
        return microseconds(gdk_frame_clock_get_frame_time(_native));
    }

    Time FrameClock::get_time_since_last_frame()
    {
        auto* timings = gdk_frame_clock_get_current_timings(_native);

        auto start = gdk_frame_timings_get_frame_time(timings);
        auto end = gdk_frame_timings_get_complete(timings) ? gdk_frame_timings_get_presentation_time(timings) : gdk_frame_timings_get_predicted_presentation_time(timings);

        return microseconds(end - start);
    }

    float FrameClock::get_fps()
    {
        return gdk_frame_clock_get_fps(_native);
    }

    void FrameClock::start()
    {
        gdk_frame_clock_begin_updating(_native);
    }

    void FrameClock::stop()
    {
        gdk_frame_clock_end_updating(_native);
    }
}