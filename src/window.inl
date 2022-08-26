// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Window::Window()
    {
        _native = GTK_WINDOW(gtk_window_new());
    }

    Window::Window(GtkWindow* window)
    {
        _native = window;
    }

    Window::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Window::present()
    {
        gtk_window_present(_native);
    }

    void Window::set_maximized(bool b)
    {
        if (b)
            gtk_window_maximize(_native);
        else
            gtk_window_unmaximize(_native);
    }

    void Window::set_fullscreen(bool b )
    {
        if (b)
            gtk_window_fullscreen(_native);
        else
            gtk_window_unfullscreen(_native);
    }

    void Window::set_child(Widget* widget)
    {
        gtk_window_set_child(_native, widget->operator GtkWidget *());
    }

    void Window::remove_child()
    {
        gtk_window_set_child(_native, nullptr);
    }
}