//
// Copyright 2022 Clemens Cords
// Created on 10/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Fixed;
    namespace detail
    {
        using FixedInternal = GtkFixed;
        DEFINE_INTERNAL_MAPPING(Fixed);
    }
    #endif

    /// @brief container that positions a widget at a specific pixel position
    /// \signals
    /// \widget_signals{Fixed}
    class Fixed : public Widget
    {
        public:
            /// @brief construct
            Fixed();

            /// @brief construct from internal
            Fixed(detail::FixedInternal*);

            /// @brief destructor
            ~Fixed();

            /// @brief get internal
            NativeObject get_internal() const override;

            /// @brief add child at specified position
            /// @param widget
            /// @param position top left corner, in absolute widget-coordinates, in pixels
            void add_child(const Widget& widget, Vector2f position);

            /// @brief remove child
            /// @param widget
            void remove_child(const Widget& widget);

            /// @brief set child position
            /// @param widget
            /// @param position top left corner, in pixels
            void set_child_position(const Widget& widget, Vector2f position);

            /// @brief get current child position
            /// @param widget
            /// @return position top left corner, in pixels
            Vector2f get_child_position(const Widget& widget);

        private:
            detail::FixedInternal* _internal = nullptr;
    };
}
