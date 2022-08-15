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

#include <src/window.inl>