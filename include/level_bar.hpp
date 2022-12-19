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

// #include <source/level_bar.inl>

namespace mousetrap
{
    LevelBar::LevelBar(float min, float max)
        : WidgetImplementation<GtkLevelBar>(GTK_LEVEL_BAR(gtk_level_bar_new_for_interval(min, max)))
    {}

    void LevelBar::add_offset_marker(std::string name, float value)
    {
        gtk_level_bar_add_offset_value(get_native(), name.c_str(), value);
    }

    void LevelBar::set_inverted(bool b)
    {
        gtk_level_bar_set_inverted(get_native(), b);
    }

    bool LevelBar::get_inverted() const
    {
        return gtk_level_bar_get_inverted(get_native());
    }

    void LevelBar::set_mode(LevelBarMode mode)
    {
        gtk_level_bar_set_mode(get_native(), (GtkLevelBarMode) mode);
    }

    LevelBarMode LevelBar::get_mode() const
    {
        return (LevelBarMode) gtk_level_bar_get_mode(get_native());
    }

    void LevelBar::set_min_value(float min)
    {
        gtk_level_bar_set_min_value(get_native(), min);
    }

    float LevelBar::get_min_value() const
    {
        return gtk_level_bar_get_min_value(get_native());
    }

    void LevelBar::set_max_value(float max)
    {
        gtk_level_bar_set_max_value(get_native(), max);
    }

    float LevelBar::get_max_value() const
    {
        return gtk_level_bar_get_max_value(get_native());
    }

    void LevelBar::set_value(float v)
    {
        gtk_level_bar_set_value(get_native(), v);
    }

    float LevelBar::get_value() const
    {
        return gtk_level_bar_get_value(get_native());
    }
}
