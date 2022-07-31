// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/container.hpp>

namespace mousetrap
{
    class Box : public Widget, public Container
    {
        public:
            Box(GtkOrientation, float spacing = 0);
            ~Box();

            GtkWidget* get_native();
            using Container::add;

        private:
            GtkBox* _box;
    };

    Box HBox(float spacing);
    Box VBox(float spacing);
}

#include <src/box.inl>
