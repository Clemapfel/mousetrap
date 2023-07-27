//
// Created by clem on 7/27/23.
//

#include <mousetrap/clamp_frame.hpp>

namespace mousetrap
{
    ClampFrame::ClampFrame(Orientation orientation)
        : Widget(adw_clamp_new()),
          CTOR_SIGNAL(ClampFrame, realize),
          CTOR_SIGNAL(ClampFrame, unrealize),
          CTOR_SIGNAL(ClampFrame, destroy),
          CTOR_SIGNAL(ClampFrame, hide),
          CTOR_SIGNAL(ClampFrame, show),
          CTOR_SIGNAL(ClampFrame, map),
          CTOR_SIGNAL(ClampFrame, unmap)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(Widget::operator NativeWidget()), (GtkOrientation) orientation);
        _internal = ADW_CLAMP(g_object_ref(Widget::operator NativeWidget()));
    }

    ClampFrame::ClampFrame(detail::ClampFrameInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(ClampFrame, realize),
          CTOR_SIGNAL(ClampFrame, unrealize),
          CTOR_SIGNAL(ClampFrame, destroy),
          CTOR_SIGNAL(ClampFrame, hide),
          CTOR_SIGNAL(ClampFrame, show),
          CTOR_SIGNAL(ClampFrame, map),
          CTOR_SIGNAL(ClampFrame, unmap)
    {
        g_object_ref(_internal);
    }

    ClampFrame::~ClampFrame()
    {
        g_object_unref(_internal);
    }

    NativeObject ClampFrame::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void ClampFrame::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(Widget::operator NativeWidget()), (GtkOrientation) orientation);
    }

    Orientation ClampFrame::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(Widget::operator NativeWidget()));
    }

    void ClampFrame::set_maximum_size(float px)
    {
        adw_clamp_set_maximum_size(_internal, px);
        adw_clamp_set_tightening_threshold(_internal, px);
    }

    float ClampFrame::get_maximum_size() const
    {
        return adw_clamp_get_maximum_size(_internal);
    }

    void ClampFrame::set_child(const Widget& child)
    {
        const Widget* child_ptr = &child;
        WARN_IF_SELF_INSERTION(ClampFrame::set_child, this, child_ptr);
        WARN_IF_PARENT_EXISTS(ClampFrame::set_child, child);

        adw_clamp_set_child(_internal, child.operator NativeWidget());
    }

    void ClampFrame::remove_child()
    {
        adw_clamp_set_child(_internal, nullptr);
    }
}