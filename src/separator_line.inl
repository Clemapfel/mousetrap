// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    inline SeparatorLine::SeparatorLine(GtkOrientation orientation)
        : WidgetImplementation<GtkSeparator>(GTK_SEPARATOR(gtk_separator_new(orientation)))
    {
        if (orientation == GTK_ORIENTATION_HORIZONTAL)
            set_hexpand(true);
        else
            set_vexpand(true);
    }
}