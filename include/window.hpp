// 
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gdk/gdk.h>
#include <include/widget.hpp>

namespace mousetrap
{
    class Window : public Widget
    {
        public:
            Window();
            Window(GtkWindow*);

            operator GtkWidget*() override;

            void set_maximized(bool);
            void set_fullscreen(bool);

            void present();

            void set_child(Widget*);
            void remove_child();

            void set_focused_widget(Widget*);

        private:
            GtkWindow* _native;
    };
}

#include <src/window.inl>