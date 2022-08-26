// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    SpinButton::SpinButton(float min, float max, float step)
    {
        _native = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(min, max, step));
    }

    SpinButton::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void SpinButton::set_value(float value)
    {
        gtk_spin_button_set_value(_native, value);
    }

    float SpinButton::get_value()
    {
        return gtk_spin_button_get_value(_native);
    }

    void SpinButton::set_digits(size_t n)
    {
        gtk_spin_button_set_digits(_native, n);
    }

    void SpinButton::set_width_chars(size_t n)
    {
        gtk_entry_set_width_chars(GTK_ENTRY(_native), n);
    }

    void SpinButton::set_wrap(bool b)
    {
        gtk_spin_button_set_wrap(_native, b);
    }
}