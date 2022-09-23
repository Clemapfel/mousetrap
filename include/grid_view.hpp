// 
// Copyright 2022 Clemens Cords
// Created on 9/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class GridView : public WidgetImplementation<GtkGridView>
    {
        public:
            GridView(GtkOrientation = GTK_ORIENTATION_VERTICAL, GtkSelectionMode = GTK_SELECTION_NONE);
            GridView(GtkSelectionMode);

            void push_front(Widget*);
            void push_back(Widget*);
            void insert(Widget*, size_t);

            void remove(Widget*);
            size_t get_n_items() const;

            void set_enable_rubberband_selection(bool);
            void set_max_columns(size_t);
            void set_min_columns(size_t);

        private:
            static void on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, GridView* instance);
            static void on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, GridView* instance);
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, GridView* instance);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, GridView* instance);

            GtkGridView* _native;
            GtkSignalListItemFactory* _factory;
            GListStore* _list_store;

            GtkSelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;
            GtkOrientation _orientation;
    };
}

#include <src/grid_view.inl>
