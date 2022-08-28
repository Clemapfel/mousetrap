// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/range.hpp>

namespace mousetrap
{
    class SpinButton : public Widget
    {
        public:
            SpinButton(float min, float max, float step);

            operator GtkWidget*() override;

            void set_value(float);
            float get_value();

            void set_digits(size_t);
            void set_wrap(bool);

        private:
            GtkSpinButton* _native;
    };
}

#include <src/spin_button.inl>
