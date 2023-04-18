//
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

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

        /// @brief whether scrollbar is visible or hidden is entirely decided by the user
        EXTERNAL = GTK_POLICY_EXTERNAL
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

    /// @brief viewport that shows part of a widget, controllable by user interaction
    class ScrolledWindow : public WidgetImplementation<GtkScrolledWindow>,
        HAS_SIGNAL(ScrolledWindow, scroll_child)
    {
        public:
            /// @brief create
            ScrolledWindow();

            /// @brief destroy
            ~ScrolledWindow();

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
            /// @param corner_placement
            void set_scrollbar_placement(CornerPlacement content_relative_to_scrollbars);

            /// @brief get placement of scrollbars
            /// @return corner placement
            CornerPlacement get_scrollbar_placement() const;

            /// @brief set whether the window has rounded corners and a dark outline
            /// @param b true if window should have rounded cornrers and a dark outline, false otherwise
            void set_has_frame(bool);

            /// @brief get whether the window has rounded corners and a dark outline
            /// @param b true if window has rounded cornrers and a dark outline, false otherwise
            bool get_has_frame() const;

            /// @brief get adjument that controls the horizontal scrollbar
            /// @return adjustment
            Adjustment& get_horizontal_adjustment();

            /// @brief get adjustment for the horizontal scrollbar
            /// @return adjustment, const
            const Adjustment& get_horizontal_adjustment() const;

            /// @brief get adjustment that controls the vertical scrollbar
            /// @return adjustment
            Adjustment& get_vertical_adjustment();

            /// @brief get adjustment that controls vertical scrollbar
            /// @return adjustment, const
            const Adjustment& get_vertical_adjustment() const;

            /// @brief set whether kinetic scrolling is recognized by the default event handler
            /// @param b true if enabled, false otherwise
            void set_kinetic_scrolling_enabled(bool);

            /// @brief set whether kinetic scroll is recognized by the default event handler
            /// @return true if enabled, false otherwise
            bool get_kinetic_scrolling_enabled() const;

            /// @brief set the windows child
            /// @param widget
            void set_child(const Widget&);

            /// @brief get the windows child
            /// @returns child
            Widget* get_child() const;

        private:
            const Widget* _child = nullptr;
            Adjustment* _h_adjustment = nullptr;
            Adjustment* _v_adjustment = nullptr;
    };
}


