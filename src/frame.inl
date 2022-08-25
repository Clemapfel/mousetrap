// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Frame::Frame()
    {
        _native = gtk_frame_new(nullptr);
    }

    void Frame::add(GtkWidget* child)
    {
        gtk_frame_set_child(GTK_FRAME(_native), child);
    }

    void Frame::remove(GtkWidget* child)
    {
        if (gtk_frame_get_child(GTK_FRAME(_native)) == child)
            gtk_frame_set_child(GTK_FRAME(_native), nullptr);
    }

    GtkWidget* Frame::get_native()
    {
        return GTK_WIDGET(_native);
    }

    void Frame::set_label(const std::string& label)
    {
        gtk_frame_set_label(GTK_FRAME(_native), label.c_str());
    }

    AspectFrame::AspectFrame(float ratio, float x_align, float y_align, bool obey_child)
    {
        _native = gtk_aspect_frame_new(x_align, y_align, ratio, obey_child);
    }

    void AspectFrame::set_ratio(float new_ratio)
    {
        gtk_aspect_frame_set_ratio(GTK_ASPECT_FRAME(_native), new_ratio);
    }

    void AspectFrame::set_child_xalign(float x)
    {
        gtk_aspect_frame_set_xalign(GTK_ASPECT_FRAME(_native), x);
    }

    void AspectFrame::set_child_yalign(float x)
    {
        gtk_aspect_frame_set_yalign(GTK_ASPECT_FRAME(_native), x);
    }
}