//
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/orientable.hpp>

namespace mousetrap
{
    /// @brief container widget that arranges a widget in the center with two widgets surrounding it
    class CenterBox : public WidgetImplementation<GtkCenterBox>, public Orientable
    {
        public:
            /// @brief construct, widgets will be arrange left and right if orientation is horizontal, top and bottom otherwise
            /// @param orientation
            CenterBox(Orientation);

            /// @brief if orientation is horizontal, set widget to the left of the center, top of the center otherwise
            /// @param widget
            void set_start_widget(Widget*);

            /// @brief set widget at the center
            /// @param widget
            void set_center_widget(Widget*);

            /// @brief if orientation is horizontal, set widget to the right of the center, bottom of the center otherwise
            /// @param widget
            void set_end_widget(Widget*);

            /// @brief if orientation is horizontal, get widget to the left of the center, top of the center otherwise
            /// @return widget, may be nullptr
            Widget* get_start_widget() const;

            /// @brief get widget at center
            /// @return widget, may be nullptr
            Widget* get_center_widget() const;

            /// @brief if orientation is horizontal, get widget to the left of the center, top of the center otherwise
            /// @return widget, may be nullptr
            Widget* get_end_widget() const;

            /// @copydoc Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc Orientable::get_orientation
            Orientation get_orientation() const;

        private:
            Widget* _start = nullptr;
            Widget* _center = nullptr;
            Widget* _end = nullptr;
    };
}
