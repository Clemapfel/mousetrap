// 
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class SeparatorLine : public WidgetImplementation<GtkSeparator>
    {
        public:
            SeparatorLine(GtkOrientation = GTK_ORIENTATION_HORIZONTAL);
    };
}

#include <src/separator_line.inl>