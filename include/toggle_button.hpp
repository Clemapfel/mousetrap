//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/action.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief toggle button, allows the user to manipulate a state
    class ToggleButton : public WidgetImplementation<GtkToggleButton>,
        HAS_SIGNAL(ToggleButton, toggled),
        HAS_SIGNAL(ToggleButton, activate),
        HAS_SIGNAL(ToggleButton, clicked)
    {
        public:
            /// @brief construct
            ToggleButton();

            /// @brief construct and set widget as child
            /// @param child
            ToggleButton(Widget*);

            /// @brief get whether the button is pressed in
            /// @return true if active, false otherwise
            bool get_active() const;

            /// @brief set whether the button is pressed in
            /// @param b true if active, false otherwise
            void set_active(bool b);

            /// @brief set widget to use as the label for the button
            /// @param widget
            void set_child(Widget*);

            /// @brief get widget used as the label for the button
            /// @return child
            Widget* get_child() const;

            /// @brief set whether the button should have rounded corners and a black outline
            /// @param b true if outline visible, false otherwise
            void set_has_frame(bool b);

            /// @brief get whether the button should have rounded corners and a black outline
            /// @return true if outline visible, false otherwise
            bool get_has_frame() const;

        private:
            Widget* _child = nullptr;
    };
}

