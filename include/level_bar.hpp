// 
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    enum LevelBarMode
    {
        CONTINOUS = GTK_LEVEL_BAR_MODE_CONTINUOUS,
        DISCRETE = GTK_LEVEL_BAR_MODE_DISCRETE
    };

    class LevelBar : public WidgetImplementation<GtkLevelBar>
    {
        public:
            LevelBar(float min, float max);

            void add_offset_marker(std::string name, float value);

            void set_inverted(bool);
            bool get_inverted() const;

            void set_mode(LevelBarMode);
            LevelBarMode get_mode() const;

            void set_min_value(float min);
            float get_min_value() const;

            void set_max_value(float max);
            float get_max_value() const;

            void set_value(float);
            float get_value() const;
    };
}
