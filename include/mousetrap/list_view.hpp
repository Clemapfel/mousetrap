//
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <vector>

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>
#include <mousetrap/selection_model.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class ListView;
    namespace detail
    {
        struct _ListViewItem;
        struct _ListViewInternal;
        using ListViewInternal = _ListViewInternal;
        DEFINE_INTERNAL_MAPPING(ListView);
    }
    #endif

    /// @brief Container widget that displays a number of items in a single row, nested lists are possible
    /// \signals
    /// \signal_activate{ListView}
    /// \widget_signals{ListView}
    class ListView : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(ListView, activate),
        HAS_SIGNAL(ListView, realize),
        HAS_SIGNAL(ListView, unrealize),
        HAS_SIGNAL(ListView, destroy),
        HAS_SIGNAL(ListView, hide),
        HAS_SIGNAL(ListView, show),
        HAS_SIGNAL(ListView, map),
        HAS_SIGNAL(ListView, unmap)
    {
        public:
            /// @brief iterator to a list, specifying it as the target during an operationt that adds an item to a list view will create a sublist at that iterator
            using Iterator = detail::_ListViewItem*;

            /// @brief number of pixels a nested list item should be offset to the right
            static inline const size_t indent_per_depth = 20;

            /// @brief ctor list view
            /// @param orientation Orientation, horizontal for left to right, vertical for top to bottom
            /// @param selection_mode
            ListView(Orientation orientation = Orientation::HORIZONTAL, SelectionMode selection_mode = SelectionMode::NONE);

            /// @brief construct from internal
            ListView(detail::ListViewInternal*);

            /// @brief dtor
            ~ListView();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add widget to the back of the list
            /// @param widget
            /// @param iterator iterator to sub-list, or nullptr to add to the toplevel list
            /// @return iterator to list the widget was inserted in
            Iterator push_back(const Widget& widget, Iterator iterator = nullptr);

            /// @brief add a widget to the front of the list
            /// @param widget
            /// @param iterator iterator to sub-list, or nullptr to add to the toplevel list
            /// @return iterator to list the widget was inserted in
            Iterator push_front(const Widget& widget, Iterator iterator = nullptr);

            /// @brief add a widget at a specific position to the list
            /// @param widget
            /// @param index
            /// @param iterator iterator to sub-list, or nullptr to add to the toplevel list
            /// @return iterator to list the widget was inserted in
            Iterator insert(const Widget& widget, size_t index, Iterator iterator = nullptr);

            /// @brief remove n-th element from list specified by iterator
            /// @param index
            /// @param iterator iterator to list to remove from, or nullptr to remove from toplevel list
            void remove(size_t index, Iterator iterator = nullptr);

            /// @brief clear list at iterator
            /// @param iterator iterator to list to clear, or nullptr to clear the toplevel list
            void clear(Iterator iterator = nullptr);

            /// @brief get widget at i-th position in list
            /// @param i index
            /// @param iterator iterator to list, or nullptr to access the toplevel list
            /// @return pointer to widget at that position, or nullptr if unable to retrieve widget
            Widget* get_widget_at(size_t i, Iterator iterator = nullptr);

            /// @brief set widget at i-th position in list
            /// @param i index
            /// @param widget
            /// @param iterator iterator to list, or nullptr to set widget in toplevel list
            void set_widget_at(size_t i, const Widget& widget, Iterator iterator = nullptr);

            /// @brief enable users to select multiple elements by click-dragging
            /// @param b true if enabled, false otherwise
            void set_enable_rubberband_selection(bool b);

            /// @brief get whether users can select multiple elements by click-dragging
            /// @return true if enabled, false otherwise
            bool get_enable_rubberband_selection() const;

            /// @brief set whether separators should be shown in between each item, this applies to the toplevel and all sub-lists
            /// @param b true if separators should be shown, false oherwise
            void set_show_separators(bool b);

            /// @brief get whether separator should be shown in between each item
            /// @return true if separators should be shown, false otherwise
            bool get_show_separators() const;

            /// @brief set whether the <tt>activate</tt> signal should be emitted when the user selects an item, as opposed to selected and activating it
            /// @param b true if emission should already happen on selection, false otherwise
            void set_single_click_activate(bool b);

            /// @brief get whether the <tt>activate</tt> signal should be emitted when the user selects an item, as opposed to selected and activating it
            /// @return true if emission already happens on selection, false otherwise
            bool get_single_click_activate() const;

            /// @brief expose the selection model, connect to its signals to monitor list item selection
            /// @return point to selection model, the instance is owned by the list view
            SelectionModel* get_selection_model();

            /// @brief get number of itmes in list and all sub lists
            /// @return n items
            size_t get_n_items() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

        private:
            detail::ListViewInternal* _internal = nullptr;
    };

    using TreeListView = ListView;
}
