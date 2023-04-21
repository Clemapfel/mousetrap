//
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cmath>

namespace mousetrap
{
    class Angle;

    /// @brief construct angle from degrees
    /// @return mousetrap::Angle
    [[nodiscard]] Angle degrees(float);

    /// @brief construct from radians
    /// @return mousetrap::Angle
    [[nodiscard]] Angle radians(float);

    /// @brief object representing an angle, unit agnostic
    struct Angle
    {
        friend Angle degrees(float);
        friend Angle radians(float);

        public:
            /// @brief convert angle to degrees
            /// @return float, in degrees
            float as_degrees() const;

            /// @brief convert angle to radians
            /// @return float, in radians
            float as_radians() const;

            /// @brief addition operator
            /// @param other
            /// @return mousetrap::Angle
            [[nodiscard]] Angle operator+(const Angle&) const;

            /// @brief subtraction operator
            /// @param other
            /// @return mousetrap::Angle
            [[nodiscard]] Angle operator-(const Angle&) const;

            /// @brief multiplication operator
            /// @param other
            /// @return mousetrap::Angle
            Angle operator*(const Angle&) const;

            /// @brief division operator
            /// @param other
            /// @return mousetrap::Angle
            Angle operator/(const Angle&) const;

            /// @brief addition assignment operator
            /// @param other
            /// @return mousetrap::Angle
            Angle& operator+=(const Angle&);

            /// @brief subtraction assignment operator
            /// @param other
            /// @return mousetrap::Angle
            Angle& operator-=(const Angle&);

            /// @brief multiplication assignment operator
            /// @param other
            /// @return mousetrap::Angle
            Angle& operator*=(const Angle&);

            /// @brief division assignment operator
            /// @param other
            /// @return mousetrap::Angle
            Angle& operator/=(const Angle&);

            /// @brief comparison operator
            /// @param other
            /// @return true if value as radians are equal, false otherwise
            bool operator==(const Angle&) const;

            /// @brief comparison operator
            /// @param other
            /// @return false if value as radians are equal, true otherwise
            bool operator!=(const Angle&) const;

            /// @brief construct from radians
            /// @param radians angle in radians
            explicit Angle(float radians);

        private:
            float _rads;
    };
}

#include <iostream>
