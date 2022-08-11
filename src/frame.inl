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
            : _ratio(ratio), _x_align(x_align), _y_align(y_align), _obey_child(obey_child)
    {
        _native = GTK_ASPECT_FRAME(gtk_aspect_frame_new(nullptr, x_align, y_align, ratio, obey_child));
    }

    void Frame::set_ratio(float new_ratio)
    {
        gtk_aspect_frame_set(_native, new_ratio, _x_align, _y_align, _obey_child);
    }

    void Frame::set_xalign(float new_align)
    {
        gtk_aspect_frame_set(_native, _ratio, new_align, _y_align, _obey_child);
    }

    void Frame::set_yalign(float new_align)
    {
        gtk_aspect_frame_set(_native, _ratio, _x_align, new_align, _obey_child);
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