//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 8/18/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class FlowBox;
    namespace detail
    {
        using FlowBoxInternal = GtkFlowBox;
        DEFINE_INTERNAL_MAPPING(FlowBox);
    }
    #endif

    /// @brief box that dynamically distributes its children across multiple rows / columns
    class FlowBox :  detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(FlowBox, realize),
        HAS_SIGNAL(FlowBox, unrealize),
        HAS_SIGNAL(FlowBox, destroy),
        HAS_SIGNAL(FlowBox, hide),
        HAS_SIGNAL(FlowBox, show),
        HAS_SIGNAL(FlowBox, map),
        HAS_SIGNAL(FlowBox, unmap)
    {
        public:
            /// @brief construct
            /// @param orientation orientation, if horizontal, widgets will be arranged left to right, if vertical, widgets will be arranged top to bottom
            FlowBox(Orientation = Orientation::HORIZONTAL);

            /// @brief construct from internal
            /// @param internal
            FlowBox(detail::FlowBoxInternal*);

            /// @brief destructor
            ~FlowBox();

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
            void insert(const Widget& to_append, uint64_t index);

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
            void set_row_spacing(float);

            /// @brief get minimum spacing between two children
            /// @return spacing in pixels
            float get_row_spacing() const;

            /// @brief set minimum spacing between two children, 0 by default
            /// @param spacing in pixels
            void set_column_spacing(float);

            /// @brief get minimum spacing between two children
            /// @return spacing in pixels
            float get_column_spacing() const;

            /// @brief get number of children
            /// @return uint64_t
            uint64_t get_n_items();

            /// @brief get orientation
            /// @return orientation
            Orientation get_orientation() const;

            /// @brief set orientation
            /// @param orientation
            void set_orientation(Orientation orientation);

        private:
            detail::FlowBoxInternal* _internal = nullptr;
    };
}