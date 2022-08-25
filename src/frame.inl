// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    GtkWidget* Frame::get_native()
    {
        return GTK_WIDGET(_native);
    }

    Frame::Frame(float ratio, float x_align, float y_align, bool obey_child)
    {
        _native = GTK_ASPECT_FRAME(gtk_aspect_frame_new(x_align, y_align, ratio, obey_child));
    }

    void Frame::set_ratio(float new_ratio)
    {
        gtk_aspect_frame_set_ratio(_native, new_ratio);
    }

    void Frame::set_label(const std::string& text, float xalign, float yalign)
    {
        gtk_frame_set_label(GTK_FRAME(_native), text.c_str());
        gtk_frame_set_label_align(GTK_FRAME(_native), xalign, yalign);
    }

    void Frame::set_shadow_type(GtkShadowType type)
    {
        gtk_frame_set_shadow_type(GTK_FRAME(_native), type);
    }
}