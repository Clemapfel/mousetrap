//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/center_box.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    CenterBox::CenterBox(Orientation orientation)
        : WidgetImplementation<GtkCenterBox>(GTK_CENTER_BOX(gtk_center_box_new()))
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    void CenterBox::set_start_child(const Widget& widget)
    {
        _start = &widget;
        WARN_IF_SELF_INSERTION(CenterBox::set_start_widget, this, _start);

        _start = &widget;
        gtk_center_box_set_start_widget(get_native(), widget.operator NativeWidget());
    }

    void CenterBox::remove_start_widget()
    {
        _start = nullptr;
        gtk_center_box_set_start_widget(get_native(), nullptr);
    }

    void CenterBox::set_end_child(const Widget& widget)
    {
        _end = &widget;
        WARN_IF_SELF_INSERTION(CenterBox::set_end_widget, this, _end);

        gtk_center_box_set_end_widget(get_native(), widget.operator NativeWidget());
    }

    void CenterBox::remove_end_widget()
    {
        _end = nullptr;
        gtk_center_box_set_end_widget(get_native(), nullptr);
    }

    void CenterBox::set_center_child(const Widget& widget)
    {
        _center = &widget;
        WARN_IF_SELF_INSERTION(CenterBox::set_center_widget, this, _center);

        gtk_center_box_set_center_widget(get_native(), widget.operator NativeWidget());
    }

    void CenterBox::remove_center_widget()
    {
        _center = nullptr;
        gtk_center_box_set_center_widget(get_native(), nullptr);
    }

    Widget* CenterBox::get_start_child() const
    {
        return const_cast<Widget*>(_start);
    }

    Widget* CenterBox::get_center_child() const
    {
        return const_cast<Widget*>(_center);
    }

    Widget* CenterBox::get_end_child() const
    {
        return const_cast<Widget*>(_end);
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