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

        private:
            GtkFrame* _native;
    };
}

#include <src/frame.inl>