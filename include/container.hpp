// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Container : public Widget
    {
        public:
            virtual void add(GtkWidget*) = 0;
            void add(Widget*);

            virtual void remove(GtkWidget*) = 0;
            void remove(Widget*);

            void clear();
    };
}

#include <src/container.inl>