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
        gtk_widget_destroy(GTK_WIDGET(_overlay));
    }

    template<typename T>
    void Overlay::set_under(T in)
    {
        gtk_container_add(GTK_CONTAINER(_overlay), (GtkWidget*) in);
    }

    template<typename T>
    void Overlay::set_over(T in)
    {
        gtk_overlay_add_overlay(_overlay, (GtkWidget*) in);
    }
}