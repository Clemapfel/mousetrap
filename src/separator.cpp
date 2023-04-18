// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

#include <include/separator.hpp>

namespace mousetrap
{
    Separator::Separator(Orientation orientation)
        : WidgetImplementation<GtkSeparator>(GTK_SEPARATOR(gtk_separator_new((GtkOrientation) orientation)))
    {
        if (orientation == Orientation::HORIZONTAL)
            set_expand_horizontally(true);
        else
            set_expand_vertically(true);
    }

    void Separator::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation Separator::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}