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
            Frame();

            void add(GtkWidget*) override;
            void remove(GtkWidget*) override;

            void set_label(const std::string&);
            GtkWidget* get_native() override;

        protected:
            GtkWidget* _native;
    };

    class AspectFrame : public Frame
    {
        public:
            AspectFrame(float ratio, float x_align = 0.5, float y_align = 0.5, bool obey_child = false);

            void set_ratio(float);
            void set_child_xalign(float);
            void set_child_yalign(float);

            using Frame::set_label;
    };
}

#include <src/frame.inl>