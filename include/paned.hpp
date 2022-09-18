// 
// Copyright 2022 Clemens Cords
// Created on 9/14/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Paned : public WidgetImplementation<GtkPaned>
    {
        public:
            Paned(GtkOrientation);

            int get_position();
            void set_position(int);

            void set_start_child(Widget*);
            void set_start_child_resizable(bool);
            void set_start_child_shrinkable(bool);

            void set_end_child(Widget*);
            void set_end_child_resizable(bool);
            void set_end_child_shrinkable(bool);

            void set_has_wide_handle(bool);
    };
}

#include <include/paned.hpp>
