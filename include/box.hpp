//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/orientable.hpp>

namespace mousetrap
{
    /// @brief container widget that arranges each of its children in series
    class Box : public WidgetImplementation<GtkBox>, public Orientable
    {
        public:
            /// @brief construct
            /// @param orientation orientation, if horizontal, widgets will be arranged left to right, if vertical, widgets will be arranged top to bottom
            Box(Orientation);

            /// @brief add a widget to the end of the box
            /// @param widget
            void push_back(const Widget&);

            /// @brief add a widget to the start of the box
            /// @param widget
            void push_front(const Widget&);

            /// @brief insert a widget after another widget already in the box
            /// @param to_append widget not currently in box
            /// @param after widget already in box, or nullptr to insert widget at the start of the box
            void insert_after(const Widget& to_append, const Widget& after);

            /// @brief remove a widget from the box
            /// @param widget widget already in box, if widget is not contained in box, does nothing
            void remove(const Widget&);

            /// @brief remove all widgets from the box
            void clear();

            /// @brief set whether the box should attempt to allocate the same size to all of its children
            /// @param b
            void set_homogeneous(bool);

            /// @brief get whether the box should attempt to allocate the same size to all of its children
            /// @return boolean
            bool get_homogeneous() const;

            /// @brief set minimum spacing between two children, 0 by default
            /// @param spacing in pixels
            void set_spacing(float);

            /// @brief get minimum spacing between two children
            /// @return spacing in pixels
            float get_spacing() const;

            /// @brief get number of children
            /// @return size_t
            size_t get_n_items();

            /// @copydoc mousetrap::Orientable::set_orientation
            Orientation get_orientation() const override;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;
    };
}
