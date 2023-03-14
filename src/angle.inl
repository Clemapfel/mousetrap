// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    inline Angle::Angle(float radians)
            : _rads(radians)
    {}

    inline Angle degrees(float dg)
    {
        return Angle(dg * (M_PI / 180.f));
    }

    inline Angle radians(float rad)
    {
        return Angle(rad);
    }

    inline float Angle::as_degrees() const
    {
        return _rads * (180.f / M_PI);
    }

    inline float Angle::as_radians() const
    {
        return _rads;
    }

    inline Angle Angle::operator+(const Angle& other)
    {
        return Angle(this->_rads + other._rads);
    }

    inline Angle Angle::operator-(const Angle& other)
    {
        return Angle(this->_rads - other._rads);
    }

    inline Angle Angle::operator*(const Angle& other)
    {
        return Angle(this->_rads * other._rads);
    }

    inline Angle Angle::operator/(const Angle& other)
    {
        return Angle(this->_rads / other._rads);
    }

    inline Angle& Angle::operator+=(const Angle& other)
    {
        this->_rads += other._rads;
        return *this;
    }

    inline Angle& Angle::operator-=(const Angle& other)
    {
        this->_rads -= other._rads;
        return *this;
    }

    inline Angle& Angle::operator*=(const Angle& other)
    {
        this->_rads *= other._rads;
        return *this;
    }

    inline Angle& Angle::operator/=(const Angle& other)
    {
        this->_rads /= other._rads;
        return *this;
    }
}