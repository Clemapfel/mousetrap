// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include "include/button.hpp"

namespace mousetrap
{
    Button::Button()
            : WidgetImplementation<GtkButton>(GTK_BUTTON(gtk_button_new())),
              HasClickedSignal<Button>(this)
    {}

    void Button::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(get_native(), b);
    }

    void Button::set_child(Widget* widget)
    {
        gtk_button_set_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }
}