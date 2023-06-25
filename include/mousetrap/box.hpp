//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Box;
    namespace detail
    {
        using BoxInternal = GtkBox;
        DEFINE_INTERNAL_MAPPING(Box);
    }
    #endif

    /// @brief container widget that arranges each of its children in series
    /// \signals
    /// \widget_signals{Box}
    class Box : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Box, realize),
        HAS_SIGNAL(Box, unrealize),
        HAS_SIGNAL(Box, destroy),
        HAS_SIGNAL(Box, hide),
        HAS_SIGNAL(Box, show),
        HAS_SIGNAL(Box, map),
        HAS_SIGNAL(Box, unmap)
    {
        public:
            /// @brief construct
            /// @param orientation orientation, if horizontal, widgets will be arranged left to right, if vertical, widgets will be arranged top to bottom
            Box(Orientation = Orientation::HORIZONTAL);

            /// @brief construct from internal
            /// @param internal
            Box(detail::BoxInternal*);

            /// @brief destructor
            ~Box();

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

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

            /// @brief get orientation
            /// @return orientation
            Orientation get_orientation() const;

            /// @brief set orientation
            /// @param orientation
            void set_orientation(Orientation orientation);

        private:
            detail::BoxInternal* _internal = nullptr;
    };
}
