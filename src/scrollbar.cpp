//
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/scrollbar.hpp>

namespace mousetrap
{
    Scrollbar::Scrollbar(Orientation orientation)
        : Widget(gtk_scrollbar_new((GtkOrientation) orientation, nullptr))
    {}

    Scrollbar::Scrollbar(detail::ScrollbarInternal* internal)
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }

    Scrollbar::~Scrollbar()
    {
        g_object_unref(_internal);
    }

    NativeObject Scrollbar::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    Adjustment Scrollbar::get_adjustment() const
    {
        return Adjustment(gtk_scrollbar_get_adjustment(_internal));
    }

    void Scrollbar::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_internal), (GtkOrientation) orientation);
    }

    Orientation Scrollbar::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(_internal));
    }
}