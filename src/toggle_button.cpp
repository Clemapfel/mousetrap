//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/toggle_button.hpp>
#include <include/action.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    ToggleButton::ToggleButton()
        : WidgetImplementation<GtkToggleButton>(GTK_TOGGLE_BUTTON(gtk_toggle_button_new())),
          CTOR_SIGNAL(ToggleButton, toggled),
          CTOR_SIGNAL(ToggleButton, activate),
          CTOR_SIGNAL(ToggleButton, clicked)
    {}

    ToggleButton::ToggleButton(Widget* child)
        : ToggleButton()
    {
        set_child(child);
    }

    bool ToggleButton::get_active() const
    {
        return gtk_toggle_button_get_active(get_native()) == TRUE;
    }

    void ToggleButton::set_active(bool b)
    {
        gtk_toggle_button_set_active(get_native(), b ? TRUE : FALSE);
    }

    void ToggleButton::set_child(Widget* widget)
    {
        WARN_IF_SELF_INSERTION(ToggleButton::set_child, this, widget);

        _child = widget;
        gtk_button_set_child(GTK_BUTTON(get_native()), _child == nullptr ? nullptr : _child->operator GtkWidget*());
    }

    Widget* ToggleButton::get_child() const
    {
        return _child;
    }

    void ToggleButton::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(get_native()), b);
    }

    bool ToggleButton::get_has_frame() const {
        return gtk_button_get_has_frame(GTK_BUTTON(get_native()));
    }
}