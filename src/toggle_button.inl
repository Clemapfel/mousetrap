// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    ToggleButton::ToggleButton()
        : WidgetImplementation<GtkToggleButton>(GTK_TOGGLE_BUTTON(gtk_toggle_button_new()))
    {}

    bool ToggleButton::get_active() const
    {
        return gtk_toggle_button_get_active(get_native()) == TRUE;
    }

    void ToggleButton::set_active(bool b)
    {
        gtk_toggle_button_set_active(get_native(), b ? TRUE : FALSE);
    }

    void ToggleButton::set_label(const std::string& str)
    {
        gtk_button_set_label(GTK_BUTTON(get_native()), str.c_str());
    }

    void ToggleButton::set_child(Widget* widget)
    {
        gtk_button_set_child(GTK_BUTTON(get_native()), widget->operator GtkWidget*());
    }

    void ToggleButton::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(GTK_BUTTON(get_native()), b);
    }

    template<typename Function_t, typename T>
    void ToggleButton::connect_signal_toggled(Function_t function, T data)
    {
        auto temp =  std::function<on_toggled_function_t<T>>(function);
        _on_toggled_f = std::function<on_toggled_function_t<void*>>(*((std::function<on_toggled_function_t<void*>>*) &temp));
        _on_toggled_data = data;

        connect_signal("toggled", on_toggled_wrapper, this);
    }

    void ToggleButton::on_toggled_wrapper(GtkToggleButton*, ToggleButton* instance)
    {
        if (instance->_on_toggled_f == nullptr)
            return;

        (instance->_on_toggled_f)(instance, instance->_on_toggled_data);
    }
}