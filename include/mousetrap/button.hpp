//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/signal_component.hpp>
#include <mousetrap/action.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief button, triggers action or signal when clicked
    class Button : public WidgetImplementation<GtkButton>,
        HAS_SIGNAL(Button, activate),
        HAS_SIGNAL(Button, clicked)
    {
        public:
            /// @brief construct
            Button();

            /// @brief construct with label
            Button(const std::string&);

            /// @brief set whether button should have a black outline
            /// @param b
            void set_has_frame(bool b);

            /// @brief get whether button has a black outline
            /// @return bool
            bool get_has_frame() const;

            /// @brief set widget used as the buttons label
            /// @param widget
            void set_child(const Widget&);

            /// @brief remove child
            void remove_child();

            /// @brief get widget used as the buttons label
            /// @return widget
            Widget* get_child() const;

            /// @brief set action triggered when the button is activated, if the action is disabled the button is also disabled automatically
            void set_action(const Action&);

        private:
            const Widget* _child = nullptr;
    };
}
