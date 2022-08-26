// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Range : public Widget
    {
        public:
            void set_value(float);
            float get_value();

        protected:
            Range();
    };
}

#include <src/range.inl>