// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Overlay::Overlay()
    {
        _native = GTK_OVERLAY(gtk_overlay_new());
    }

    Overlay::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Overlay::set_child(Widget* in)
    {
        gtk_overlay_set_child(_native, in->operator GtkWidget*());
    }

    void Overlay::add_overlay(Widget* in)
    {
        gtk_overlay_add_overlay(_native, in->operator GtkWidget*());
    }

    void Overlay::remove_overlay(Widget* in)
    {
        gtk_overlay_remove_overlay(_native, in->operator GtkWidget*());
    }

    void Overlay::remove_child()
    {
        gtk_overlay_set_child(_native, nullptr);
    }
}