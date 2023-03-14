// 
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#include <include/popover.hpp>

namespace mousetrap
{
    Popover::Popover()
        : WidgetImplementation<GtkPopover>(GTK_POPOVER(gtk_popover_new()))
    {}

    void Popover::popup()
    {
        gtk_popover_popup(get_native());
    }

    void Popover::popdown()
    {
        gtk_popover_popdown(get_native());
    }

    void Popover::present()
    {
        gtk_popover_present(get_native());
    }

    void Popover::set_child(Widget* widget)
    {
        gtk_popover_set_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    void Popover::set_relative_position(GtkPositionType position)
    {
        gtk_popover_set_position(get_native(), position);
    }

    void Popover::set_has_arrow(bool b)
    {
        gtk_popover_set_has_arrow(get_native(), b);
    }

    void Popover::set_autohide(bool b)
    {
        gtk_popover_set_autohide(get_native(), b);
    }

    void Popover::attach_to(Widget* widget)
    {
        gtk_widget_set_parent(GTK_WIDGET(get_native()), widget->operator GtkWidget*());
    }
}