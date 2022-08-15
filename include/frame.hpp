// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/container.hpp>

namespace mousetrap
{
    class Frame : public Container
    {
        public:
            Frame(float ratio, float x_align = 0.5, float y_align = 0.5, bool obey_child = false);

            void set_ratio(float);
            void set_xalign(float);
            void set_yalign(float);

            void set_label(const std::string&, float xalign, float yalign);
            void set_shadow_type(GtkShadowType);

            GtkWidget* get_native();

        private:
            float _ratio;
            float _x_align;
            float _y_align;
            bool _obey_child;

            GtkAspectFrame* _native;
    };
}

#include <src/frame.inl>