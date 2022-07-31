// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cmath>

namespace rat
{
    /// \brief class representing an angular measurement, clockwise
    struct Angle
    {
        public:
            /// no docs
            explicit Angle(float radians);

            /// \brief convert the angle to degrees
            /// \returns degrees
            float as_degrees() const;

            /// \brief convert the angle to radians
            /// \returns radians
            float as_radians() const;

            /// \brief addition
            /// \param angle: other angle
            /// \returns new angle
            Angle operator+(const Angle&);

            /// \brief subtraction
            /// \param angle: other angle
            /// \returns new angle
            Angle operator-(const Angle&);

            /// \brief multiplication
            /// \param angle: other angle
            /// \returns new angle
            Angle operator*(const Angle&);

            /// \brief division
            /// \param angle: other angle
            /// \returns new angle
            Angle operator/(const Angle&);

            /// \brief addition assignment
            /// \param angle: other angle
            /// \returns reference to self after assignment
            Angle& operator+=(const Angle&);

            /// \brief subtraction assignment
            /// \param angle: other angle
            /// \returns reference to self after assignment
            Angle& operator-=(const Angle&);

            /// \brief multiplication assignment
            /// \param angle: other angle
            /// \returns reference to self after assignment
            Angle& operator*=(const Angle&);

            /// \brief division assignment
            /// \param angle: other angle
            /// \returns reference to self after assignment
            Angle& operator/=(const Angle&);

        private:
            float _rads;
    };

    /// \brief create a rat::Angle from degrees
    /// \param degrees: may be negative
    /// \returns angle
    Angle degrees(float);

    /// \brief create a rat::Angle from radians
    /// \param radians: may be negative
    /// \returns angle
    Angle radians(float);
}

// #############################################################################

namespace rat
{
    Angle::Angle(float radians)
            : _rads(radians)
    {}

    Angle degrees(float dg)
    {
        return Angle(dg * (M_PI / 180.f));
    }

    Angle radians(float rad)
    {
        return Angle(rad);
    }

    float Angle::as_degrees() const
    {
        return _rads * (180.f / M_PI);
    }

    float Angle::as_radians() const
    {
        return _rads;
    }

    Angle Angle::operator+(const Angle& other)
    {
        return Angle(this->_rads + other._rads);
    }

    Angle Angle::operator-(const Angle& other)
    {
        return Angle(this->_rads - other._rads);
    }

    Angle Angle::operator*(const Angle& other)
    {
        return Angle(this->_rads * other._rads);
    }

    Angle Angle::operator/(const Angle& other)
    {
        return Angle(this->_rads / other._rads);
    }

    Angle& Angle::operator+=(const Angle& other)
    {
        this->_rads += other._rads;
        return *this;
    }

    Angle& Angle::operator-=(const Angle& other)
    {
        this->_rads -= other._rads;
        return *this;
    }

    Angle& Angle::operator*=(const Angle& other)
    {
        this->_rads *= other._rads;
        return *this;
    }

    Angle& Angle::operator/=(const Angle& other)
    {
        this->_rads /= other._rads;
        return *this;
    }
}