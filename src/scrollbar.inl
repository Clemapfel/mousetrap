// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    GtkWidget* Scrollbar::get_native()
    {
        return GTK_WIDGET(_native);
    }

    Scrollbar::Scrollbar(Adjustment adjustment, GtkOrientation orientation)
            : _adjustment(adjustment)
    {
        _native = GTK_SCROLLBAR(gtk_scrollbar_new(orientation, _adjustment.get_native()));
    }

    Scrollbar::Scrollbar(GtkOrientation orientation)
            : _native(GTK_SCROLLBAR(gtk_scrollbar_new(orientation, nullptr))), _adjustment(gtk_range_get_adjustment(GTK_RANGE(_native)))
    {}

    Adjustment& Scrollbar::get_adjustment()
    {
        return _adjustment;
    }
}