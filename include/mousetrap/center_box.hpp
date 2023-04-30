//
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientable.hpp>

namespace mousetrap
{
    /// @brief container widget that arranges a widget in the center with two widgets surrounding it
    /// \signals
    /// \widget_signals{CenterBox}
    class CenterBox : public WidgetImplementation<GtkCenterBox>, public Orientable
    {
        public:
            /// @brief construct, widgets will be arrange left and right if orientation is horizontal, top and bottom otherwise
            /// @param orientation
            CenterBox(Orientation);

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

            /// @brief if orientation is horizontal, get widget to the left of the center, top of the center otherwise
            /// @return widget, may be nullptr
            Widget* get_start_child() const;

            /// @brief get widget at center
            /// @return widget, may be nullptr
            Widget* get_center_child() const;

            /// @brief if orientation is horizontal, get widget to the left of the center, top of the center otherwise
            /// @return widget, may be nullptr
            Widget* get_end_child() const;

            /// @copydoc Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc Orientable::get_orientation
            Orientation get_orientation() const;

        private:
            const Widget* _start = nullptr;
            const Widget* _center = nullptr;
            const Widget* _end = nullptr;
    };
}
