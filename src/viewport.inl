// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    inline Viewport::Viewport(Adjustment& h_adjustment, Adjustment& v_adjustment)
            : WidgetImplementation<GtkViewport>(GTK_VIEWPORT(gtk_viewport_new(
            h_adjustment.operator GtkAdjustment*(),
            v_adjustment.operator GtkAdjustment*()))
    )
    {}

    inline Viewport::Viewport()
            : WidgetImplementation<GtkViewport>(GTK_VIEWPORT(gtk_viewport_new(
            gtk_adjustment_new(0, 0, 0, 0, 0, 0),
            gtk_adjustment_new(0, 0, 0, 0, 0, 0)
    )))
    {}

    inline void Viewport::set_child(Widget* widget)
    {
        gtk_viewport_set_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    inline void Viewport::set_scroll_to_focus(bool b)
    {
        gtk_viewport_set_scroll_to_focus(get_native(), b);
    }
}