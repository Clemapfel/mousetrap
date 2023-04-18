//
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    /// @brief container widget that assures childs size conforms to given aspect ratio
    class AspectFrame : public WidgetImplementation<GtkAspectFrame>
    {
        public:
            /// @brief construct
            /// @param ratio aspect ratio, width / height
            /// @param x_align horizontal alignment of child, in [0, 1] where 0 for left-most, 1 for right-most
            /// @param y_align vertical alignment of child, in [0, 1] where 0 for top-most, 1 for bottom-most
            AspectFrame(float ratio, float x_align = 0.5, float y_align = 0.5);

            /// @brief set ratio, width / height
            /// @param ratio new ratio as fraction
            void set_ratio(float);

            /// @brief get ratio
            /// @return float
            float get_ratio() const;

            /// @brief set child horizontal alignment
            /// @param value in [0, 1] where 0 for left-most, 1 for right-most
            void set_child_x_alignment(float);

            /// @brief set child vertical alignment
            /// @param value in [0, 1] where 0 for top-most, 1 for bottom-most
            void set_child_y_alignment(float);

            /// @brief get child horizontal alignment
            /// @return float
            float get_child_x_alignment() const;

            /// @brief get child vertical alignment
            /// @return float
            float get_child_y_alignment() const;

            /// @brief set child
            /// @param widget, can be nullptr
            void set_child(Widget*);

            /// @brief get child
            /// @return widget
            Widget* get_child() const;

            /// @brief remove child, equivalent to <tt>set_child(nullptr)</tt>
            void remove_child();

        private:
            Widget* _child = nullptr;
    };
}
