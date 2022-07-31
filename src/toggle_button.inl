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
        connect_signal("toggled", on_toggled_wrapper);
    }

    ToggleButton::~ToggleButton()
    {
        gtk_widget_destroy(GTK_WIDGET(_native));
    }

    bool ToggleButton::get_active() const
    {
        return gtk_toggle_button_get_active(_native) == TRUE;
    }

    void ToggleButton::set_active(bool b)
    {
        gtk_toggle_button_set_active(_native, b ? TRUE : FALSE);
    }

    GtkWidget* ToggleButton::get_native()
    {
        return GTK_WIDGET(_native);
    }

    void ToggleButton::on_toggled_wrapper(GtkToggleButton* self, void* instance)
    {
        ((ToggleButton*) instance)->on_toggled(self, nullptr);
    }

    void ToggleButton::on_toggled(GtkToggleButton* self, gpointer userdata)
    {
        // noop
    }
}