//
// Copyright 2022 Clemens Cords
// Created on 9/14/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Paned;
    namespace detail
    {
        using PanedInternal = GtkPaned;
        DEFINE_INTERNAL_MAPPING(Paned);
    }
    #endif

    /// @brief paned, displays two children with a draggable area in between, dragging the area resizes the respective two widgets
    /// \signals
    /// \widget_signals{Paned}
    class Paned : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Paned, realize),
        HAS_SIGNAL(Paned, unrealize),
        HAS_SIGNAL(Paned, destroy),
        HAS_SIGNAL(Paned, hide),
        HAS_SIGNAL(Paned, show),
        HAS_SIGNAL(Paned, map),
        HAS_SIGNAL(Paned, unmap)
    {
        public:
            /// @brief construct empty
            /// @brief orientation Orientation of the two child widgets, if horizontal: left-right, if vertical: top-bottom
            Paned(Orientation);

            /// @brief construct from internal
            Paned(detail::PanedInternal*);

            /// @brief expose paned
            ~Paned();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief get the current position of the draggable separator bar
            /// @return position in local widget space, in pixels
            int get_position();

            /// @brief set current position of the draggable separator bar
            /// @param v position in local window space, in pixels
            /// \bug position does not seem to use the coordinate system stated in GTK4,
            void set_position(int);

            /// @brief set start child
            /// @param widget
            void set_start_child(const Widget&);

            /// @brief remove start child
            void remove_start_child();

            /// @brief set whether the start child will increase in size if the separator bar is dragged away from it
            /// @param b true if enabled, false otherwise
            void set_start_child_resizable(bool);

            /// @brief get whether the start child will increase in size if the esparator bar is dragged away from it
            /// @return true if enabled, false otherwise
            bool get_start_child_resizable() const;

            /// @brief set whether the start child will decrease in size if the separator bar is pulled towards it
            /// @param b true to enable, false otherwise
            void set_start_child_shrinkable(bool);

            /// @brief get whether the start child will decrease in size if the separator bar is pulled towards it
            /// @return true if enabled, false otherwise
            bool get_start_child_shrinkable() const;

            /// @brief set end child
            /// @param widget
            void set_end_child(const Widget&);

            /// @brief remove end child
            void remove_end_child();

            /// @brief set whether the end child will increase in size if the separator bar is dragged away from it
            /// @param b true if enabled, false otherwise
            void set_end_child_resizable(bool);

            /// @brief get whether the end child will increase in size if the esparator bar is dragged away from it
            /// @return true if enabled, false otherwise
            bool get_end_child_resizable() const;

            /// @brief set whether the end child will decrease in size if the separator bar is pulled towards it
            /// @param b true to enable, false otherwise
            void set_end_child_shrinkable(bool);

            /// @brief get whether the end child will decrease in size if the separator bar is pulled towards it
            /// @return true if enabled, false otherwise
            bool get_end_child_shrinkable() const;

            /// @brief set whether the separator bar is a wide bar instead of a 1-px line, on by default
            /// @param b true for wide bar, false otherwise
            void set_has_wide_handle(bool);

            /// @brief get whether the separator bar is a wide bar instead of a 1-px line
            /// @return true if wide, false otherwise
            bool get_has_wide_handle() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

        private:
            detail::PanedInternal* _internal = nullptr;
    };
}


