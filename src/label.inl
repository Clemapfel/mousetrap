// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Label::Label(const std::string& str)
    {
        _native = GTK_LABEL(gtk_label_new(str.c_str()));
    }

    GtkWidget* Label::get_native()
    {
        return GTK_WIDGET(_native);
    }

    Label::~Label()
    {}

    void Label::set_text(const std::string& text)
    {
        gtk_label_set_text(_native, text.c_str());
    }

    void Label::set_use_markup(bool b)
    {
        gtk_label_set_use_markup(_native, b);
    }
}