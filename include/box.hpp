// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Box : public WidgetImplementation<GtkBox>
    {
        public:
            Box(GtkBox*);
            Box(GtkOrientation, float spacing = 0);

            void push_back(Widget*);
            void push_front(Widget*);
            void insert_after(Widget* to_append, Widget* after);
            void reorder_after(Widget* to_move, Widget* after);

            void remove(Widget*);
            void clear();

            void set_homogeneous(bool);

            GtkOrientation get_orientation() const;

            size_t get_n_items();
    };
}
