// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cmath>

namespace mousetrap
{
    class Angle;

    Angle degrees(float);
    Angle radians(float);

    struct Angle
    {
        friend Angle degrees(float);
        friend Angle radians(float);

        public:
            float as_degrees() const;
            float as_radians() const;

            Angle operator+(const Angle&);
            Angle operator-(const Angle&);

            Angle operator*(const Angle&);
            Angle operator/(const Angle&);
            Angle& operator+=(const Angle&);
            Angle& operator-=(const Angle&);
            Angle& operator*=(const Angle&);
            Angle& operator/=(const Angle&);

        private:
            explicit Angle(float radians);

            float _rads;
    };
}
