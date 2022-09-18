// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <include/frame.hpp>

namespace mousetrap
{
    Frame::Frame()
        : WidgetImplementation<GtkFrame>(GTK_FRAME(gtk_frame_new("")))
    {}

    void Frame::set_child(Widget* in)
    {
        gtk_frame_set_child(get_native(), in->operator GtkWidget *());
    }

    void Frame::remove_child()
    {
        gtk_frame_set_child(get_native(), nullptr);
    }

    void Frame::set_label(const std::string& label)
    {
        gtk_frame_set_label(get_native(), label.c_str());
    }
}