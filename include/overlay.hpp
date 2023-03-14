// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Overlay : public WidgetImplementation<GtkOverlay>
    {
        public:
            Overlay();

            void set_child(Widget*);
            void add_overlay(Widget*);

            void remove_child();
            void remove_overlay(Widget*);
    };
}


