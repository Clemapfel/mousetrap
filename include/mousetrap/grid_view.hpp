//
// Copyright 2022 Clemens Cords
// Created on 9/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>
#include <mousetrap/selection_model.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class GridView;
    namespace detail
    {
        struct _GridViewInternal;
        using GridViewInternal = _GridViewInternal;
        DEFINE_INTERNAL_MAPPING(GridView);
    }
    #endif

    /// @brief container, arranges widget in a grid
    /// \signals
    /// \signal_activate{GridView}
    /// \widget_signals{GridView}
    class GridView : public Widget,
        HAS_SIGNAL(GridView, activate),
        HAS_SIGNAL(GridView, realize),
        HAS_SIGNAL(GridView, unrealize),
        HAS_SIGNAL(GridView, destroy),
        HAS_SIGNAL(GridView, hide),
        HAS_SIGNAL(GridView, show),
        HAS_SIGNAL(GridView, map),
        HAS_SIGNAL(GridView, unmap)
    {
        public:
            /// @brief construct
            /// @param orientation if horizontal, widgets will be arranged left to right, if vertical, widgets will be arranged top to bottom
            /// @param selection_mode
            GridView(Orientation orientation = Orientation::VERTICAL, SelectionMode selection_mode = SelectionMode::NONE);

            /// @brief construct from internal
            GridView(detail::GridViewInternal*);

            /// @brief destructor
            ~GridView();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add widget to the left if orientation is horizontal, to the top if vertical
            /// @param widget
            void push_front(const Widget& widget);

            /// @brief add widget to the right if orientation is horizontal, to the bottom if vertical
            /// @param widget
            void push_back(const Widget& widget);

            /// @brief insert widget at index
            /// @param widget
            /// @param index
            void insert(const Widget& widget, size_t index);

            /// @brief remove all widgets
            void clear();

            /// @brief remove widget
            /// @param widget
            void remove(const Widget& widget);

            /// @brief get number of widgets
            /// @return n
            size_t get_n_items() const;

            /// @brief enable user interaction where a user can click-drag across the mousetrap::GridView to select multiple items
            /// @param b true to enable, false otherwise
            void set_enable_rubberband_selection(bool b);

            /// @brief get whether user interaction where a user can click-drag across the mousetrap::GridView to select multiple items is enabled
            /// @return true if enabled, false otherwise
            bool get_enable_rubberband_selection() const;

            /// @brief set maximum number of columns
            /// @param n
            void set_max_n_columns(size_t n);

            /// @brief get maximum number of columns
            /// @return n
            size_t get_max_n_columns() const;

            /// @brief set minimum number of columns
            /// @param n
            void set_min_n_columns(size_t n);

            /// @brief get minimum number of columns
            /// @return n
            size_t get_min_n_columns() const;

            /// @brief get selection model
            /// @return pointer to selection model
            SelectionModel* get_selection_model();

            /// @brief set whether an <tt>activated</tt> signal should be emitted when the user selects the item, as opposed to selecting and activating the item
            /// @param enable true to enable, false otherwise
            void set_single_click_activate(bool enable);

            /// @brief get whether an <tt>activated</tt> signal is be emitted when the user selects the item, as opposed to selecting and activating the item
            /// @return true if enable, false otherwise
            bool get_single_click_activate() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation orientation);

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

        private:
           detail::GridViewInternal* _internal = nullptr;
    };
}
