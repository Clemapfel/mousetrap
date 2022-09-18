// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#include <include/scrollbar.hpp>

namespace mousetrap
{
    Scrollbar::Scrollbar(Adjustment adjustment, GtkOrientation orientation)
        : WidgetImplementation<GtkScrollbar>((GTK_SCROLLBAR(gtk_scrollbar_new(orientation, adjustment.operator GtkAdjustment*()))))
    {}
}