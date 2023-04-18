//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/10/23
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>
#include <include/gtk_common.hpp>
#include <include/selection_model.hpp>
#include <include/selectable.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
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
    }
    #endif

    /// @brief view that displays widget in a table, ordered by column
    class ColumnView : public WidgetImplementation<GtkColumnView>, public Selectable,
        HAS_SIGNAL(ColumnView, activate)
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
                    void set_title(const std::string&);

                    /// @brief get title of column
                    /// @return title
                    std::string get_title() const;

                    /// @brief set fixed width of column
                    /// @param width
                    void set_fixed_width(float);

                    /// @brief get fixed width of column
                    /// @return width
                    float get_fixed_with() const;

                    /// @brief set the menumodel to use for the header menu, this menu is accessible by clicked the columns header bar
                    /// @param menu_model
                    void set_header_menu(const MenuModel&);

                    /// @brief set whether column should be visible
                    /// @param visible
                    void set_is_visible(bool);

                    /// @brief get whether column is visible
                    /// @return true if visible, false otherwise
                    bool get_is_visible() const;

                    /// @brief set whether column should be resizable
                    /// @param resizable
                    void set_is_resizable(bool);

                    /// @brief get whether column is resizable
                    /// @return true if resizable, false otherwise
                    bool get_is_resizable() const;

                protected:
                    /// @brief construct as thin wrapper \internal
                    /// @param native native GtkColumnViewColumn
                    Column(GtkColumnViewColumn*);

                private:
                    GtkColumnViewColumn* _native = nullptr;
            };

        public:
            ColumnView(SelectionMode mode = SelectionMode::NONE);

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
            void remove_column(const Column&);

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
            void set_widget(const Column&, size_t row_i, const Widget& widget);

            /// @brief convenience function that maps widgets to columns and appends them as a new row at end of the list
            /// @tparam WidgetRef_ts has to be Widget, Widget& or const Widget&
            /// @param widgets number of widgets has to smaller than or equal to the number of columns
            template<typename... WidgetRef_ts>
            void push_back_row(WidgetRef_ts... widget_by_column);

            /// @brief convenience functino that maps widgets to columns and appends them as a new row at end of the list
            /// @tparam WidgetRef_ts has to be Widget, Widget& or const Widget&
            /// @param widgets
            template<typename... WidgetRef_ts>
            void push_front_row(WidgetRef_ts... widget_by_column);

            /// @brief convenience functino that maps widgets to columns and appends them as a new row at end of the list
            /// @tparam WidgetRef_ts has to be Widget, Widget& or const Widget&
            /// @param i row index, table is backfilled if current number of rows is less than i
            /// @param widgets
            template<typename... WidgetRef_ts>
            void insert_row(size_t i, WidgetRef_ts... widget_by_column);

            /// @brief set whether the user is able to selecte multiple items by click-dragging
            /// @param b true if this type of selection should be enabled, false otherwise
            void set_enable_rubberband_selection(bool);

            /// @brief get whether the user is able to selecte multiple items by click-dragging
            /// @return true if this type of selection should be enabled, false otherwise
            bool get_enable_rubberband_selection() const;

            /// @brief set whether rows should be separated
            /// @param b true if separators should be visible, false otherwise
            void set_show_row_separators(bool);

            /// @brief get whether row separators are shown
            /// @return true if separators are visible, false otherwise
            bool get_show_row_separators() const;

            /// @brief set whether columns should be separated
            /// @param b true if spearators should be visible, false otherwise
            void set_show_column_separators(bool);

            /// @brief get whether row separators are shown
            /// @return true if separators are visible, false otherwise
            bool get_show_column_separators() const;

            /// @brief set whether the column views seletion model should emit an activate signal when the user clicks on an item in the view once
            /// @param b true if signal emissions this way should be allowed, false otherwise
            void set_single_click_activate(bool);

            /// @brief get whether the column views seletion model should emit an activate signal when the user clicks on an item in the view once
            /// @param b true if signal emissions this way should be allowed, false otherwise
            bool get_single_click_activate() const;

            /// @brief set whether user can reorder columns by click-dragging
            /// @param b true if reorderable, false otherwise
            void set_is_reorderable(bool);

            /// @brief get whether the user can reorder columns by click-dragging
            /// @return true if reorderable, false otherwise
            bool get_is_reorderable() const;

            /// @brief expose the selection model
            /// @return selection model
            SelectionModel* get_selection_model() override;

            /// @brief get number of rows
            /// @return size_t
            size_t get_n_rows() const;

            /// @brief get number of columns
            /// qreturn size_t
            size_t get_n_columns() const;

        private:
            static GtkColumnViewColumn* new_column(const std::string& title);
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, GtkColumnViewColumn*);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, GtkColumnViewColumn*);

            detail::ColumnViewInternal* _internal = nullptr;
    };
}

#include <src/column_view.inl>
