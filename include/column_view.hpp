//
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

#include <include/widget.hpp>

namespace mousetrap
{
    class ColumnViewColumn;

    class ColumnView : public Widget
    {
        public:
            ColumnView(GtkSelectionModel* selection_model = nullptr);

        private:
            GtkColumnView* _native;
    };

}

// #include <src/column_view.inl>

namespace mousetrap
{
    ColumnView::ColumnView(GtkSelectionModel* selection_model)
    {
        _native = GTK_COLUMN_VIEW(gtk_column_view_new(selection_model));
    }
}