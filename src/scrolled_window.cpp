//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/scrolled_window.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    ScrolledWindow::ScrolledWindow()
        : Widget(gtk_scrolled_window_new()),
          CTOR_SIGNAL(ScrolledWindow, scroll_child)
    {
        _internal = g_object_ref_sink(GTK_SCROLLED_WINDOW(Widget::operator NativeWidget()));
    }
    
    ScrolledWindow::ScrolledWindow(detail::ScrolledWindowInternal* internal)
        : Widget(GTK_WIDGET(internal)), 
          CTOR_SIGNAL(ScrolledWindow, scroll_child)
    {
        _internal = g_object_ref(internal);
    }

    ScrolledWindow::~ScrolledWindow()
    {
        g_object_unref(_internal);
    }

    NativeObject ScrolledWindow::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void ScrolledWindow::set_propagate_natural_height(bool b)
    {
        gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(_internal), b);
    }

    void ScrolledWindow::set_propagate_natural_width(bool b)
    {
        gtk_scrolled_window_set_propagate_natural_width(GTK_SCROLLED_WINDOW(_internal), b);
    }

    bool ScrolledWindow::get_propagate_natural_height() const
    {
        return gtk_scrolled_window_get_propagate_natural_height(GTK_SCROLLED_WINDOW(_internal));
    }

    bool ScrolledWindow::get_propagate_natural_width() const
    {
        return gtk_scrolled_window_get_propagate_natural_width(GTK_SCROLLED_WINDOW(_internal));
    }

    void ScrolledWindow::set_horizontal_scrollbar_policy(ScrollbarVisibilityPolicy policy)
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(GTK_SCROLLED_WINDOW(_internal), &h_policy, &v_policy);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(_internal), (GtkPolicyType) policy, h_policy);
    }

    void ScrolledWindow::set_vertical_scrollbar_policy(ScrollbarVisibilityPolicy policy)
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(GTK_SCROLLED_WINDOW(_internal), &h_policy, &v_policy);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(_internal), v_policy, (GtkPolicyType) policy);
    }

    ScrollbarVisibilityPolicy ScrolledWindow::get_horizontal_scrollbar_policy() const
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(GTK_SCROLLED_WINDOW(_internal), &h_policy, &v_policy);
        return (ScrollbarVisibilityPolicy) h_policy;
    }

    ScrollbarVisibilityPolicy ScrolledWindow::get_vertical_scrollbar_policy() const
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(GTK_SCROLLED_WINDOW(_internal), &h_policy, &v_policy);
        return (ScrollbarVisibilityPolicy) v_policy;
    }

    void ScrolledWindow::set_scrollbar_placement(CornerPlacement content_relative_to_scrollbars)
    {
        gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW(_internal), (GtkCornerType) content_relative_to_scrollbars);
    }

    CornerPlacement ScrolledWindow::get_scrollbar_placement() const
    {
        return (CornerPlacement) gtk_scrolled_window_get_placement(GTK_SCROLLED_WINDOW(_internal));
    }

    bool ScrolledWindow::get_has_frame() const
    {
        return gtk_scrolled_window_get_has_frame(GTK_SCROLLED_WINDOW(_internal));
    }

    void ScrolledWindow::set_has_frame(bool b)
    {
        gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(_internal), b);
    }

    Adjustment ScrolledWindow::get_horizontal_adjustment()
    {
        return Adjustment(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(_internal)));
    }

    Adjustment ScrolledWindow::get_vertical_adjustment()
    {
        return Adjustment(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(_internal)));
    }


    bool ScrolledWindow::get_kinetic_scrolling_enabled() const
    {
        return gtk_scrolled_window_get_kinetic_scrolling(GTK_SCROLLED_WINDOW(_internal));
    }

    void ScrolledWindow::set_kinetic_scrolling_enabled(bool b)
    {
        gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(_internal), b);
    }

    void ScrolledWindow::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(ScrolledWindow::set_child, this, ptr);

        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(_internal), child.operator NativeWidget());
    }

    void ScrolledWindow::remove_child()
    {
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(_internal), nullptr);
    }
}