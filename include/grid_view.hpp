//
// Copyright 2022 Clemens Cords
// Created on 9/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/selectable.hpp>
#include <include/orientable.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _GridViewInternal;
        using GridViewInternal = _GridViewInternal;
    }
    #endif

    /// @brief container, arranges widget in a grid
    class GridView : public WidgetImplementation<GtkGridView>, public Orientable, public Selectable,
        HAS_SIGNAL(GridView, activate)
    {
        public:
            /// @brief construct
            /// @param orientation if horizontal, widgets will be arranged left to right, if vertical, widgets will be arranged top to bottom
            /// @param selection_mode
            GridView(Orientation = Orientation::VERTICAL, SelectionMode = SelectionMode::NONE);

            /// @brief add widget to the left if orientation is horizontal, to the top if vertical
            /// @param widget
            void push_front(const Widget&);

            /// @brief add widget to the right if orientation is horizontal, to the bottom if vertical
            /// @param widget
            void push_back(const Widget&);

            /// @brief insert widget at index
            /// @param widget
            /// @param index
            void insert(const Widget&, size_t);

            /// @brief remove all widgets
            void clear();

            /// @brief remove widget
            /// @param widget
            void remove(const Widget&);

            /// @brief get number of widgets
            /// @return n
            size_t get_n_items() const;

            /// @brief enable user interaction where a user can click-drag across the mousetrap::GridView to select multiple items
            /// @param enabled true to enable, false otherwise
            void set_enable_rubberband_selection(bool);

            /// @brief get whether user interaction where a user can click-drag across the mousetrap::GridView to select multiple items is enabled
            /// @return true if enabled, false otherwise
            bool get_enable_rubberband_selection() const;

            /// @brief set maximum number of columns
            /// @param n
            void set_max_n_columns(size_t);

            /// @brief get maximum number of columns
            /// @return n
            size_t get_max_n_columns() const;

            /// @brief set minimum number of columns
            /// @param n
            void set_min_n_columns(size_t);

            /// @brief get minimum number of columns
            /// @return n
            size_t get_min_n_columns() const;

            /// @brief get selection model
            /// @return pointer to selection model
            SelectionModel* get_selection_model() override;

            /// @brief set whether an <tt>activated</tt> signal should be emitted when the user selects the item, as opposed to selecting and activating the item
            /// @param enable true to enable, false otherwise
            void set_single_click_activate(bool);

            /// @brief get whether an <tt>activated</tt> signal is be emitted when the user selects the item, as opposed to selecting and activating the item
            /// @return true if enable, false otherwise
            bool get_single_click_activate() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;

            /// @copydoc mousetrap::Orientation::get_orientation
            Orientation get_orientation() const override;

        private:
           detail::GridViewInternal* _internal = nullptr;
    };
}
