// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include "include/check_button.hpp"

namespace mousetrap
{
    CheckButton::CheckButton()
        : WidgetImplementation<GtkCheckButton>(GTK_CHECK_BUTTON(gtk_check_button_new()))
    {}

    bool CheckButton::get_is_checked()
    {
        return gtk_check_button_get_active(get_native());
    }

    void CheckButton::set_is_checked(bool b)
    {
        gtk_check_button_set_active(get_native(), b);
    }

    bool CheckButton::get_is_inconsistent()
    {
        return gtk_check_button_get_inconsistent(get_native());
    }

    void CheckButton::set_is_inconsistent(bool b)
    {
        gtk_check_button_set_inconsistent(get_native(), b);
    }

    template<typename Function_t, typename T>
    void CheckButton::connect_signal_toggled(Function_t function, T data)
    {
        auto temp =  std::function<on_toggled_function_t<T>>(function);
        _on_toggled_f = std::function<on_toggled_function_t<void*>>(*((std::function<on_toggled_function_t<void*>>*) &temp));
        _on_toggled_data = data;

        connect_signal("toggled", on_toggled_wrapper, this);
    }

    void CheckButton::on_toggled_wrapper(GtkCheckButton*, CheckButton* instance)
    {
        if (instance->_on_toggled_f == nullptr)
            return;

        (instance->_on_toggled_f)(instance, instance->_on_toggled_data);
    }
}