// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Overlay::Overlay()
    {
        _overlay = GTK_OVERLAY(gtk_overlay_new());
    }

    GtkWidget* Overlay::get_native()
    {
        return GTK_WIDGET(_overlay);
    }

    void Overlay::set_under(Widget* in)
    {
        gtk_overlay_set_child(_overlay, in->get_native());
    }

    void Overlay::set_over(Widget* in)
    {
        gtk_overlay_add_overlay(_overlay, in->get_native());
    }

    void Overlay::remove_over(Widget* in)
    {
        gtk_overlay_remove_overlay(_overlay, in->get_native());
    }

    void Overlay::remove_under()
    {
        gtk_overlay_set_child(_overlay, nullptr);
    }
}