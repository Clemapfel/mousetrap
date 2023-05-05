//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/transition_type.hpp>

namespace mousetrap
{
    /// @brief widget that contains exactly one child and animates the process of hiding / showing it
    /// \signals
    /// \signal_revealed{Revealer}
    /// \widget_signals{Revealer}
    class Revealer : public WidgetImplementation<GtkRevealer>,
        HAS_SIGNAL(Revealer, revealed)
    {
        public:
            /// @brief construct
            /// @param transition_type which animation style to use
            Revealer(RevealerTransitionType transition_type = RevealerTransitionType::CROSSFADE);

            /// @brief set child
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief remove child
            void remove_child();

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
            void set_transition_type(RevealerTransitionType transition_type);

            /// @brief get animation style
            /// @return transition type
            RevealerTransitionType get_transition_type() const;

            /// @brief set amount of time that the hide/show animation will take
            /// @param time
            void set_transition_duration(Time time);

            /// @brief get amount of time that the hide/show animation takes
            /// @return time
            Time get_transition_duration() const;

        private:
            const Widget* _child = nullptr;
    };
}