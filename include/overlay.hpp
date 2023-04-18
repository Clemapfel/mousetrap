//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    /// @brief a widget that allows to display multiple widgets above a common child
    class Overlay : public WidgetImplementation<GtkOverlay>
    {
        public:
            /// @brief construct empty
            Overlay();

            /// @brief set the bottom-most widget
            /// @param widget
            void set_child(const Widget&);

            /// @brief remove the bottom-most widget
            void remove_child();

            /// @brief get the bottom-most widget
            /// @return widget
            Widget* get_child() const;

            /// @brief add a widget on top
            /// @param widget
            /// @param include_in_measurement should the entire stacked overlay of widgets be resized if <tt>widget</tt> size exceeds that of the base childs size
            /// @param clip should the widget be clipped if the overlays size is smaller than that of the overlaid widget
            void add_overlay(const Widget&, bool included_in_measurement = true, bool clip = false);

            /// @brief remove a widget from the overlay, this does not affect the bottom-most child widget
            /// @param widget
            void remove_overlay(const Widget&);

        private:
            const Widget* _child = nullptr;
    };
}


