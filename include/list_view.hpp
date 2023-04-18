//
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <vector>

#include <include/widget.hpp>
#include <include/orientable.hpp>
#include <include/selectable.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _ListViewItem;
        struct _ListViewInternal;
        using ListViewInternal = _ListViewInternal;
    }
    #endif

    /// @brief Container widget that displays a number of items in a single row, nested lists are possible
    class ListView : public WidgetImplementation<GtkListView>, public Orientable, public Selectable,
        HAS_SIGNAL(ListView, activate)
    {
        public:
            /// @brief iterator to a list, specifying it as the target during an operationt that adds an item to a list view will create a sublist at that iterator
            using Iterator = detail::_ListViewItem*;

            /// @brief ctor list view
            /// @param orientation Orientation, horizontal for left to right, vertical for top to bottom
            /// @param selection_mode
            ListView(Orientation = Orientation::HORIZONTAL, SelectionMode = SelectionMode::NONE);

            /// @brief dtor
            ~ListView();

            /// @brief add widget to the back of the list
            /// @param widget
            /// @param iterator iterator to sub-list, or nullptr to add to the toplevel list
            /// @return iterator to list the widget was inserted in
            Iterator push_back(const Widget& widget, Iterator = nullptr);

            /// @brief add a widget to the front of the list
            /// @param widget
            /// @param iterator iterator to sub-list, or nullptr to add to the toplevel list
            /// @return iterator to list the widget was inserted in
            Iterator push_front(const Widget& widget, Iterator = nullptr);

            /// @brief add a widget at a specific position to the list
            /// @param widget
            /// @param iterator iterator to sub-list, or nullptr to add to the toplevel list
            /// @return iterator to list the widget was inserted in
            Iterator insert(size_t, const Widget&, Iterator = nullptr);

            /// @brief remove n-th element from list specified by iterator
            /// @param index
            /// @param iterator iterator to list to remove from, or nullptr to remove from toplevel list
            void remove(size_t, Iterator = nullptr);

            /// @brief clear list at iterator
            /// @param iterator iterator to list to clear, or nullptr to clear the toplevel list
            void clear(Iterator = nullptr);

            /// @brief get widget at i-th position in list
            /// @param i index
            /// @param iterator iterator to list, or nullptr to access the toplevel list
            /// @return pointer to widget at that position, or nullptr if unable to retrieve widget
            Widget* get_widget_at(size_t i, Iterator = nullptr);

            /// @brief set widget at i-th position in list
            /// @param i index
            /// @param widget
            /// @param iterator iterator to list, or nullptr to set widget in toplevel list
            void set_widget_at(size_t i, const Widget&, Iterator = nullptr);

            /// @brief enable users to select multiple elements by click-dragging
            /// @param b true if enabled, false otherwise
            void set_enable_rubberband_selection(bool);

            /// @brief get whether users can select multiple elements by click-dragging
            /// @return true if enabled, false otherwise
            bool get_enable_rubberband_selection() const;

            /// @brief set whether separators should be shown in between each item, this applies to the toplevel and all sub-lists
            /// @param b true if separators should be shown, false oherwise
            void set_show_separators(bool);

            /// @brief get whether separator should be shown in between each item
            /// @return true if separators should be shown, false otherwise
            bool get_show_separators() const;

            /// @brief set whether the <tt>activate</tt> signal should be emitted when the user selects an item, as opposed to selected and activating it
            /// @param b true if emission should already happen on selection, false otherwise
            void set_single_click_activate(bool);

            /// @brief get whether the <tt>activate</tt> signal should be emitted when the user selects an item, as opposed to selected and activating it
            /// @param true if emission already happens on selection, false otherwise
            bool get_single_click_activate() const;

            /// @brief expose the selection model, connect to its signals to monitor list item selection
            /// @return point to selection model, the instance is owned by the list view
            SelectionModel* get_selection_model() override;

            /// @brief get number of itmes in list and all sub lists
            /// @return n items
            size_t get_n_items() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const override;

        private:
            detail::ListViewInternal* _internal = nullptr;
    };

    using TreeListView = ListView;
}
