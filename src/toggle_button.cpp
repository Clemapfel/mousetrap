//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/toggle_button.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    ToggleButton::ToggleButton()
        : WidgetImplementation<GtkToggleButton>(GTK_TOGGLE_BUTTON(gtk_toggle_button_new())),
          CTOR_SIGNAL(ToggleButton, toggled),
          CTOR_SIGNAL(ToggleButton, activate),
          CTOR_SIGNAL(ToggleButton, clicked)
    {}

    bool ToggleButton::get_active() const
    {
        return gtk_toggle_button_get_active(get_native()) == TRUE;
    }

    void ToggleButton::set_active(bool b)
    {
        gtk_toggle_button_set_active(get_native(), b ? TRUE : FALSE);
    }

    void ToggleButton::set_child(const Widget& widget)
    {
        _child = &widget;
        WARN_IF_SELF_INSERTION(ToggleButton::set_child, this, _child);
        gtk_button_set_child(GTK_BUTTON(get_native()), widget.operator GtkWidget*());
    }

    void ToggleButton::remove_child()
    {
        _child = nullptr;
        gtk_button_set_child(GTK_BUTTON(get_native()), nullptr);
    }

    Widget* ToggleButton::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    void ToggleButton::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(get_native()), b);
    }

    bool ToggleButton::get_has_frame() const {
        return gtk_button_get_has_frame(GTK_BUTTON(get_native()));
    }


    void ToggleButton::set_is_circular(bool b)
    {
        if (b and not get_is_circular())
            gtk_widget_add_css_class(GTK_WIDGET(get_native()), "circular");
        else if (not b and get_is_circular())
            gtk_widget_remove_css_class(GTK_WIDGET(get_native()), "circular");
    }

    bool ToggleButton::get_is_circular() const
    {
        return gtk_widget_has_css_class(GTK_WIDGET(get_native()), "circular");
    }
}