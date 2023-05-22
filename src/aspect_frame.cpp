//
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/aspect_frame.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    AspectFrame::AspectFrame(float ratio, float x_align, float y_align)
        : Widget(gtk_aspect_frame_new(x_align, y_align, ratio, false))
    {
        _internal = GTK_ASPECT_FRAME(operator NativeWidget());

        if (ratio <= 0)
            log::critical("In AspectFrame::AspectFrame: Ratio " + std::to_string(ratio) + " cannot not be 0 or negative", MOUSETRAP_DOMAIN);

        if (x_align < 0 or x_align > 1)
            log::warning("In AspectFrame::AspectFrame: Specified child x-alignment " + std::to_string(x_align) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);

        if (y_align < 0 or y_align > 1)
            log::warning("In AspectFrame::AspectFrame: Specified child y-alignment " + std::to_string(x_align) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);
    }
    
    AspectFrame::AspectFrame(detail::AspectFrameInternal* internal)
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }
    
    AspectFrame::~AspectFrame() 
    {
        g_object_unref(_internal);
    }

    NativeObject AspectFrame::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void AspectFrame::set_child(const Widget& child)
    {
        const Widget* child_ptr = &child;
        WARN_IF_SELF_INSERTION(AspectFrame::set_child, this, child_ptr);

        gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(operator NativeWidget()), child.operator NativeWidget());
    }

    void AspectFrame::remove_child()
    {
        gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(operator NativeWidget()), nullptr);
    }

    void AspectFrame::set_ratio(float new_ratio)
    {
        if (new_ratio <= 0)
            log::critical("In AspectFrame::set_ratio: Ratio " + std::to_string(new_ratio) + " cannot not be 0 or negative", MOUSETRAP_DOMAIN);

        gtk_aspect_frame_set_ratio(GTK_ASPECT_FRAME(operator NativeWidget()), new_ratio);
    }

    void AspectFrame::set_child_x_alignment(float x)
    {
        if (x < 0 or x > 1)
            log::warning("In AspectFrame::set_child_x_alignment: Specified child x-alignment " + std::to_string(x) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);

        gtk_aspect_frame_set_xalign(GTK_ASPECT_FRAME(operator NativeWidget()), x);
    }

    void AspectFrame::set_child_y_alignment(float x)
    {
        if (x < 0 or x > 1)
            log::warning("In AspectFrame::set_child_x_alignment: Specified child y-alignment " + std::to_string(x) + " is outside [0, 1] ", MOUSETRAP_DOMAIN);

        gtk_aspect_frame_set_yalign(GTK_ASPECT_FRAME(operator NativeWidget()), x);
    }

    float AspectFrame::get_ratio() const
    {
        return gtk_aspect_frame_get_ratio(GTK_ASPECT_FRAME(operator NativeWidget()));
    }

    float AspectFrame::get_child_x_alignment() const
    {
        return gtk_aspect_frame_get_xalign(GTK_ASPECT_FRAME(operator NativeWidget()));
    }

    float AspectFrame::get_child_y_alignment() const
    {
        return gtk_aspect_frame_get_yalign(GTK_ASPECT_FRAME(operator NativeWidget()));
    }
}