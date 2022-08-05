// 
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

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
    {
        //if (_native != nullptr)
          //  gtk_widget_destroy(GTK_WIDGET(_native));
    }

    GtkWidget* Window::get_native()
    {
        return GTK_WIDGET(_native);
    }
}