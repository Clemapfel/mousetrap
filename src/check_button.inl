// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

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
}