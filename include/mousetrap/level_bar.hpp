//
// Copyright 2022 Clemens Cords
// Created on 12/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientable.hpp>

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

    #ifndef DOXYGEN
    class LevelBar;
    namespace detail
    {
        using LevelBarInternal = GtkLevelBar;
        DEFINE_INTERNAL_MAPPING(LevelBar);
    }
    #endif

    /// @brief level bar to indicate a value as a fraction of a whole
    /// \signals
    /// \widget_signals{LevelBar}
    class LevelBar : public Widget,
        HAS_SIGNAL(LevelBar, realize),
        HAS_SIGNAL(LevelBar, unrealize),
        HAS_SIGNAL(LevelBar, destroy),
        HAS_SIGNAL(LevelBar, hide),
        HAS_SIGNAL(LevelBar, show),
        HAS_SIGNAL(LevelBar, map),
        HAS_SIGNAL(LevelBar, unmap)
    {
        public:
            /// @brief construct
            /// @param min minimum value
            /// @param max maximum value
            LevelBar(float min, float max);

            /// @brief construct from internal
            LevelBar(detail::LevelBarInternal*);

            /// @brief destructor
            ~LevelBar();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add a marker to the part of the bar that corresponds to a value
            /// @param name text to display
            /// @param value
            void add_marker(const std::string& name, float value);

            /// @brief remove a marker with given name
            /// @param name
            void remove_marker(const std::string& name);

            /// @brief if whether the bar should be drawn inverted (bottom-to-top instead of top-to-bottom, or right-to-left instead of left-to-right, depending on orientation
            /// @param b
            void set_inverted(bool b);

            /// @brief get whether the bar should be drawn inverted (bottom-to-top instead of top-to-bottom, or right-to-left instead of left-to-right, depending on orientation
            /// @return bool
            bool get_inverted() const;

            /// @brief choose mode, continuous vs discrete
            /// @param mode
            void set_mode(LevelBarMode mode);

            /// @brief get mode, continuous vs discrete
            /// @return mode
            LevelBarMode get_mode() const;

            /// @brief set lower bound of the values
            /// @param min value
            void set_min_value(float min);

            /// @brief get lower bound of the values
            /// @return min value
            float get_min_value() const;

            /// @brief set upper bound of the values
            /// @param max value
            void set_max_value(float max);

            /// @brief get upper bound of the values
            /// @return max value
            float get_max_value() const;

            /// @brief set value the level bar is displaying
            /// @param value
            void set_value(float value);

            /// @brief get value the level bar is displaying
            /// @return value
            float get_value() const;

            /// @copydoc Box::get_orientation
            Orientation get_orientation() const;

            /// @copydoc Box::set_orientation
            void set_orientation(Orientation orientation);

        private:
            detail::LevelBarInternal* _internal = nullptr;
    };
}
