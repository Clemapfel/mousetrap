//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/10/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/menu_model.hpp>
#include <mousetrap/gtk_common.hpp>
#include <mousetrap/selection_model.hpp>
#include <mousetrap/selectable.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class ColumnView;
    namespace detail
    {
        struct _ColumnViewInternal
        {
            GObject parent;

            GtkColumnView* native;
            GListStore* list_store;

            SelectionModel* selection_model;
            GtkSelectionMode selection_mode;
        };
        using ColumnViewInternal = _ColumnViewInternal;
        DEFINE_INTERNAL_MAPPING(ColumnView);

        using ColumnViewColumnInternal = GtkColumnViewColumn;
    }
    #endif

    /// @brief view that displays widget in a table, ordered by column
    /// \signals
    /// \signal_activate{ColumnView}
    /// \widget_signals{ColumnView}
    class ColumnView : public Widget,
        HAS_SIGNAL(ColumnView, activate),
        HAS_SIGNAL(ColumnView, realize),
        HAS_SIGNAL(ColumnView, unrealize),
        HAS_SIGNAL(ColumnView, destroy),
        HAS_SIGNAL(ColumnView, hide),
        HAS_SIGNAL(ColumnView, show),
        HAS_SIGNAL(ColumnView, map),
        HAS_SIGNAL(ColumnView, unmap)
    {
        public:
            class Column
            {
                friend class ColumnView;

                public:
                    /// @brief uninstatiable, use ColumnView::get_column to access individual columns
                    Column() = delete;

                    /// @brief set title of column
                    /// @param title
                    void set_title(const std::string& title);

                    /// @brief get title of column
                    /// @return title
                    std::string get_title() const;

                    /// @brief set fixed width of column
                    /// @param width
                    void set_fixed_width(float width);

                    /// @brief get fixed width of column
                    /// @return width
                    float get_fixed_width() const;

                    /// @brief set the menu model to use for the header menu, this menu is accessible by clicking the columns header bar
                    /// @param menu_model
                    void set_header_menu(const MenuModel& menu_model);

                    /// @brief set whether column should be visible
                    /// @param visible
                    void set_is_visible(bool visible);

                    /// @brief get whether column is visible
                    /// @return true if visible, false otherwise
                    bool get_is_visible() const;

                    /// @brief set whether column should be resizable
                    /// @param resizable
                    void set_is_resizable(bool resizable);

                    /// @brief get whether column is resizable
                    /// @return true if resizable, false otherwise
                    bool get_is_resizable() const;

                    /// @brief construct as thin wrapper \for_internal_use_only
                    /// @param native native GtkColumnViewColumn
                    Column(detail::ColumnViewColumnInternal*);

                    /// @brief expose internal
                    NativeObject get_internal() const;

                private:
                    GtkColumnViewColumn* _native = nullptr;
            };

        public:
            /// @brief construct
            /// @param mode selection mode, governs how many rows can be selected at one time
            ColumnView(SelectionMode mode = SelectionMode::NONE);

            /// @brief construct from internal
            ColumnView(detail::ColumnViewInternal*);

            /// @brief destructor
            ~ColumnView();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add a column to the end of the column view
            /// @param title header name for the column
            /// @returns pointer to newly created column iterator
            [[nodiscard]] Column push_back_column(const std::string& title);

            /// @brief add a colum to the start of the column view
            /// @param title header name for the column
            /// @returns pointer to newly created column iterator
            [[nodiscard]] Column push_front_column(const std::string& title);

            /// @brief insert a column at given position, inserts at the end if index is out of bounds
            /// @param index index of column to insert a new column after
            /// @param title header name for the column
            /// @returns pointer to newly created column iterator
            [[nodiscard]] Column insert_column(size_t index, const std::string& title);

            /// @brief remove a column from the column view
            /// @param column
            void remove_column(const Column& column);

            /// @brief get column at specified position
            /// @param column_i index
            /// @returns column, may be a wrapped nullptr if column_i out of bounds
            [[nodiscard]] Column get_column_at(size_t column_i);

            /// @brief get column with given title
            /// @param title current title of the column
            /// @returns column, may be a wrapped nullptr if column_i out of bounds
            [[nodiscard]] Column get_column_with_title(const std::string& title);

            /// @brief get whether at least one column has given title
            /// @param title
            /// @return true if at least once column with that title exists, false otherwise
            bool has_column_with_title(const std::string& title);

            /// @brief set at row / column. If the row does not yet exist, the table will be backfilled with empty rows until the given index is reached. If the column  does not yet exist, no action will be taken and the widget will not appear in the table
            /// @param column column
            /// @param row_i row index
            /// @param widget widget to insert
            void set_widget(const Column& column, size_t row_i, const Widget& widget);

            /// @brief convenience function that maps widgets to columns and appends them as a new row at end of the list
            /// @tparam WidgetRef_ts has to be Widget, Widget& or const Widget&
            /// @param widget_by_column number of widgets has to smaller than or equal to the number of columns
            template<typename... WidgetRef_ts>
            void push_back_row(WidgetRef_ts... widget_by_column);

            /// @brief convenience function that maps widgets to columns and appends them as a new row at end of the list
            /// @tparam WidgetRef_ts has to be Widget, Widget& or const Widget&
            /// @param widget_by_column
            template<typename... WidgetRef_ts>
            void push_front_row(WidgetRef_ts... widget_by_column);

            /// @brief convenience functino that maps widgets to columns and appends them as a new row at end of the list
            /// @tparam WidgetRef_ts has to be Widget, Widget& or const Widget&
            /// @param i row index, table is backfilled if current number of rows is less than i
            /// @param widget_by_column
            template<typename... WidgetRef_ts>
            void insert_row(size_t i, WidgetRef_ts... widget_by_column);

            /// @brief set whether the user is able to selecte multiple items by click-dragging
            /// @param b true if this type of selection should be enabled, false otherwise
            void set_enable_rubberband_selection(bool b);

            /// @brief get whether the user is able to selecte multiple items by click-dragging
            /// @return true if this type of selection should be enabled, false otherwise
            bool get_enable_rubberband_selection() const;

            /// @brief set whether rows should be separated
            /// @param b true if separators should be visible, false otherwise
            void set_show_row_separators(bool b);

            /// @brief get whether row separators are shown
            /// @return true if separators are visible, false otherwise
            bool get_show_row_separators() const;

            /// @brief set whether columns should be separated
            /// @param b true if spearators should be visible, false otherwise
            void set_show_column_separators(bool b);

            /// @brief get whether row separators are shown
            /// @return true if separators are visible, false otherwise
            bool get_show_column_separators() const;

            /// @brief set whether the column views seletion model should emit an activate signal when the user clicks on an item in the view once
            /// @param b true if signal emissions this way should be allowed, false otherwise
            void set_single_click_activate(bool b);

            /// @brief get whether the column views seletion model should emit an activate signal when the user clicks on an item in the view once
            /// @return true if signal emissions this way should be allowed, false otherwise
            bool get_single_click_activate() const;

            /// @brief set whether user can reorder columns by click-dragging
            /// @param b true if reorderable, false otherwise
            void set_is_reorderable(bool b);

            /// @brief get whether the user can reorder columns by click-dragging
            /// @return true if reorderable, false otherwise
            bool get_is_reorderable() const;

            /// @brief expose the selection model
            /// @return selection model
            SelectionModel* get_selection_model();

            /// @brief get number of rows
            /// @return size_t
            size_t get_n_rows() const;

            /// @brief get number of columns
            /// @return size_t
            size_t get_n_columns() const;

        private:
            static GtkColumnViewColumn* new_column(const std::string& title);
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, GtkColumnViewColumn*);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, GtkColumnViewColumn*);

            detail::ColumnViewInternal* _internal = nullptr;
    };
}

#include "inline/column_view.inl"
