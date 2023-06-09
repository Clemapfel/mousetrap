//
// Created by clem on 11/10/22.
//

#include <mousetrap/frame_clock.hpp>

namespace mousetrap
{
    FrameClock::operator GObject*() const
    {
        return G_OBJECT(_internal);
    }

    NativeObject FrameClock::get_internal() const
    {
        return operator GObject*();
    }

    FrameClock::FrameClock(GdkFrameClock* internal)
        : _internal(internal),
          CTOR_SIGNAL(FrameClock, update),
          CTOR_SIGNAL(FrameClock, paint)
    {
        g_object_ref(_internal);
    }

    FrameClock::~FrameClock()
    {
        g_object_unref(_internal);
    }

    FrameClock::FrameClock(FrameClock&& other) noexcept
        : CTOR_SIGNAL(FrameClock, update),
          CTOR_SIGNAL(FrameClock, paint)
    {
        _internal = g_object_ref(other._internal);
        other._internal = nullptr;
    }

    FrameClock& FrameClock::operator=(FrameClock&& other) noexcept
    {
        _internal = g_object_ref(other._internal);
        other._internal = nullptr;
        return *this;
    }

    Time FrameClock::get_target_frame_duration() const
    {
        return microseconds(gdk_frame_clock_get_frame_time(_internal));
    }

    Time FrameClock::get_time_since_last_frame() const
    {
        auto* timings = gdk_frame_clock_get_current_timings(_internal);

        auto start = gdk_frame_timings_get_frame_time(timings);
        auto end = gdk_frame_timings_get_complete(timings) ? gdk_frame_timings_get_presentation_time(timings) : gdk_frame_timings_get_predicted_presentation_time(timings);

        return microseconds(end - start);
    }

    float FrameClock::get_fps() const
    {
        return gdk_frame_clock_get_fps(_internal);
    }

    /*
    void FrameClock::start()
    {
        gdk_frame_clock_begin_updating(_internal);
    }

    void FrameClock::stop()
    {
        gdk_frame_clock_end_updating(_internal);
    }
     */
}