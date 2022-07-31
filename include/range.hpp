// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace rat
{
    // cannot be instanced
    class Range : public Widget
    {
        public:
            void set_value(float);
            float get_value();

        protected:
            Range();
    };
}

// ###

namespace rat
{
    float Range::get_value()
    {
        return gtk_range_get_value(GTK_RANGE(get_native()));
    }

    void Range::set_value(float value)
    {
        gtk_range_set_value(GTK_RANGE(get_native()), value);
    }

    Range::Range()
    {}
}