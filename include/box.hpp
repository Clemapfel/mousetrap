// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Box : public Widget
    {
        public:
            Box(GtkOrientation, float spacing = 0);

            operator GtkWidget*() override;

            void push_back(Widget*);
            void push_front(Widget*);
            void insert_after(Widget* to_append, Widget* after);
            void reorder_after(Widget* to_move, Widget* after);

            void remove(Widget*);
            void clear();

        private:
            GtkBox* _native;
    };

    Box HBox(float spacing = 0);
    Box VBox(float spacing = 0);
}

#include <src/box.inl>
