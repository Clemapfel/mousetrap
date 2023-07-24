//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/signal_component.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/icon.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Button;
    namespace detail
    {
        using ButtonInternal = GtkButton;
        DEFINE_INTERNAL_MAPPING(Button);
    }
    #endif

    /// @brief button, triggers action or signal when clicked
    /// \signals
    /// \signal_clicked{Button}
    /// \widget_signals{Button}
    class Button : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Button, clicked),
        HAS_SIGNAL(Button, realize),
        HAS_SIGNAL(Button, unrealize),
        HAS_SIGNAL(Button, destroy),
        HAS_SIGNAL(Button, hide),
        HAS_SIGNAL(Button, show),
        HAS_SIGNAL(Button, map),
        HAS_SIGNAL(Button, unmap)
    {
        public:
            /// @brief construct
            Button();

            /// @brief destruct
            ~Button();

            /// @brief construct from internal
            /// @param internal
            Button(detail::ButtonInternal*);

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

            /// @brief set whether button should have a black outline
            /// @param b
            void set_has_frame(bool b);

            /// @brief get whether button has a black outline
            /// @return bool
            bool get_has_frame() const;

            /// @brief set whether button should be circular
            /// @param b true if circular, false otherwise
            void set_is_circular(bool);

            /// @brief get whether button is circular
            /// @return true if circular, false otherwise
            bool get_is_circular() const;

            /// @brief set widget used as the buttons label
            /// @param widget
            void set_child(const Widget&);

            /// @brief remove child
            void remove_child();

            /// @brief set child to an icon
            /// @param icon
            void set_icon(const Icon&);

            /// @brief set action triggered when the button is activated, if the action is disabled the button is also disabled automatically
            void set_action(const Action&);

        private:
            detail::ButtonInternal* _internal = nullptr;
    };
}
