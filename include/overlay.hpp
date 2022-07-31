// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/container.hpp>

namespace mousetrap
{
    class Overlay : public Container
    {
        public:
            Overlay();
            ~Overlay();

            GtkWidget* get_native();

            template<typename T>
            void set_under(T);

            template<typename T>
            void set_over(T);

        private:
            GtkOverlay* _overlay;
    };
}

#include <src/overlay.inl>
