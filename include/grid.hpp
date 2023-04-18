//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/24/23
//

#pragma once

#include <include/widget.hpp>
#include <include/orientable.hpp>

namespace mousetrap
{
    /// @brief displays widgets in a grid, one widget may take up more than one cell
    class Grid : public WidgetImplementation<GtkGrid>, public Orientable
    {
        public:
            /// @brief construct
            Grid();

            /// @brief insert widget a position
            /// @param widget
            /// @param row_column row and column index, may be negative
            /// @param n_horizontal_cells how many horizontal cells should the widget occupy
            /// @param n_vertical_cells how many vertical cells should the widget occupy
            void insert(const Widget& widget, Vector2i row_column_index, size_t n_horizontal_cells, size_t n_vertical_cells);

            /// @brief remove a widget from the grid
            /// @param widget
            void remove(const Widget&);

            /// @brief get the row and column position of a widget
            /// @param widget
            /// @return row index, column index
            Vector2i get_position(const Widget&) const;

            /// @brief get how many cells a widget should occupy
            /// @param widget
            /// @return number of rows, number of columns
            Vector2ui get_bounds(const Widget&) const;

            /// @brief insert a new row at given index
            /// @param index may be negative
            void insert_row_at(int index);

            /// @brief remove row at given index
            /// @param index may be negative
            void remove_row_at(int index);

            /// @brief insert column at given index
            /// @param index may be negative
            void insert_column_at(int index);

            /// @brief remove column at given index
            /// @param index may be negative
            void remove_column_at(int index);

            /// @brief set spacing between columns
            /// @param spacing in pixels
            void set_column_spacing(float);

            /// @brief get spacing between columns
            /// @return spacing in pixels
            float get_column_spacing() const;

            /// @brief set spacing between rows
            /// @param spacing in pixels
            void set_row_spacing(float);

            /// @brief get spacing between rows
            /// @return spacing in pixels
            float get_row_spacing() const;

            /// @brief set whether all rows should allocate the same size
            /// @param b true if allocation should be homogenous, false otherwise
            void set_rows_homogenous(bool);

            /// @brief get whether all rows should allocate the same size
            /// @return true if allocation should be homogenous, false otherwise
            bool get_rows_homogenous() const;

            /// @brief set whether all columns should allocate the same size
            /// @param b true if allocation should be homogenous, false otherwise
            void set_columns_homogenous(bool);

            /// @brief get whether all columns should allocate the same size
            /// @return true if allocation should be homogenous, false otherwise
            bool get_columns_homogenous() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            Orientation get_orientation() const override;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;
    };
}
