// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include "include/button.hpp"

namespace mousetrap
{
    Button::Button()
        : WidgetImplementation<GtkButton>(GTK_BUTTON(gtk_button_new()))
    {}

    void Button::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(get_native(), b);
    }

    void Button::set_child(Widget* widget)
    {
        gtk_button_set_child(get_native(), widget->operator GtkWidget*());
    }

    template<typename Function_t, typename T>
    void Button::connect_signal_clicked(Function_t function, T data)
    {
        auto temp =  std::function<on_clicked_function_t<T>>(function);
        _on_clicked_f = std::function<on_clicked_function_t<void*>>(*((std::function<on_clicked_function_t<void*>>*) &temp));
        _on_clicked_data = data;

        connect_signal("clicked", on_clicked_wrapper, this);
    }

    void Button::on_clicked_wrapper(GtkButton*, Button* instance)
    {
        if (instance->_on_clicked_f == nullptr)
            return;

        (instance->_on_clicked_f)(instance, instance->_on_clicked_data);
    }
}