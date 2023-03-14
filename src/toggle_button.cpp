// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/toggle_button.hpp>
#include <include/action.hpp>

namespace mousetrap
{
    ToggleButton::ToggleButton()
        : WidgetImplementation<GtkToggleButton>(GTK_TOGGLE_BUTTON(gtk_toggle_button_new())), HasToggledSignal(this)
    {}

    bool ToggleButton::get_active() const
    {
        return gtk_toggle_button_get_active(get_native()) == TRUE;
    }

    void ToggleButton::set_active(bool b)
    {
        gtk_toggle_button_set_active(get_native(), b ? TRUE : FALSE);
    }

    void ToggleButton::set_action(Action& stateful_action)
    {
        gtk_actionable_set_action_name(GTK_ACTIONABLE(get_native()), ("app." + stateful_action.get_id()).c_str());
    }

    void ToggleButton::set_label(const std::string& str)
    {
        gtk_button_set_label(GTK_BUTTON(get_native()), str.c_str());
    }

    void ToggleButton::set_child(Widget* widget)
    {
        gtk_button_set_child(GTK_BUTTON(get_native()), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    void ToggleButton::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(get_native()), b);
    }
}