//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/24/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>
#include <mousetrap/relative_position.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Grid;
    namespace detail
    {
        using GridInternal = GtkGrid;
        DEFINE_INTERNAL_MAPPING(Grid);
    }
    #endif

    /// @brief displays widgets in a grid, one widget may take up more than one cell
    /// \signals
    /// \widget_signals{Grid}
    class Grid : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Grid, realize),
        HAS_SIGNAL(Grid, unrealize),
        HAS_SIGNAL(Grid, destroy),
        HAS_SIGNAL(Grid, hide),
        HAS_SIGNAL(Grid, show),
        HAS_SIGNAL(Grid, map),
        HAS_SIGNAL(Grid, unmap)
    {
        public:
            /// @brief construct
            Grid();

            /// @brief construct from internal
            Grid(detail::GridInternal*);

            /// @brief destructor
            ~Grid();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief insert widget a position
            /// @param widget
            /// @param row_column_index row and column index, may be negative
            /// @param n_horizontal_cells how many horizontal cells should the widget occupy
            /// @param n_vertical_cells how many vertical cells should the widget occupy
            void insert(const Widget& widget, Vector2i row_column_index, uint64_t n_horizontal_cells = 1, uint64_t n_vertical_cells = 1);

            /// @brief insert widget such that it is next to a widget already in the grid
            /// @param to_insert new widget
            /// @param already_in_grid already inserted widget
            /// @param relative_position
            /// @param n_horizontal_cells how many horizontal cells should the widget occupy
            /// @param n_vertical_cells how many vertical cells should the widget occupy
            void insert_next_to(const Widget& to_insert, const Widget& already_in_grid, RelativePosition, uint64_t n_horizontal_cells = 1, uint64_t n_vertical_cells = 1);

            /// @brief remove a widget from the grid
            /// @param widget
            void remove(const Widget& widget);

            /// @brief get the row and column position of a widget
            /// @param widget
            /// @return row index, column index
            Vector2i get_position(const Widget& widget) const;

            /// @brief get how many cells a widget should occupy
            /// @param widget
            /// @return number of rows, number of columns
            Vector2ui get_size(const Widget& widget) const;

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
            void set_column_spacing(float spacing);

            /// @brief get spacing between columns
            /// @return spacing in pixels
            float get_column_spacing() const;

            /// @brief set spacing between rows
            /// @param spacing in pixels
            void set_row_spacing(float spacing);

            /// @brief get spacing between rows
            /// @return spacing in pixels
            float get_row_spacing() const;

            /// @brief set whether all rows should allocate the same size
            /// @param b true if allocation should be homogenous, false otherwise
            void set_rows_homogeneous(bool b);

            /// @brief get whether all rows should allocate the same size
            /// @return true if allocation should be homogenous, false otherwise
            bool get_rows_homogeneous() const;

            /// @brief set whether all columns should allocate the same size
            /// @param b true if allocation should be homogenous, false otherwise
            void set_columns_homogeneous(bool b);

            /// @brief get whether all columns should allocate the same size
            /// @return true if allocation should be homogenous, false otherwise
            bool get_columns_homogeneous() const;

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

        private:
            detail::GridInternal* _internal = nullptr;
    };
}
