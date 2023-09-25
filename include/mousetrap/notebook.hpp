//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/relative_position.hpp>
#include <mousetrap/selection_model.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.hpp"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Notebook;
    namespace detail
    {
        struct _NotebookInternal
        {
            GObject parent;
            GtkNotebook* native;
            bool popup_enabled = false;
            bool tabs_reorderable = false;
        };
        using NotebookInternal = _NotebookInternal;
        DEFINE_INTERNAL_MAPPING(Notebook);
    }
    #endif

    /// @brief container widget that displays multiple pages the cuser can switch between. Each page as a "tab" with a label
    /// \signals
    /// \signal_page_added{Notebook}
    /// \signal_page_reordered{Notebook}
    /// \signal_page_removed{Notebook}
    /// \signal_page_selection_changed{Notebook}
    /// \widget_signals{Notebook}
    class Notebook : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Notebook, page_added),
        HAS_SIGNAL(Notebook, page_reordered),
        HAS_SIGNAL(Notebook, page_removed),
        HAS_SIGNAL(Notebook, page_selection_changed),
        HAS_SIGNAL(Notebook, realize),
        HAS_SIGNAL(Notebook, unrealize),
        HAS_SIGNAL(Notebook, destroy),
        HAS_SIGNAL(Notebook, hide),
        HAS_SIGNAL(Notebook, show),
        HAS_SIGNAL(Notebook, map),
        HAS_SIGNAL(Notebook, unmap)
    {
        public:
            /// @brief construct empty
            Notebook();

            /// @brief construct from internal
            Notebook(detail::NotebookInternal*);

            /// @brief destructor
            ~Notebook();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add a page to start of the notebook
            /// @param child_widget widget to display as the pages content, may be nullptr
            /// @param label_widget widget to use for the lable in the pages tab, may be nullptr
            /// @return index of the newly inserted page
            uint64_t push_front(const Widget& child_widget, const Widget& label_widget);

            /// @brief add a page to end of the notebook
            /// @param child_widget widget to display as the pages content, may be nullptr
            /// @param label_widget widget to use for the lable in the pages tab, may be nullptr
            /// @return index of the newly inserted page
            uint64_t push_back(const Widget& child_widget, const Widget& label_widget);

            /// @brief add a page at a specific position in the notebook
            /// @param new_position
            /// @param child_widget widget to display as the pages content, may be nullptr
            /// @param label_widget widget to use for the lable in the pages tab, may be nullptr
            /// @return index of the newly inserted page
            uint64_t insert(uint64_t new_position, const Widget& child_widget, const Widget& label_widget);

            /// @brief move page that has child as widget to given position
            /// @param child
            /// @param new position
            void move_page_to(uint64_t current_position, uint64_t new_position);

            /// @brief remove page at given position, removes last page if position is out of bounds
            /// @param position
            void remove(uint64_t position);

            /// @brief make active the page after the current one, does nothing if current page is the last
            void next_page();

            /// @brief switch to page with the given index, or the last page if index is out of bounds
            /// @param index
            void goto_page(uint64_t index);

            /// @brief make active the page before the current one, does nothing if current page is the first
            void previous_page();

            /// @brief return index of current page
            /// @return index of current page, -1 if no page exists
            int64_t get_current_page() const;

            /// @brief get number of pages
            /// @return uint64_t
            uint64_t get_n_pages() const;

            /// @brief set pages can be changed with a scroll motion
            /// @param b true if scroll motion allowed, false otherwise
            void set_is_scrollable(bool);

            /// @brief get whether pages can be changed with a scroll motion
            /// @return true if scroll motion allowed, false otherwise
            bool get_is_scrollable() const;

            /// @brief set whether notebook will have a black border
            /// @param b true if black border visible, false otherwise
            void set_has_border(bool b);

            /// @brief get whether notebook is displaying a black border
            /// @return true if black border visible, false otherwise
            bool get_has_border() const;

            /// @brief set whether the tabs of pages should be displayed
            /// @param b true if tabs should be displayed, false otherwise
            void set_tabs_visible(bool b);

            /// @brief get whether tabs are being display
            /// @return true if tabs are visible, false otherwise
            bool get_tabs_visible() const;

            /// @brief set whether a menu dropdown that allows users to navigate to a specific page should be accessible
            /// @param b true if it should be enabled, false otherwise
            void set_quick_change_menu_enabled(bool b);

            /// @brief get whether a menu dropdown that allows users to navigate to a speicifc page is accessible
            /// @return true if enabled, false otherwise
            bool get_quick_change_menu_enabled() const;

            /// @brief set relative position of the tabs
            /// @param position relative position
            void set_tab_position(RelativePosition position);

            /// @brief get relative position of the tabs
            /// @return relative position
            RelativePosition get_tab_position() const;

            /// @brief set whether the user can reorder pages in the notebook by click-dragging the tabs
            /// @param b true if enabled, false otherwise
            void set_tabs_reorderable(bool b);

            /// @brief get whether the user can reorder pages in the notebook by click-dragging the tabs
            /// @return true if enabled, false otherwise
            bool get_tabs_reorderable() const;

        private:
            detail::NotebookInternal* _internal = nullptr;
    };
}