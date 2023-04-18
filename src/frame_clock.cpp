//
// Created by clem on 11/10/22.
//

#include <include/frame_clock.hpp>

namespace mousetrap
{
    FrameClock::operator GObject*() const
    {
        return G_OBJECT(_native);
    }

    FrameClock::FrameClock(GdkFrameClock* native)
        : _native(native),
          CTOR_SIGNAL(FrameClock, update),
          CTOR_SIGNAL(FrameClock, paint)
    {
        g_object_ref(_native);
    }

    FrameClock::~FrameClock()
    {
        if (_native != nullptr)
            g_object_unref(_native);
    }

    FrameClock::FrameClock(FrameClock&& other) noexcept
        : CTOR_SIGNAL(FrameClock, update),
          CTOR_SIGNAL(FrameClock, paint)
    {
        _native = other._native;
        other._native = nullptr;
    }

    FrameClock& FrameClock::operator=(FrameClock&& other) noexcept
    {
        _native = other._native;
        other._native = nullptr;
        return *this;
    }

    Time FrameClock::get_frame_time() const
    {
        return microseconds(gdk_frame_clock_get_frame_time(_native));
    }

    Time FrameClock::get_time_since_last_frame() const
    {
        auto* timings = gdk_frame_clock_get_current_timings(_native);

        auto start = gdk_frame_timings_get_frame_time(timings);
        auto end = gdk_frame_timings_get_complete(timings) ? gdk_frame_timings_get_presentation_time(timings) : gdk_frame_timings_get_predicted_presentation_time(timings);

        return microseconds(end - start);
    }

    float FrameClock::get_fps() const
    {
        return gdk_frame_clock_get_fps(_native);
    }

    /*
    void FrameClock::start()
    {
        gdk_frame_clock_begin_updating(_native);
    }

    void FrameClock::stop()
    {
        gdk_frame_clock_end_updating(_native);
    }
     */
}