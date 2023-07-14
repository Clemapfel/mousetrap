//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/transition_type.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Revealer;
    namespace detail
    {
        using RevealerInternal = GtkRevealer;
        DEFINE_INTERNAL_MAPPING(Revealer);
    }
    #endif

    /// @brief widget that contains exactly one child and animates the process of hiding / showing it
    /// \signals
    /// \signal_revealed{Revealer}
    /// \widget_signals{Revealer}
    class Revealer : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Revealer, revealed),
        HAS_SIGNAL(Revealer, realize),
        HAS_SIGNAL(Revealer, unrealize),
        HAS_SIGNAL(Revealer, destroy),
        HAS_SIGNAL(Revealer, hide),
        HAS_SIGNAL(Revealer, show),
        HAS_SIGNAL(Revealer, map),
        HAS_SIGNAL(Revealer, unmap)
    {
        public:
            /// @brief construct
            /// @param transition_type which animation style to use
            Revealer(RevealerTransitionType transition_type = RevealerTransitionType::CROSSFADE);

            /// @brief construct from internal
            Revealer(detail::RevealerInternal*);

            /// @brief destructor
            ~Revealer();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief set child
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief remove child
            void remove_child();

            /// @brief set whether the child is shown. If the state changes, the animation is played
            /// @param b false if the child should be hidden, true otherwise
            void set_is_revealed(bool b);

            /// @brief get whether the child is currently being shown
            /// @return true if the child is visible, false otherwise
            bool get_is_revealed() const;

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
            detail::RevealerInternal* _internal = nullptr;
    };
}