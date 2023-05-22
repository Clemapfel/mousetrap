//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Overlay;
    namespace detail
    {
        using OverlayInternal = GtkOverlay;
        DEFINE_INTERNAL_MAPPING(Overlay),
    }
    #endif

    /// @brief a widget that allows to display multiple widgets above a common child
    /// \signals
    /// \widget_signals{Overlay}
    class Overlay : public Widget
    {
        public:
            /// @brief construct empty
            Overlay();

            /// @brief construct from internal
            Overlay(detail::OverlayInternal*);

            /// @brief destructor
            ~Overlay();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set the bottom-most widget
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief remove the bottom-most widget
            void remove_child();

            /// @brief add a widget on top
            /// @param widget
            /// @param included_in_measurement should the entire stacked overlay of widgets be resized if <tt>widget</tt> size exceeds that of the base childs size
            /// @param clip should the widget be clipped if the overlays size is smaller than that of the overlaid widget
            void add_overlay(const Widget& widget, bool included_in_measurement = true, bool clip = false);

            /// @brief remove a widget from the overlay, this does not affect the bottom-most child widget
            /// @param widget
            void remove_overlay(const Widget& widget);

        private:
            detail::OverlayInternal* _internal = nullptr;
    };
}


