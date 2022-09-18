// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Label::Label(const std::string& str)
        : WidgetImplementation<GtkLabel>(GTK_LABEL(gtk_label_new(str.c_str())))
    {}
    
    void Label::set_text(const std::string& text)
    {
        gtk_label_set_text(get_native(), text.c_str());
    }

    std::string Label::get_text()
    {
        return std::string(gtk_label_get_text(get_native()));
    }

    void Label::set_use_markup(bool b)
    {
        gtk_label_set_use_markup(get_native(), b);
    }
}