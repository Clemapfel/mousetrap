// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#include "include/check_button.hpp"

namespace mousetrap
{
    inline CheckButton::CheckButton()
        : WidgetImplementation<GtkCheckButton>(GTK_CHECK_BUTTON(gtk_check_button_new())), HasToggledSignal<CheckButton>(this)
    {}

    inline bool CheckButton::get_active()
    {
        return gtk_check_button_get_active(get_native());
    }

    inline void CheckButton::set_active(bool b)
    {
        gtk_check_button_set_active(get_native(), b);
    }

    inline bool CheckButton::get_is_inconsistent()
    {
        return gtk_check_button_get_inconsistent(get_native());
    }

    inline void CheckButton::set_is_inconsistent(bool b)
    {
        gtk_check_button_set_inconsistent(get_native(), b);
    }
}