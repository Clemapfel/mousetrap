// 
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class AspectFrame : public WidgetImplementation<GtkAspectFrame>
    {
        public:
            AspectFrame(float ratio, float x_align = 0.5, float y_align = 0.5, bool obey_child = false);

            void set_ratio(float);
            void set_child_xalign(float);
            void set_child_yalign(float);

            void set_child(Widget*);
            void remove_child();
    };
}

#include <src/aspect_frame.inl>