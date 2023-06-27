//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/icon.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class ToggleButton;
    namespace detail
    {
        using ToggleButtonInternal = GtkToggleButton;
        DEFINE_INTERNAL_MAPPING(ToggleButton);
    }
    #endif

    /// @brief toggle button, allows the user to manipulate a state
    /// \signals
    /// \signal_toggled{ToggleButton}
    /// \signal_activate{ToggleButton}
    /// \signal_clicked{ToggleButton}
    /// \widget_signals{ToggleButton}
    class ToggleButton : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(ToggleButton, toggled),
        HAS_SIGNAL(ToggleButton, activate),
        HAS_SIGNAL(ToggleButton, clicked),
        HAS_SIGNAL(ToggleButton, realize),
        HAS_SIGNAL(ToggleButton, unrealize),
        HAS_SIGNAL(ToggleButton, destroy),
        HAS_SIGNAL(ToggleButton, hide),
        HAS_SIGNAL(ToggleButton, show),
        HAS_SIGNAL(ToggleButton, map),
        HAS_SIGNAL(ToggleButton, unmap)
    {
        public:
            /// @brief construct
            ToggleButton();

            /// @brief construct from internal
            ToggleButton(detail::ToggleButtonInternal*);

            /// @brief destructor
            ~ToggleButton();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief get whether the button is pressed in
            /// @return true if active, false otherwise
            bool get_is_active() const;

            /// @brief set whether the button is pressed in
            /// @param b true if active, false otherwise
            void set_is_active(bool b);

            /// @brief set widget to use as the label for the button
            /// @param widget
            void set_child(const Widget&);

            /// @brief use an icon as the buttons label
            /// @param icon
            void set_icon(const Icon&);

            /// @brief remove label widget
            void remove_child();

            /// @brief set whether the button should have rounded corners and a black outline
            /// @param b true if outline visible, false otherwise
            void set_has_frame(bool b);

            /// @brief get whether the button should have rounded corners and a black outline
            /// @return true if outline visible, false otherwise
            bool get_has_frame() const;

            /// @brief set whether button should be circular
            /// @param b
            void set_is_circular(bool);

            /// @brief get whether button is circular
            /// @return true if circular, false otherwise
            bool get_is_circular() const;

        private:
            detail::ToggleButtonInternal* _internal = nullptr;
    };
}

