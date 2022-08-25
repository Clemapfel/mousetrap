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

    void Window::add(GtkWidget* widget)
    {
        gtk_window_set_child(_native, widget);
    }

    void Window::remove(GtkWidget* widget)
    {
        if (gtk_window_get_child(_native) != widget)
        {
            std::cerr << "[WARNING] In Window::remove: Widget " << widget
                      << " is not the child of this window. No removal will take place" << std::endl;
            return;
        }

        gtk_window_set_child(_native, nullptr);
    }
}