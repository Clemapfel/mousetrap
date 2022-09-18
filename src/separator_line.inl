// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    SeparatorLine::SeparatorLine(GtkOrientation orientation)
        : WidgetImplementation<GtkSeparator>(GTK_SEPARATOR(gtk_separator_new(orientation)))
    {}
}