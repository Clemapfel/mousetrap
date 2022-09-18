// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    class Scrollbar : public WidgetImplementation<GtkScrollbar>
    {
        public:
            Scrollbar(Adjustment, GtkOrientation = GTK_ORIENTATION_HORIZONTAL);
    };
}

#include <src/scrollbar.inl>