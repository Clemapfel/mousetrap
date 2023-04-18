//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#include <include/scrolled_window.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    ScrolledWindow::ScrolledWindow()
        : WidgetImplementation<GtkScrolledWindow>(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new())),
          CTOR_SIGNAL(ScrolledWindow, scroll_child)
    {
        _h_adjustment = new Adjustment(gtk_scrolled_window_get_hadjustment(get_native()));
        _v_adjustment = new Adjustment(gtk_scrolled_window_get_hadjustment(get_native()));
    }

    ScrolledWindow::~ScrolledWindow()
    {
        delete _h_adjustment;
        delete _v_adjustment;
    }

    void ScrolledWindow::set_propagate_natural_height(bool b)
    {
        gtk_scrolled_window_set_propagate_natural_height(get_native(), b);
    }

    void ScrolledWindow::set_propagate_natural_width(bool b)
    {
        gtk_scrolled_window_set_propagate_natural_width(get_native(), b);
    }

    bool ScrolledWindow::get_propagate_natural_height() const
    {
        return gtk_scrolled_window_get_propagate_natural_height(get_native());
    }

    bool ScrolledWindow::get_propagate_natural_width() const
    {
        return gtk_scrolled_window_get_propagate_natural_width(get_native());
    }

    void ScrolledWindow::set_horizontal_scrollbar_policy(ScrollbarVisibilityPolicy policy)
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(get_native(), &h_policy, &v_policy);
        gtk_scrolled_window_set_policy(get_native(), (GtkPolicyType) policy, h_policy);
    }

    void ScrolledWindow::set_vertical_scrollbar_policy(ScrollbarVisibilityPolicy policy)
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(get_native(), &h_policy, &v_policy);
        gtk_scrolled_window_set_policy(get_native(), v_policy, (GtkPolicyType) policy);
    }

    ScrollbarVisibilityPolicy ScrolledWindow::get_horizontal_scrollbar_policy() const
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(get_native(), &h_policy, &v_policy);
        return (ScrollbarVisibilityPolicy) h_policy;
    }

    ScrollbarVisibilityPolicy ScrolledWindow::get_vertical_scrollbar_policy() const
    {
        GtkPolicyType h_policy;
        GtkPolicyType v_policy;
        gtk_scrolled_window_get_policy(get_native(), &h_policy, &v_policy);
        return (ScrollbarVisibilityPolicy) v_policy;
    }

    void ScrolledWindow::set_scrollbar_placement(CornerPlacement content_relative_to_scrollbars)
    {
        gtk_scrolled_window_set_placement(get_native(), (GtkCornerType) content_relative_to_scrollbars);
    }

    CornerPlacement ScrolledWindow::get_scrollbar_placement() const
    {
        return (CornerPlacement) gtk_scrolled_window_get_placement(get_native());
    }

    bool ScrolledWindow::get_has_frame() const
    {
        return gtk_scrolled_window_get_has_frame(get_native());
    }

    void ScrolledWindow::set_has_frame(bool b)
    {
        gtk_scrolled_window_set_has_frame(get_native(), b);
    }

    Adjustment& ScrolledWindow::get_horizontal_adjustment()
    {
        return *_h_adjustment;
    }

    Adjustment& ScrolledWindow::get_vertical_adjustment()
    {
        return *_v_adjustment;
    }

    const Adjustment& ScrolledWindow::get_horizontal_adjustment() const
    {
        return *_h_adjustment;
    }

    const Adjustment& ScrolledWindow::get_vertical_adjustment() const
    {
        return *_v_adjustment;
    }

    bool ScrolledWindow::get_kinetic_scrolling_enabled() const
    {
        return gtk_scrolled_window_get_kinetic_scrolling(get_native());
    }

    void ScrolledWindow::set_kinetic_scrolling_enabled(bool b)
    {
        gtk_scrolled_window_set_kinetic_scrolling(get_native(), b);
    }

    Widget* ScrolledWindow::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    void ScrolledWindow::set_child(const Widget& child)
    {
        auto* ptr = &child;
        WARN_IF_SELF_INSERTION(ScrolledWindow::set_child, this, ptr);

        _child = ptr;
        gtk_scrolled_window_set_child(get_native(), child.operator NativeWidget());
    }
}