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
            void add(GtkWidget*);
            void add(Widget*);
    };
}

#include <src/container.inl>