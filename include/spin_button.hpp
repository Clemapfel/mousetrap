// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class SpinButton : public WidgetImplementation<GtkSpinButton>, public HasValueChangedSignal<SpinButton>
    {
        public:
            SpinButton(float min, float max, float step);

            void set_value(float);
            float get_value();

            void set_lower_limit(float);
            float get_lower_limit();

            void set_upper_limit(float);
            float get_upper_limit();

            void set_digits(size_t);
            void set_wrap(bool);
    };
}


