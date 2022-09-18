// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class CheckButton : public WidgetImplementation<GtkCheckButton>
    {
        public:
            CheckButton();
            CheckButton(const std::string& label);

            bool get_is_checked();
            void set_is_checked(bool b);

            bool get_is_inconsistent();
            void set_is_inconsistent(bool b);
    };
}

#include <src/check_button.inl>
