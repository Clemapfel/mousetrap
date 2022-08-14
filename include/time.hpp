// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cmath>
#include <chrono>

namespace mousetrap
{
    class Time
    {
        public:
            double as_minutes() const;
            double as_seconds() const;
            double as_milliseconds() const;
            double as_microseconds() const;
            size_t as_nanoseconds() const;

        private:
            friend Time minutes(double);
            friend Time seconds(double);
            friend Time milliseconds(double);
            friend Time microseconds(double);
            friend Time nanoseconds(size_t);

            friend class Clock;

            Time(int64_t n_nanoseconds);

            std::chrono::duration<int64_t, std::nano> _ns; // in nanoseconds
    };


    Time minutes(double);
    Time seconds(double);
    Time milliseconds(double);
    Time microseconds(double);
    Time nanoseconds(size_t ns);

    class Clock
    {
        public:
            Clock();

            Time restart();
            Time elapsed();

        private:
            std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<int64_t, std::nano>> _start;
    };
}

#include <src/time.inl>
