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
    {
        //gtk_widget_destroy(get_native());
    }
}