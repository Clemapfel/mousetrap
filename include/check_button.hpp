//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief state of a check button
    enum class CheckButtonState
    {
        /// @brief active, checkmark appears inside button bounds
        ACTIVE = 1,

        /// @brief inactive, inside button is empty
        INACTIVE = 0,

        /// @brief inconsistent, shows a `-` symbol instead of a checkmark
        INCONSISTENT = -1
    };

    /// @brief check button, displays a boolean state as a checkmark
    class CheckButton : public WidgetImplementation<GtkCheckButton>,
        HAS_SIGNAL(CheckButton, activate),
        HAS_SIGNAL(CheckButton, toggled)
    {
        public:
            /// @brief construct button
            CheckButton();

            /// @brief set state, this emits the toggled signal
            /// @param state
            void set_state(CheckButtonState);

            /// @brief get state
            /// @return mousetrap::CheckButtonState
            CheckButtonState get_state() const;

            /// @brief check whether state is active
            /// @return true if state is CheckButtonState::ACTIVE, false otherwise
            bool get_active() const;

            #if GTK_MINOR_VERSION >= 8

            /// @brief set widget displays next to the button, usually used for a text label
            /// @param widget
            void set_child(const Widget&);

            /// @brief remove widget
            void remove_child();

            /// @brief get widget that is displayed next to the button, or nullptr if no such widget was set
            /// @return widget
            Widget* get_child() const;

            #endif

        private:
            const Widget* _child = nullptr;
    };
}
