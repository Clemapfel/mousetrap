//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/transition_type.hpp>

namespace mousetrap
{
    /// @brief widget that contains exactly one child and animates the process of hiding / showing it
    class Revealer : public WidgetImplementation<GtkRevealer>
    {
        public:
            /// @brief construct
            /// @param transition_type which animation style to use
            Revealer(RevealerTransitionType = RevealerTransitionType::CROSSFADE);

            /// @brief set child
            /// @param widget
            void set_child(const Widget&);

            /// @brief get child
            /// @return child
            Widget* get_child() const;

            /// @brief set whether the child is shown. If the state changes, the animation is played
            /// @param b false if the child should be hidden, true otherwise
            void set_revealed(bool b);

            /// @brief get whether the child is currently being shown
            /// @return true if the child is visible, false otherwise
            bool get_revealed() const;

            /// @brief set animation style
            /// @param transition_type which animation style to use
            void set_transition_type(RevealerTransitionType);

            /// @brief get animation style
            /// @return transition type
            RevealerTransitionType get_transition_type() const;

            /// @brief set amount of time that the hide/show animation will take
            /// @param time
            void set_transition_duration(Time);

            /// @brief get amount of time that the hide/show animation takes
            /// @return time
            Time get_transition_duration() const;

        private:
            const Widget* _child = nullptr;
    };
}