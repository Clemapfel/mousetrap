//
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <include/paned.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Paned::Paned(Orientation orientation)
        : WidgetImplementation<GtkPaned>(GTK_PANED(gtk_paned_new((GtkOrientation) orientation)))
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

    void Paned::set_start_child(const Widget& widget)
    {
        _start_child = &widget;
        WARN_IF_SELF_INSERTION(Paned::set_start_child, this, _start_child);
        gtk_paned_set_start_child(get_native(), _start_child == nullptr ? nullptr : _start_child->operator GtkWidget*());
    }

    Widget* Paned::get_start_child() const
    {
        return const_cast<Widget*>(_start_child);
    }

    void Paned::set_start_child_resizable(bool b)
    {
        gtk_paned_set_resize_start_child(get_native(), b);
    }

    bool Paned::get_start_child_resizable() const
    {
        return gtk_paned_get_resize_start_child(get_native());
    }

    void Paned::set_start_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_start_child(get_native(), b);
    }

    bool Paned::get_start_child_shrinkable() const
    {
        return gtk_paned_get_shrink_start_child(get_native());
    }

    void Paned::set_end_child(const Widget& widget)
    {
        _end_child = &widget;
        WARN_IF_SELF_INSERTION(Paned::set_end_child, this, _end_child);
        gtk_paned_set_end_child(get_native(), _end_child == nullptr ? nullptr : _end_child->operator GtkWidget*());
    }

    Widget* Paned::get_end_child() const
    {
        return const_cast<Widget*>(_end_child);
    }

    void Paned::set_end_child_resizable(bool b)
    {
        gtk_paned_set_resize_end_child(get_native(), b);
    }

    bool Paned::get_end_child_resizable() const
    {
        return gtk_paned_get_resize_end_child(get_native());
    }

    void Paned::set_end_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_end_child(get_native(), b);
    }

    bool Paned::get_end_child_shrinkable() const
    {
        return gtk_paned_get_shrink_end_child(get_native());
    }

    void Paned::set_has_wide_handle(bool b)
    {
        gtk_paned_set_wide_handle(get_native(), b);
    }

    bool Paned::get_has_widget_handle() const
    {
        return gtk_paned_get_wide_handle(get_native());
    }

    void Paned::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation Paned::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}