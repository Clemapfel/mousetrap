//
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/adjustment.hpp>

namespace mousetrap
{
    /// @brief decides when scrollbars are hidden / shown
    enum class ScrollbarVisibilityPolicy
    {
        /// @brief scrollbars always hidden
        NEVER = GTK_POLICY_NEVER,

        /// @brief scrollbar is always visible
        ALWAYS = GTK_POLICY_ALWAYS,

        /// @brief whether scrollbar is visible is decided by the default handler
        AUTOMATIC = GTK_POLICY_AUTOMATIC,
    };

    /// @brief placement of the scrollbars relative to the window
    enum class CornerPlacement
    {
        /// @brief in the top left corner
        TOP_LEFT = GTK_CORNER_TOP_LEFT,

        /// @brief in the top right corner
        TOP_RIGHT = GTK_CORNER_TOP_RIGHT,

        /// @brief in the bottom left corner
        BOTTOM_LEFT = GTK_CORNER_BOTTOM_LEFT,

        /// @brief in the bottom right corner
        BOTTOM_RIGHT = GTK_CORNER_BOTTOM_RIGHT
    };

    #ifndef DOXYGEN
    class Viewport;
    namespace detail
    {
        using ViewportInternal = GtkScrolledWindow;
        DEFINE_INTERNAL_MAPPING(Viewport);
    }
    #endif

    /// @brief viewport that shows part of a widget, controllable by user interaction
    /// \signals
    /// \signal_scroll_child{Viewport}
    /// \widget_signals{Viewport}
    class Viewport : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Viewport, scroll_child),
        HAS_SIGNAL(Viewport, realize),
        HAS_SIGNAL(Viewport, unrealize),
        HAS_SIGNAL(Viewport, destroy),
        HAS_SIGNAL(Viewport, hide),
        HAS_SIGNAL(Viewport, show),
        HAS_SIGNAL(Viewport, map),
        HAS_SIGNAL(Viewport, unmap)
    {
        public:
            /// @brief create
            Viewport();

            /// @brief create from internal
            Viewport(detail::ViewportInternal*);

            /// @brief destroy
            ~Viewport();

            /// @brief destructor
            NativeObject get_internal() const;

            /// @brief should the windows height be that of its child
            /// @param b if true, window listens to size, false otherwise
            void set_propagate_natural_height(bool);

            /// @brief get whether the window should listen to the height of its child
            /// @return true if window listens to size, false otherwise
            bool get_propagate_natural_height() const;

            /// @brief should the windows width be that of its child
            /// @param b if true, window listens to size, false otherwise
            void set_propagate_natural_width(bool);

            /// @brief get wehter the window should listen to the width of its child
            /// @return true if window listens to size, false otherwise
            bool get_propagate_natural_width() const;

            /// @brief set visibility policy for the horizontal scrollbar
            /// @param policy
            void set_horizontal_scrollbar_policy(ScrollbarVisibilityPolicy);

            /// @brief get visibility policy of the horizontal scrollbar
            /// @return policy
            ScrollbarVisibilityPolicy get_horizontal_scrollbar_policy() const;

            /// @brief set visibility policy for vertical scrollbar
            /// @param policy
            void set_vertical_scrollbar_policy(ScrollbarVisibilityPolicy);

            /// @brief get visibility policy for vertical scrollbar
            /// @return policy
            ScrollbarVisibilityPolicy get_vertical_scrollbar_policy() const;

            /// @brief set placement of the scrollbars
            /// @param content_relative_to_scrollbars
            void set_scrollbar_placement(CornerPlacement content_relative_to_scrollbars);

            /// @brief get placement of scrollbars
            /// @return corner placement
            CornerPlacement get_scrollbar_placement() const;

            /// @brief set whether the window has rounded corners and a dark outline
            /// @param b true if window should have rounded cornrers and a dark outline, false otherwise
            void set_has_frame(bool);

            /// @brief get whether the window has rounded corners and a dark outline
            /// @return b true if window has rounded cornrers and a dark outline, false otherwise
            bool get_has_frame() const;

            /// @brief get adjument that controls the horizontal scrollbar
            /// @return adjustment
            Adjustment get_horizontal_adjustment();

            /// @brief get adjustment that controls the vertical scrollbar
            /// @return adjustment
            Adjustment get_vertical_adjustment();

            /// @brief set whether kinetic scrolling is recognized by the default event handler
            /// @param b true if enabled, false otherwise
            void set_kinetic_scrolling_enabled(bool);

            /// @brief set whether kinetic scroll is recognized by the default event handler
            /// @return true if enabled, false otherwise
            bool get_kinetic_scrolling_enabled() const;

            /// @brief set the windows child
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief remove child
            void remove_child();

        private:
            detail::ViewportInternal* _internal = nullptr;
    };
}


