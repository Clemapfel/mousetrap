// 
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gdk/gdk.h>
#include <include/widget.hpp>
#include <include/container.hpp>

namespace mousetrap
{
    class Window : public Container
    {
        public:
            Window(GtkWindowType);
            ~Window();

            GtkWidget* get_native() override;

            void set_maximized(bool);
            void set_fullscreen(bool);

            void present();

        private:
            GtkWindow* _native;
    };
}

// ###

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