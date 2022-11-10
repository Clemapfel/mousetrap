// 
// Copyright 2022 Clemens Cords
// Created on 11/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <include/time.hpp>

namespace mousetrap
{
    class FrameClock : public SignalEmitter,
        public HasUpdateSignal<FrameClock> // emitted at start of frame before anything else happens
    {
        public:
            FrameClock(GdkFrameClock*);

            operator GObject*();

            /// \brief timestamp
            Time get_frame_time();

            /// \brief duration
            Time get_time_since_last_frame();

            float get_fps();

            void start();
            void stop();

        private:
            GdkFrameClock* _native;
    };
}

#include <src/frame_clock.inl>