// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Window::Window(GtkWindowType type)
    {
        _native = GTK_WINDOW(gtk_window_new(type));
    }

    Window::~Window()
    {}

    GtkWidget* Window::get_native()
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
}