// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/separator.hpp>

namespace mousetrap
{
    Separator::Separator(float opacity, Orientation orientation)
        : Widget(gtk_separator_new((GtkOrientation) orientation)),
          CTOR_SIGNAL(Separator, realize),
          CTOR_SIGNAL(Separator, unrealize),
          CTOR_SIGNAL(Separator, destroy),
          CTOR_SIGNAL(Separator, hide),
          CTOR_SIGNAL(Separator, show),
          CTOR_SIGNAL(Separator, map),
          CTOR_SIGNAL(Separator, unmap)
    {
        _internal = g_object_ref_sink(GTK_SEPARATOR(Widget::operator NativeWidget()));

        if (orientation == Orientation::HORIZONTAL)
            set_expand_horizontally(true);
        else
            set_expand_vertically(true);

        set_opacity(opacity);
    }
    
    Separator::Separator(detail::SeparatorInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Separator, realize),
          CTOR_SIGNAL(Separator, unrealize),
          CTOR_SIGNAL(Separator, destroy),
          CTOR_SIGNAL(Separator, hide),
          CTOR_SIGNAL(Separator, show),
          CTOR_SIGNAL(Separator, map),
          CTOR_SIGNAL(Separator, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    Separator::~Separator() 
    {
        g_object_unref(_internal);
    }

    NativeObject Separator::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Separator::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_internal), (GtkOrientation) orientation);
    }

    Orientation Separator::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(_internal));
    }
}