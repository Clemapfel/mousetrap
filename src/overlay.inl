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

    Overlay::~Overlay()
    {
        //gtk_widget_destroy(GTK_WIDGET(_overlay));
    }

    GtkWidget* Overlay::get_native()
    {
        return GTK_WIDGET(_overlay);
    }

    void Overlay::set_under(GtkWidget* in)
    {
        gtk_container_add(GTK_CONTAINER(_overlay), GTK_WIDGET(in));
    }

    void Overlay::set_over(GtkWidget* in)
    {
        gtk_overlay_add_overlay(_overlay, GTK_WIDGET(in));
    }

    void Overlay::set_under(Widget* in)
    {
        gtk_container_add(GTK_CONTAINER(_overlay), GTK_WIDGET(in->get_native()));
    }

    void Overlay::set_over(Widget* in)
    {
        gtk_overlay_add_overlay(_overlay, GTK_WIDGET(in->get_native()));
    }

    void Overlay::set_pass_through(Widget* widget, bool b)
    {
        gtk_overlay_set_overlay_pass_through(_overlay, widget->get_native(), b);
    }
}