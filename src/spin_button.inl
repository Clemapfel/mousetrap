// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    SpinButton::SpinButton(float min, float max, float step)
    {
        _native = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(min, max, step));

        connect_signal("input", on_input_wrapper, this);
        connect_signal("output", on_output_wrapper, this);
        connect_signal("value-changed", on_value_changed_wrapper, this);
    }

    SpinButton::~SpinButton()
    {
        //gtk_widget_destroy(GTK_WIDGET(_native));
    }

    GtkWidget* SpinButton::get_native()
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

    void SpinButton::set_wrap(bool b)
    {
        gtk_spin_button_set_wrap(_native, b);
    }

    gint SpinButton::on_input_wrapper(GtkSpinButton* self, gdouble* new_value, void* instance)
    {
        return ((SpinButton*) instance)->on_input(self, new_value, nullptr);
    }

    gint SpinButton::on_input(GtkSpinButton* self, gdouble* new_value, gpointer user_data)
    {
        try
        {
            *new_value = std::stof(gtk_entry_get_text(GTK_ENTRY(self)));
        }
        catch (...)
        {
            return GTK_INPUT_ERROR;
        }

        return TRUE;
    }

    gboolean SpinButton::on_output(GtkSpinButton* self, gpointer user_data)
    {
        return FALSE;
    }

    gboolean SpinButton::on_output_wrapper(GtkSpinButton* self, void* instance)
    {
        return ((SpinButton*) instance)->on_output(self, nullptr);
    }

    void SpinButton::on_value_changed(GtkSpinButton* self, gpointer user_data)
    {}

    void SpinButton::on_value_changed_wrapper(GtkSpinButton* self, void* instance)
    {
        ((SpinButton*) instance)->on_value_changed(self, nullptr);
    }
}