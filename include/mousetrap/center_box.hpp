//
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientable.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class CenterBox;
    namespace detail
    {
        using CenterBoxInternal = GtkCenterBox;
        DEFINE_INTERNAL_MAPPING(CenterBox);
    }
    #endif

    /// @brief container widget that arranges a widget in the center with two widgets surrounding it
    /// \signals
    /// \widget_signals{CenterBox}
    class CenterBox : public Widget
    {
        public:
            /// @brief construct, widgets will be arrange left and right if orientation is horizontal, top and bottom otherwise
            /// @param orientation
            CenterBox(Orientation);

            /// @brief construct from internal
            CenterBox(detail::CenterBoxInternal*);

            /// @brief destructor
            ~CenterBox();

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

            /// @brief if orientation is horizontal, set widget to the left of the center, top of the center otherwise
            /// @param widget
            void set_start_child(const Widget&);

            /// @brief set widget at the center
            /// @param widget
            void set_center_child(const Widget&);

            /// @brief if orientation is horizontal, set widget to the right of the center, bottom of the center otherwise
            /// @param widget
            void set_end_child(const Widget&);

            /// @brief remove start widget
            void remove_start_widget();

            /// @brief remove center widget
            void remove_center_widget();

            /// @brief remove end widget
            void remove_end_widget();

            /// @copydoc Box::get_orientation
            Orientation get_orientation() const;

            /// @copydoc Box::set_orientation
            void set_orientation(Orientation orientation);

        private:
            detail::CenterBoxInternal* _internal = nullptr;
    };
}
