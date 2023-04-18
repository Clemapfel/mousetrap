//
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <include/aspect_frame.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    AspectFrame::AspectFrame(float ratio, float x_align, float y_align)
        : WidgetImplementation<GtkAspectFrame>(GTK_ASPECT_FRAME(gtk_aspect_frame_new(x_align, y_align, ratio, false)))
    {
        if (ratio <= 0)
            log::critical("In AspectFrame::AspectFrame: Ratio " + std::to_string(ratio) + " cannot not be 0 or negative", MOUSETRAP_DOMAIN);

        if (x_align < 0 or x_align > 1)
            log::warning("In AspectFrame::AspectFrame: Specified child x-alignment " + std::to_string(x_align) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);

        if (y_align < 0 or y_align > 1)
            log::warning("In AspectFrame::AspectFrame: Specified child y-alignment " + std::to_string(x_align) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);
    }

    void AspectFrame::set_child(Widget* child)
    {
        WARN_IF_SELF_INSERTION(AspectFrame::set_child, this, child);

        _child = child;
        gtk_aspect_frame_set_child(get_native(), child == nullptr ? nullptr : child->operator GtkWidget*());
    }

    Widget* AspectFrame::get_child() const
    {
        return _child;
    }

    void AspectFrame::remove_child()
    {
        gtk_aspect_frame_set_child(get_native(), nullptr);
    }

    void AspectFrame::set_ratio(float new_ratio)
    {
        if (new_ratio <= 0)
            log::critical("In AspectFrame::set_ratio: Ratio " + std::to_string(new_ratio) + " cannot not be 0 or negative", MOUSETRAP_DOMAIN);

        gtk_aspect_frame_set_ratio(get_native(), new_ratio);
    }

    void AspectFrame::set_child_x_alignment(float x)
    {
        if (x < 0 or x > 1)
            log::warning("In AspectFrame::set_child_x_alignment: Specified child x-alignment " + std::to_string(x) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);

        gtk_aspect_frame_set_xalign(get_native(), x);
    }

    void AspectFrame::set_child_y_alignment(float x)
    {
        if (x < 0 or x > 1)
            log::warning("In AspectFrame::set_child_x_alignment: Specified child y-alignment " + std::to_string(x) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);

        gtk_aspect_frame_set_yalign(get_native(), x);
    }

    float AspectFrame::get_ratio() const
    {
        return gtk_aspect_frame_get_ratio(get_native());
    }

    float AspectFrame::get_child_x_alignment() const
    {
        return gtk_aspect_frame_get_xalign(get_native());
    }

    float AspectFrame::get_child_y_alignment() const
    {
        return gtk_aspect_frame_get_yalign(get_native());
    }
}