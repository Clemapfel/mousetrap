// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    CheckButton::CheckButton()
    {
        _native = GTK_CHECK_BUTTON(gtk_check_button_new());
    }

    CheckButton::CheckButton(const std::string& label)
    {
        _native = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(label.c_str()));
    }

    CheckButton::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    bool CheckButton::get_is_checked()
    {
        return gtk_check_button_get_active(_native);
    }

    void CheckButton::set_is_checked(bool b)
    {
        gtk_check_button_set_active(_native, b);
    }

    bool CheckButton::get_is_inconsistent()
    {
        return gtk_check_button_get_inconsistent(_native);
    }

    void CheckButton::set_is_inconsistent(bool b)
    {
        gtk_check_button_set_inconsistent(_native, b);
    }
}