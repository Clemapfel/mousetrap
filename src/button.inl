// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include "include/button.hpp"

namespace mousetrap
{
    inline Button::Button()
            : WidgetImplementation<GtkButton>(GTK_BUTTON(gtk_button_new())),
              HasClickedSignal<Button>(this)
    {}

    inline void Button::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(get_native(), b);
    }

    inline void Button::set_child(Widget* widget)
    {
        gtk_button_set_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    inline void Button::set_action(Action& action)
    {
        gtk_actionable_set_action_name(GTK_ACTIONABLE(get_native()), ("app."+ action.get_id()).c_str());
    }
}