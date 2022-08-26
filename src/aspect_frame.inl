// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void AspectFrame::set_child(Widget* child)
    {
        gtk_aspect_frame_set_child(_native, child->operator GtkWidget*());
    }

    void AspectFrame::remove_child()
    {
        gtk_aspect_frame_set_child(_native, nullptr);
    }

    AspectFrame::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    AspectFrame::AspectFrame(float ratio, float x_align, float y_align, bool obey_child)
    {
        _native = GTK_ASPECT_FRAME(gtk_aspect_frame_new(x_align, y_align, ratio, obey_child));
    }

    void AspectFrame::set_ratio(float new_ratio)
    {
        gtk_aspect_frame_set_ratio(_native, new_ratio);
    }

    void AspectFrame::set_child_xalign(float x)
    {
        gtk_aspect_frame_set_xalign(_native, x);
    }

    void AspectFrame::set_child_yalign(float x)
    {
        gtk_aspect_frame_set_yalign(_native, x);
    }
}