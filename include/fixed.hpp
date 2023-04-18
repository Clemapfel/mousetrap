//
// Copyright 2022 Clemens Cords
// Created on 10/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    /// @brief container that positions a widget at a specific pixel position
    class Fixed : public WidgetImplementation<GtkFixed>
    {
        public:
            /// @brief construct
            Fixed();

            /// @brief add child at specified position
            /// @param widget
            /// @param position top left corner, in pixels
            void add_child(const Widget&, Vector2f);

            /// @brief remove child
            /// @param widget
            void remove_child(const Widget&);

            /// @brief set child position
            /// @param child
            /// @param position top left corner, in pixels
            void set_child_position(const Widget&, Vector2f);

            /// @brief get current child position
            /// @param widget
            /// @param position top left corner, in pixels
            Vector2f get_child_position(const Widget&);
    };
}
