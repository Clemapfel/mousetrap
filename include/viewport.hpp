// 
// Copyright 2022 Clemens Cords
// Created on 9/28/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    class Viewport : public WidgetImplementation<GtkViewport>
    {
        public:
            Viewport();
            Viewport(Adjustment& h_adjustment, Adjustment& v_adjustment);
            void set_child(Widget* widget);
            void set_scroll_to_focus(bool b);

    };
}

#include <src/viewport.inl>
