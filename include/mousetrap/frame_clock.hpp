//
// Copyright 2022 Clemens Cords
// Created on 11/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/time.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class FrameClock;
    namespace detail
    {
        using FrameClockInternal = GdkFrameClock;
        DEFINE_INTERNAL_MAPPING(FrameClock);
    }
    #endif

    /// @brief clock that is updated every render unit
    /// \signals
    /// \signal_update{FrameClock}
    /// \signal_paint{FrameClock}
    class FrameClock : public SignalEmitter,
        HAS_SIGNAL(FrameClock, update),
        HAS_SIGNAL(FrameClock, paint)
    {
        public:
            /// @brief construct from GdkFrameClock \for_internal_use_only. Use the callback of widget::add_tick_callback to get an initialized mousetrap::FrameClock
            /// @param clock
            FrameClock(detail::FrameClockInternal* clock);

            /// @brief destruct
            ~FrameClock();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief copy ctor deleted
            FrameClock(const FrameClock&);

            /// @brief move ctor, safely transfers ownership
            /// @param other
            FrameClock(FrameClock&& other) noexcept;

            /// @brief copy assignment deleted
            FrameClock& operator=(const FrameClock&);

            /// @brief move assignment, safely transfers ownership
            /// @param other
            /// @return self after mutation
            FrameClock& operator=(FrameClock&& other) noexcept;

            /// @brief expose as GObject \for_internal_use_only
            operator GObject*() const;

            /// @brief get duration of one frame
            /// @return mousetrap::Time
            Time get_target_frame_duration() const;

            /// @brief get duration since the laste render cycle
            /// @return mousetrap::Time
            Time get_time_since_last_frame() const;

            /// @brief get effective fps of the render loop
            float get_fps() const;

        private:
            detail::FrameClockInternal* _internal = nullptr;
    };
}
