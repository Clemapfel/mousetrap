// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Scale::Scale(float min, float max, float step, GtkOrientation orientation)
        : WidgetImplementation<GtkScale>(GTK_SCALE(gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step)))
    {
        gtk_scale_set_draw_value(get_native(), false);
    }

    void Scale::set_value(float v)
    {
        gtk_range_set_value(GTK_RANGE(get_native()), v);
    }

    float Scale::get_value()
    {
        return gtk_range_get_value(GTK_RANGE(get_native()));
    }

    void Scale::set_draw_value(bool b)
    {
        gtk_scale_set_draw_value(get_native(), b);
    }

    void Scale::add_mark(float value, GtkPositionType pos, const std::string& text)
    {
        gtk_scale_add_mark(get_native(), value, pos, (text.empty() ? nullptr : text.c_str()));
    }

    void Scale::clear_marks()
    {
        gtk_scale_clear_marks(get_native());
    }

}