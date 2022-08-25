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

            GtkWidget* get_native();

            TODO SPLIT FRAMES AND ASPECT FRAMES

        private:
            GtkAspectFrame* _native;
    };
}

#include <src/frame.inl>