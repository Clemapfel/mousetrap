//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/angle.hpp>

namespace mousetrap
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

    Angle Angle::operator+(const Angle& other) const
    {
        return Angle(this->_rads + other._rads);
    }

    Angle Angle::operator-(const Angle& other) const
    {
        return Angle(this->_rads - other._rads);
    }

    Angle Angle::operator*(const Angle& other) const
    {
        return Angle(this->_rads * other._rads);
    }

    Angle Angle::operator/(const Angle& other) const
    {
        return Angle(this->_rads / other._rads);
    }

    bool Angle::operator==(const Angle& other) const
    {
        return this->_rads == other._rads;
    }

    bool Angle::operator!=(const Angle& other) const
    {
        return not (*this == other);
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