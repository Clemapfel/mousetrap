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

    void Container::remove(GtkWidget* out)
    {
        gtk_container_remove(GTK_CONTAINER(get_native()), GTK_WIDGET(out));
    }

    void Container::remove(Widget* out)
    {
        gtk_container_remove(GTK_CONTAINER(get_native()), GTK_WIDGET(out->get_native()));
    }

    void Container::clear()
    {
        auto* glist = gtk_container_get_children(GTK_CONTAINER(get_native()));
        for (auto it = glist; it != nullptr; it = g_list_next(it))
            gtk_widget_destroy(GTK_WIDGET(it->data));
            //gtk_container_remove(GTK_CONTAINER(get_native()), GTK_WIDGET(it->data));

        g_list_free(glist);
    }
}