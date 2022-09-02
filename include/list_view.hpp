// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <vector>

#include <include/widget.hpp>

namespace mousetrap
{
    namespace detail { struct _TreeListViewItem; }

    class TreeListView : public Widget
    {
        public:
            using Iterator = detail::_TreeListViewItem*;

            TreeListView(GtkSelectionMode = GTK_SELECTION_NONE);
            operator GtkWidget*() override;

            Iterator push_back(Widget* widget, Iterator = nullptr);
            Iterator push_front(Widget* widget, Iterator = nullptr);
            Iterator insert(size_t, Widget*, Iterator = nullptr);

            Iterator move_item_to(size_t old_position, size_t new_position, Iterator old_it, Iterator new_it);

            void remove(size_t, Iterator = nullptr);

            Widget* get_widget_at(size_t i, Iterator = nullptr);
            void set_widget_at(size_t i, Widget*, Iterator = nullptr);

            void set_enable_rubberband_selection(bool);
            void set_show_separators(bool);

        private:
            static void on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*);

            static GListModel* on_tree_list_model_create(void* item, void* user_data);
            static void on_tree_list_model_destroy(void* item);

            GtkSignalListItemFactory* _factory;

            GtkListView* _list_view;
            GListStore* _root;
            GtkTreeListModel* _tree_list_model;

            GtkSelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;
    };

    using ListView = TreeListView;
}

#include <src/list_view.inl>