//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <include/center_box.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    CenterBox::CenterBox(Orientation orientation)
        : WidgetImplementation<GtkCenterBox>(GTK_CENTER_BOX(gtk_center_box_new()))
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    void CenterBox::set_start_widget(Widget* widget)
    {
        WARN_IF_SELF_INSERTION(CenterBox::set_start_widget, this, widget);

        _start = widget;
        gtk_center_box_set_start_widget(get_native(), _start != nullptr ? _start->operator GtkWidget*() : nullptr);
    }

    void CenterBox::set_end_widget(Widget* widget)
    {
        WARN_IF_SELF_INSERTION(CenterBox::set_end_widget, this, widget);

        _end = widget;
        gtk_center_box_set_end_widget(get_native(), _end != nullptr ? _end->operator GtkWidget*() : nullptr);
    }

    void CenterBox::set_center_widget(Widget* widget)
    {
        WARN_IF_SELF_INSERTION(CenterBox::set_center_widget, this, widget);

        _center = widget;
        gtk_center_box_set_center_widget(get_native(), _center != nullptr ? _center->operator GtkWidget*() : nullptr);
    }

    Widget* CenterBox::get_start_widget() const
    {
        return _start;
    }

    Widget* CenterBox::get_center_widget() const
    {
        return _center;
    }

    Widget* CenterBox::get_end_widget() const
    {
        return _end;
    }

    void CenterBox::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation CenterBox::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}