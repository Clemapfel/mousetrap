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

// ###

namespace mousetrap
{
    Time::Time(int64_t n_nanoseconds)
            : _ns(std::chrono::nanoseconds(n_nanoseconds))
    {}

    Time minutes(double n)
    {
        return Time(int64_t(ceil(n * 1e+9) * 60));
    }

    Time seconds(double n)
    {
        return Time(int64_t(ceil(n * 1e+9)));
    }

    Time milliseconds(double n)
    {
        return Time(int64_t(ceil(n * 1e+6)));
    }

    Time microseconds(double n)
    {
        return Time(int64_t(ceil(n * 1e+3)));
    }

    Time nanoseconds(size_t n)
    {
        return Time(n);
    }

    double Time::as_minutes() const
    {
        return as_seconds() / 60;
    }

    double Time::as_seconds() const
    {
        return double(_ns.count()) / 1e+9;
    }

    double Time::as_milliseconds() const
    {
        return double(_ns.count()) / 1e+6;
    }

    double Time::as_microseconds() const
    {
        return double(_ns.count()) / 1e+3;
    }

    size_t Time::as_nanoseconds() const
    {
        return _ns.count();
    }

    Clock::Clock()
            : _start(std::chrono::steady_clock::now())
    {}

    Time Clock::elapsed()
    {
        auto now = std::chrono::steady_clock::now();

        std::chrono::duration<int64_t, std::nano> elapsed = now - _start;
        return Time(elapsed.count());
    }

    Time Clock::restart()
    {
        Time since = elapsed();
        _start = std::chrono::steady_clock::now();
        return since;
    }
}