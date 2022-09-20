// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    SpinButton::SpinButton(float min, float max, float step)
        : WidgetImplementation<GtkSpinButton>(GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(min, max, step))),
          HasValueChangedSignal<SpinButton>(this)
    {}

    void SpinButton::set_value(float value)
    {
        gtk_spin_button_set_value(get_native(), value);
    }

    float SpinButton::get_value()
    {
        return gtk_spin_button_get_value(get_native());
    }

    void SpinButton::set_digits(size_t n)
    {
        gtk_spin_button_set_digits(get_native(), n);
    }

    void SpinButton::set_wrap(bool b)
    {
        gtk_spin_button_set_wrap(get_native(), b);
    }
}