// 
// Copyright 2022 Clemens Cords
// Created on 10/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Fixed : public WidgetImplementation<GtkFixed>
    {
        public:
            Fixed();

            void add_child(Widget*, Vector2f);
            void remove_child(Widget*);

            void set_child_position(Widget*, Vector2f);
            Vector2f get_child_position(Widget*);
    };
}
