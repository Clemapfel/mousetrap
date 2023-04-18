//
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/orientable.hpp>

namespace mousetrap
{
    /// @brief level bar displa ymode
    enum LevelBarMode
    {
        /// @brief level bar does not have steps
        CONTINUOUS = GTK_LEVEL_BAR_MODE_CONTINUOUS,

        /// @brief level bar is displayed in steps
        DISCRETE = GTK_LEVEL_BAR_MODE_DISCRETE
    };

    /// @brief level bar to indicate a value as a fraction of a whole
    class LevelBar : public WidgetImplementation<GtkLevelBar>, public Orientable
    {
        public:
            /// @brief construct
            /// @param min minimum value
            /// @param max maximum value
            LevelBar(float min, float max);

            /// @brief add a marker to the part of the bar that corresponds to a value
            /// @param name text to display
            /// @param value
            void add_marker(const std::string& name, float value);

            /// @brief remove a marker with given name
            /// @param name
            void remove_marker(const std::string& name);

            /// @brief if whether the bar should be drawn inverted (bottom-to-top instead of top-to-bottom, or right-to-left instead of left-to-right, depending on orientation
            /// @param b
            void set_inverted(bool);

            /// @brief get whether the bar should be drawn inverted (bottom-to-top instead of top-to-bottom, or right-to-left instead of left-to-right, depending on orientation
            /// @return bool
            bool get_inverted() const;

            /// @brief choose mode, continuous vs discrete
            /// @param mode
            void set_mode(LevelBarMode);

            /// @brief get mode, continuous vs discrete
            /// @return mode
            LevelBarMode get_mode() const;

            /// @brief set lower bound of the values
            /// @param min value
            void set_min_value(float min);

            /// @brief get lower bound of the values
            /// @param min value
            float get_min_value() const;

            /// @brief set upper bound of the values
            /// @param max value
            void set_max_value(float max);

            /// @brief get upper bound of the values
            /// @param max value
            float get_max_value() const;

            /// @brief set value the level bar is displaying
            /// @param value
            void set_value(float);

            /// @brief get value the level bar is displaying
            /// @return value
            float get_value() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            Orientation get_orientation() const override;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;
    };
}
