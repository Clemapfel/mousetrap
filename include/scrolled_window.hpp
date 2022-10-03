// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    class ScrolledWindow : public WidgetImplementation<GtkScrolledWindow>
    {
        public:
            ScrolledWindow();

            void set_min_content_width(float);
            void set_min_content_height(float);
            void set_max_content_width(float);
            void set_max_content_height(float);

            void set_propagate_natural_height(bool);
            void set_propagate_natural_width(bool);

            void set_policy(GtkPolicyType horizontal, GtkPolicyType vertical);
            void set_placement(GtkCornerType content_relative_to_scrollbars);
            void set_has_frame(bool);

            void set_hadjustment(Adjustment&);
            void set_vadjustment(Adjustment&);

            void set_kinetic_scrolling_enabled(bool);

            void set_child(Widget*);
    };
}

#include <src/scrolled_window.inl>
