// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <include/frame.hpp>

namespace mousetrap
{
    Frame::Frame()
    {
        _native = GTK_FRAME(gtk_frame_new(""));
    }

    GtkWidget* Frame::get_native()
    {
        return GTK_WIDGET(_native);
    }

    void Frame::add(GtkWidget* in)
    {
        gtk_frame_set_child(_native, in);
    }

    void Frame::remove(GtkWidget*)
    {
        gtk_frame_set_child(_native, nullptr);
    }

    void Frame::set_label(const std::string& label)
    {
        gtk_frame_set_label(_native, label.c_str());
    }
}