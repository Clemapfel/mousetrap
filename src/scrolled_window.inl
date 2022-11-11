// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    ScrolledWindow::ScrolledWindow()
        : WidgetImplementation<GtkScrolledWindow>(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new()))
    {}
    
    void ScrolledWindow::set_child(Widget* widget)
    {
        gtk_scrolled_window_set_child(get_native(), widget == nullptr ? nullptr : widget->operator GtkWidget*());
    }

    void ScrolledWindow::set_propagate_natural_height(bool b)
    {
        gtk_scrolled_window_set_propagate_natural_height(get_native(), b);
    }

    void ScrolledWindow::set_propagate_natural_width(bool b)
    {
        gtk_scrolled_window_set_propagate_natural_height(get_native(), b);
    }

    void ScrolledWindow::set_min_content_width(float v)
    {
        gtk_scrolled_window_set_min_content_width(get_native(), v);
    }

    void ScrolledWindow::set_min_content_height(float v)
    {
        gtk_scrolled_window_set_min_content_height(get_native(), v);
    }

    void ScrolledWindow::set_max_content_width(float v)
    {
        gtk_scrolled_window_set_min_content_width(get_native(), v);
    }

    void ScrolledWindow::set_max_content_height(float v)
    {
        gtk_scrolled_window_set_min_content_height(get_native(), v);
    }

    void ScrolledWindow::set_policy(GtkPolicyType h, GtkPolicyType v)
    {
        gtk_scrolled_window_set_policy(get_native(), h, v);
    }

    void ScrolledWindow::set_placement(GtkCornerType type)
    {
        gtk_scrolled_window_set_placement(get_native(), type);
    }

    void ScrolledWindow::set_has_frame(bool b)
    {
        gtk_scrolled_window_set_has_frame(get_native(), b);
    }

    void ScrolledWindow::set_kinetic_scrolling_enabled(bool b)
    {
        gtk_scrolled_window_set_kinetic_scrolling(get_native(), b);
    }

    void ScrolledWindow::set_hadjustment(Adjustment& adjustment)
    {
        gtk_scrolled_window_set_hadjustment(get_native(), adjustment.operator GtkAdjustment*());
    }

    Adjustment ScrolledWindow::get_hadjustment()
    {
        return Adjustment(gtk_scrolled_window_get_hadjustment(get_native()));
    }

    void ScrolledWindow::set_vadjustment(Adjustment& adjustment)
    {
        gtk_scrolled_window_set_vadjustment(get_native(), adjustment.operator GtkAdjustment*());
    }

    Adjustment ScrolledWindow::get_vadjustment()
    {
        return Adjustment(gtk_scrolled_window_get_vadjustment(get_native()));
    }
}