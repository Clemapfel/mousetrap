// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Label : public WidgetImplementation<GtkLabel>
    {
        public:
            Label();
            Label(const std::string&);

            void set_text(const std::string&);
            std::string get_text();
            void set_use_markup(bool b);
    };
}

#include <src/label.inl>