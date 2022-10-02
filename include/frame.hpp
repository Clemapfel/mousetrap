// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Frame : public WidgetImplementation<GtkFrame>
    {
        public:
            Frame();

            void set_child(Widget*);
            void remove_child();

            void set_label_widget(Widget*);
            void set_label_align(float x_align);

    };
}

#include <src/frame.inl>