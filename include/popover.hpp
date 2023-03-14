// 
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Popover : public WidgetImplementation<GtkPopover>
    {
        public:
            Popover();

            void popup();
            void popdown();
            void present();

            void set_child(Widget*);

            void set_relative_position(GtkPositionType);
            void set_has_arrow(bool);
            void set_autohide(bool);

            void attach_to(Widget*);
    };
}


