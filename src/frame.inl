// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <include/frame.hpp>

namespace mousetrap
{
    inline Frame::Frame()
        : WidgetImplementation<GtkFrame>(GTK_FRAME(gtk_frame_new("")))
    {
        set_label_widget(nullptr);
    }

    inline void Frame::set_child(Widget* in)
    {
        gtk_frame_set_child(get_native(), in == nullptr ? nullptr : in->operator GtkWidget *());
    }

    inline void Frame::remove_child()
    {
        gtk_frame_set_child(get_native(), nullptr);
    }

    inline void Frame::set_label_widget(Widget* widget)
    {
        gtk_frame_set_label_widget(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    inline void Frame::set_label_align(float x)
    {
        gtk_frame_set_label_align(get_native(), x);
    }
}