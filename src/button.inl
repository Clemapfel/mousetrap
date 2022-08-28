// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Button::Button()
    {
        _native = GTK_BUTTON(gtk_button_new());
    }

    Button::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Button::set_label(const std::string& text)
    {
        gtk_button_set_label(_native, text.c_str());
    }

    void Button::set_icon(const std::string& path)
    {
        _icon = GTK_IMAGE(gtk_image_new_from_file(path.c_str()));
        gtk_button_set_child(_native, GTK_WIDGET(_icon));
    }

    void Button::set_has_frame(bool b)
    {
        gtk_button_set_has_frame(_native, b);
    }
}