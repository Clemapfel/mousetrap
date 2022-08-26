// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class ScrolledWindow : public Widget
    {
        public:
            ScrolledWindow();

            operator GtkWidget*() override;

            void set_min_content_width(float);
            void set_min_content_height(float);
            void set_max_content_width(float);
            void set_max_content_height(float);

            void set_policy(GtkPolicyType horizontal, GtkPolicyType vertical);
            void set_placement(GtkCornerType content_relative_to_scrollbars);
            void set_has_frame(bool);

            void set_kinetic_scrolling_enabled(bool);

        private:
            GtkScrolledWindow* _native;
    };
}

#include <src/scrolled_window.inl>
