// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    void AspectFrame::set_child(Widget* child)
    {
        gtk_aspect_frame_set_child(get_native(), child->operator GtkWidget*());
    }

    void AspectFrame::remove_child()
    {
        gtk_aspect_frame_set_child(get_native(), nullptr);
    }

    AspectFrame::AspectFrame(float ratio, float x_align, float y_align, bool obey_child)
        : WidgetImplementation<GtkAspectFrame>(GTK_ASPECT_FRAME(gtk_aspect_frame_new(x_align, y_align, ratio, obey_child)))
    {}

    void AspectFrame::set_ratio(float new_ratio)
    {
        gtk_aspect_frame_set_ratio(get_native(), new_ratio);
    }

    void AspectFrame::set_child_xalign(float x)
    {
        gtk_aspect_frame_set_xalign(get_native(), x);
    }

    void AspectFrame::set_child_yalign(float x)
    {
        gtk_aspect_frame_set_yalign(get_native(), x);
    }
}