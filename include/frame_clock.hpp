//
// Copyright 2022 Clemens Cords
// Created on 11/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <include/time.hpp>
#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief clock that is updated every render unit
    class FrameClock : public SignalEmitter,
        HAS_SIGNAL(FrameClock, update),
        HAS_SIGNAL(FrameClock, paint)
    {
        public:
            /// @brief construct from GdkFrameClock \internal. Use the callback of widget::add_tick_callback to get an initialized mousetrap::FrameClock
            /// @param clock
            FrameClock(GdkFrameClock*);

            /// @brief destruct
            ~FrameClock();

            /// @brief copy ctor deleted
            FrameClock(const FrameClock&) = delete;

            /// @brief move ctor, safely transfers ownership
            /// @param other
            FrameClock(FrameClock&&) noexcept;

            /// @brief copy assignment deleted
            FrameClock& operator=(const FrameClock&) = delete;

            /// @brief move assignment, safely transfers ownership
            /// @param other
            /// @return self after mutation
            FrameClock& operator=(FrameClock&&) noexcept;

            /// @brief expose as GObject \internal
            operator GObject*() const;

            /// @brief get duration of one frame
            /// @return mousetrap::Time
            Time get_frame_time() const;

            /// @brief get duration since the laste render cycle
            /// @return mousetrap::Time
            Time get_time_since_last_frame() const;

            /// @brief get effective fps of the render loop
            float get_fps() const;

        private:
            GdkFrameClock* _native = nullptr;
    };
}
