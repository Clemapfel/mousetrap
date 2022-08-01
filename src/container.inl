// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void Container::add(GtkWidget* in)
    {
        gtk_container_add(GTK_CONTAINER(get_native()), GTK_WIDGET(in));
    }

    void Container::add(Widget* in)
    {
        gtk_container_add(GTK_CONTAINER(get_native()), GTK_WIDGET(in->get_native()));
    }
}