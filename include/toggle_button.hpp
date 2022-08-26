// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class ToggleButton : public Widget
    {
        public:
            ToggleButton();

            operator GtkWidget*() override;

            bool get_active() const;
            void set_active(bool b);

            void set_label(const std::string&);


        private:
            GtkToggleButton* _native;
    };
}

#include <src/toggle_button.inl>