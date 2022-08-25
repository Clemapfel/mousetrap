// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/container.hpp>

namespace mousetrap
{
    class Overlay
    {
        public:
            Overlay();
            virtual ~Overlay();

            GtkWidget* get_native();

            void set_under(GtkWidget*);
            void set_over(GtkWidget*);

            void remove_under();
            void remove_over(GtkWidget*);

        private:
            GtkOverlay* _overlay;
    };
}

#include <src/overlay.inl>
