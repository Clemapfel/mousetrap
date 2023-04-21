//
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cmath>
#include <chrono>
#include <string>

namespace mousetrap
{
    /// @brief Duration, nanosecond precision
    class Time
    {
        public:
            /// @brief get duration as number of minutes
            /// @returns double
            double as_minutes() const;

            /// @brief get duration as number of seconds
            /// @returns double
            double as_seconds() const;

            /// @brief get duration as number of milliseconds
            /// @returns double
            double as_milliseconds() const;

            /// @brief get duration as number of microseconds
            /// @returns double
            double as_microseconds() const;

            /// @brief get duration as number of nanoseconds
            /// @returns int64_t
            int64_t as_nanoseconds() const;

            /// @brief assignment add
            /// @param other
            /// @returns reference to self after assignment
            Time& operator+=(const Time& other);

            /// @brief assignment subtract
            /// @param other
            /// @returns reference to self after assignment
            Time& operator-=(const Time& other);

            /// @brief operator add
            /// @param time
            /// @returns newly created duration
            [[nodiscard]] Time operator+(const Time&);

            /// @brief operator subtract
            /// @param time
            /// @returns newly created duration
            [[nodiscard]] Time operator-(const Time&);

            /// @brief equality comparison
            /// @param other
            /// @returns true if both duration are the same nanosecond length, false otherwise
            bool operator==(const Time& other);

            /// @brief inequality comparison
            /// @param other
            /// @returns false if both duration are the same nanosecond length, true otherwise
            bool operator!=(const Time& other);

            /// @brief less than operator
            /// @param other
            /// @returns true if nanosecond length of this is smaller than other, false otherwise
            bool operator<(const Time& other);

            /// @brief greater than operator
            /// @param other
            /// @returns false if nanosecond length of this is smaller than other, true otherwise
            bool operator>(const Time& other);

            /// @brief less than or equal operator
            /// @param other
            /// @returns true if nanosecond length of this is smaller than or equal to other, false otherwise
            bool operator<=(const Time& other);

            /// @brief greater than or equal operator
            /// @param other
            /// @returns false if nanosecond length of this is smaller than or equal to other, true otherwise
            bool operator>=(const Time& other);

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

    /// @brief construct duration from number of minutes
    /// @param n_minutes, double
    /// @returns new duration object
    [[nodiscard]] Time minutes(double);

    /// @brief construct duration from number of seconds
    /// @param n_seconds, double
    /// @returns new duration object
    [[nodiscard]] Time seconds(double);

    /// @brief construct duration from number of milliseconds
    /// @param n_milliseconds, double
    /// @returns new duration object
    [[nodiscard]] Time milliseconds(double);

    /// @brief construct duration from number of microseconds
    /// @param n_microseconds, double
    /// @returns new duration object
    [[nodiscard]] Time microseconds(double);

    /// @brief construct duration from number of nanoseconds
    /// @param n_nanoseconds, signed
    /// @returns new duration object
    [[nodiscard]] Time nanoseconds(int64_t ns);

    /// @brief timing device, counts seconds elapsed in real-time
    class Clock
    {
        public:
            /// @brief ctor, automatically starts measuring time
            Clock();

            /// @brief get elapsed time, then restart the clock
            /// @returns elapsed time since last restart / construction
            Time restart();

            /// @brief get elapsed time
            /// @returns elapsed time since last restart / construction
            Time elapsed();

        private:
            std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<int64_t, std::nano>> _start;
    };

    /// @brief get current time as timestamp
    /// @returns string
    std::string get_timestamp_now();
}
