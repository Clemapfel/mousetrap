// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class SeparatorLine : public Widget
    {
        public:
            SeparatorLine(GtkOrientation = GTK_ORIENTATION_HORIZONTAL);

            operator GtkWidget*();

        private:
            GtkSeparator* _native;
    };
}

#include <src/separator_line.inl>