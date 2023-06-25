//
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Frame;
    namespace detail
    {
        using FrameInternal = GtkFrame;
        DEFINE_INTERNAL_MAPPING(Frame);
    }
    #endif

    /// @brief draws frame around widget, rounded corners. Has exactly one child and an optional label widget
    /// \signals
    /// \widget_signals{Frame}
    class Frame : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Frame, realize),
        HAS_SIGNAL(Frame, unrealize),
        HAS_SIGNAL(Frame, destroy),
        HAS_SIGNAL(Frame, hide),
        HAS_SIGNAL(Frame, show),
        HAS_SIGNAL(Frame, map),
        HAS_SIGNAL(Frame, unmap)
    {
        public:
            /// @brief construct
            Frame();

            /// @brief construct from internal
            Frame(detail::FrameInternal*);

            /// @brief destructor
            ~Frame();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set the child
            /// @param child
            void set_child(const Widget& child);

            /// @brief remove child, frame is now empty
            void remove_child();

            /// @brief set label widget
            /// @param widget
            void set_label_widget(const Widget& widget);

            /// @brief remove label widget, frame no longer has a label
            void remove_label_widget();

            /// @brief set label widget horizontal alignment
            /// @param x_align float in [0, 1], 0 for left-most, 1 for right-most alignment
            void set_label_x_alignment(float x_align);

            /// @brief get label horizontal alignment
            /// @return float in [0, 1], 0 for left-most, 1 for right-most alignment
            float get_label_x_alignment() const;

        private:
            detail::FrameInternal* _internal = nullptr;
    };
}
