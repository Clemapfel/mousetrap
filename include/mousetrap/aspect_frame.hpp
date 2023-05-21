//
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class AspectFrame;
    namespace detail
    {
        using AspectFrameInternal = GtkAspectFrame;
        DEFINE_INTERNAL_MAPPING(AspectFrame);
    }
    #endif

    /// @brief container widget that assures childs size conforms to given aspect ratio
    /// \signals
    /// \widget_signals{AspectFrame}
    class AspectFrame : public Widget
    {
        public:
            /// @brief construct
            /// @param ratio aspect ratio, width / height
            /// @param x_align horizontal alignment of child, in [0, 1] where 0 for left-most, 1 for right-most
            /// @param y_align vertical alignment of child, in [0, 1] where 0 for top-most, 1 for bottom-most
            AspectFrame(float ratio, float x_align = 0.5, float y_align = 0.5);

            /// @brief construct from internal \internal
            /// @param internal
            AspectFrame(detail::AspectFrameInternal*);

            /// @brief dtor
            ~AspectFrame();

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const;

            /// @brief set ratio, width / height
            /// @param ratio new ratio as fraction
            void set_ratio(float ratio);

            /// @brief get ratio
            /// @return float
            float get_ratio() const;

            /// @brief set child horizontal alignment
            /// @param value in [0, 1] where 0 for left-most, 1 for right-most
            void set_child_x_alignment(float value);

            /// @brief set child vertical alignment
            /// @param value in [0, 1] where 0 for top-most, 1 for bottom-most
            void set_child_y_alignment(float value);

            /// @brief get child horizontal alignment
            /// @return float
            float get_child_x_alignment() const;

            /// @brief get child vertical alignment
            /// @return float
            float get_child_y_alignment() const;

            /// @brief set child
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief get child
            /// @return widget
            Widget* get_child() const;

            /// @brief remove child, equivalent to <tt>set_child(nullptr)</tt>
            void remove_child();

        private:
            detail::AspectFrameInternal* _internal = nullptr;
    };
}
