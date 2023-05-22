//
// Created by clem on 3/24/23.
//

#include <mousetrap/grid.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap
{
    Grid::Grid()
        : Widget(gtk_grid_new())
    {
        _internal = GTK_GRID(Widget::operator NativeWidget());
    }
    
    Grid::Grid(detail::GridInternal* internal)
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }
    
    Grid::~Grid() 
    {
        g_object_unref(_internal);
    }

    NativeObject Grid::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Grid::insert(const Widget& widget, Vector2i row_column_index, size_t n_horizontal_cells, size_t n_vertical_cells)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Grid::insert, this, ptr);
        gtk_grid_attach(GTK_GRID(_internal), widget.operator GtkWidget*(), row_column_index.x, row_column_index.y, n_horizontal_cells, n_vertical_cells);
    }

    void Grid::remove(const Widget& widget)
    {
        gtk_grid_remove(GTK_GRID(_internal), widget.operator GtkWidget*());
    }

    Vector2i Grid::get_position(const Widget& widget) const
    {
        int column, row, width, height;
        gtk_grid_query_child(GTK_GRID(_internal), widget.operator GtkWidget *(), &column, &row, &width, &height);
        return Vector2i(column, row);
    }

    Vector2ui Grid::get_bounds(const Widget& widget) const
    {
        int column, row, width, height;
        gtk_grid_query_child(GTK_GRID(_internal), widget.operator GtkWidget *(), &column, &row, &width, &height);
        return Vector2ui(width, height);
    }

    void Grid::insert_row_at(int index)
    {
        gtk_grid_insert_row(GTK_GRID(_internal), index);
    }

    void Grid::remove_row_at(int index)
    {
        gtk_grid_remove_row(GTK_GRID(_internal), index);
    }

    void Grid::insert_column_at(int index)
    {
        gtk_grid_insert_column(GTK_GRID(_internal), index);
    }

    void Grid::remove_column_at(int index)
    {
        gtk_grid_remove_column(GTK_GRID(_internal), index);
    }

    void Grid::set_column_spacing(float v)
    {
        gtk_grid_set_column_spacing(GTK_GRID(_internal), v);
    }

    float Grid::get_column_spacing() const
    {
        return gtk_grid_get_column_homogeneous(GTK_GRID(_internal));
    }

    void Grid::set_row_spacing(float v)
    {
        gtk_grid_set_row_spacing(GTK_GRID(_internal), v);
    }

    float Grid::get_row_spacing() const
    {
        return gtk_grid_get_row_spacing(GTK_GRID(_internal));
    }

    void Grid::set_rows_homogenous(bool b)
    {
        gtk_grid_set_row_homogeneous(GTK_GRID(_internal), b);
    }

    bool Grid::get_rows_homogenous() const
    {
        return gtk_grid_get_row_homogeneous(GTK_GRID(_internal));
    }

    void Grid::set_columns_homogenous(bool b)
    {
        gtk_grid_set_column_homogeneous(GTK_GRID(_internal), b);
    }

    bool Grid::get_columns_homogenous() const
    {
        return gtk_grid_get_column_homogeneous(GTK_GRID(_internal));
    }

    void Grid::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(GTK_GRID(_internal)), (GtkOrientation) orientation);
    }

    Orientation Grid::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(GTK_GRID(_internal)));
    }
}
