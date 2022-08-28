// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    SeparatorLine::SeparatorLine(GtkOrientation orientation)
    {
        _native = GTK_SEPARATOR(gtk_separator_new(orientation));
    }

    SeparatorLine::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }
}