// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Scale::Scale(float min, float max, float step, GtkOrientation orientation)
    {
        _native = GTK_SCALE(gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step));
        gtk_scale_set_draw_value(_native, false);
    }

    GtkWidget* Scale::get_native()
    {
        return GTK_WIDGET(_native);
    }

    Scale::~Scale()
    {}

    void Scale::set_draw_value(bool b)
    {
        gtk_scale_set_draw_value(_native, b);
    }

    void Scale::add_mark(float value, GtkPositionType pos, const std::string& text)
    {
        gtk_scale_add_mark(_native, value, pos, (text.empty() ? nullptr : text.c_str()));
    }

    void Scale::clear_marks()
    {
        gtk_scale_clear_marks(_native);
    }

}