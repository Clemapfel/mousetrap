// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    ToggleButton::ToggleButton()
    {
        _native = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
    }

    bool ToggleButton::get_active() const
    {
        return gtk_toggle_button_get_active(_native) == TRUE;
    }

    void ToggleButton::set_active(bool b)
    {
        gtk_toggle_button_set_active(_native, b ? TRUE : FALSE);
    }

    void ToggleButton::set_label(const std::string& str)
    {
        gtk_button_set_label(GTK_BUTTON(_native), str.c_str());
    }

    ToggleButton::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void ToggleButton::set_child(Widget* widget)
    {
        gtk_button_set_child(GTK_BUTTON(_native), widget->operator GtkWidget*());
    }

    void ToggleButton::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(_native), b);
    }
}