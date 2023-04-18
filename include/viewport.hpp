//
// Copyright 2022 Clemens Cords
// Created on 9/28/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    /// @brief viewport, displays part of a widget. Only scrolls if the allocated size of the viewport is less than the allocated size of the child
    class Viewport : public WidgetImplementation<GtkViewport>
    {
        public:
            /// @brief construct
            Viewport();

            /// @brief set child
            /// @param widget
            void set_child(Widget* widget);

            /// @brief get child
            /// @return child, or nullptr if childless
            Widget* get_child();

            /// @brief get adjustment that control horizontal scrolling
            /// @return adjustment
            Adjustment& get_horizontal_adjustment();

            /// @brief get adjustment that controls horizontal scrolling
            /// @return adjustment, const
            const Adjustment& get_horizontal_adjustment() const;

            /// @brief get adjustment that control vertical scrolling
            /// @return adjustment
            Adjustment& get_vertical_adjustment();

            /// @brief get adjustment that control vertical scrolling
            /// @return adjustment, const
            const Adjustment& get_vertical_adjustment() const;

        private:
            Widget* _child = nullptr;
            Adjustment* _h_adjustment = nullptr;
            Adjustment* _v_adjustment = nullptr;
    };
}


