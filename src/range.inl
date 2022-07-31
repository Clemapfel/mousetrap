// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
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