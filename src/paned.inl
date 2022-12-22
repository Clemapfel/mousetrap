// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Paned::Paned(GtkOrientation orientation)
        : WidgetImplementation<GtkPaned>(GTK_PANED(gtk_paned_new(orientation)))
    {
        set_start_child_resizable(true);
        set_end_child_resizable(true);
        set_has_wide_handle(true);
    }

    void Paned::set_position(int v)
    {
        gtk_paned_set_position(get_native(), v);
    }

    int Paned::get_position()
    {
        return gtk_paned_get_position(get_native());
    }

    void Paned::set_start_child(Widget* widget)
    {
        gtk_paned_set_start_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    void Paned::set_start_child_resizable(bool b)
    {
        gtk_paned_set_resize_start_child(get_native(), b);
    }

    void Paned::set_start_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_start_child(get_native(), b);
    }

    void Paned::set_end_child(Widget* widget)
    {
        gtk_paned_set_end_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    void Paned::set_end_child_resizable(bool b)
    {
        gtk_paned_set_resize_end_child(get_native(), b);
    }

    void Paned::set_end_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_end_child(get_native(), b);
    }

    void Paned::set_has_wide_handle(bool b)
    {
        gtk_paned_set_wide_handle(get_native(), b);
    }
}