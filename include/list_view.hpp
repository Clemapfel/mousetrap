// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <vector>

#include <include/widget.hpp>
#include <include/selection_model.hpp>

namespace mousetrap
{
    namespace detail { struct _TreeListViewItem; }

    class TreeListView : public WidgetImplementation<GtkListView>,
        public HasListItemActivateSignal<TreeListView>
    {
        public:
            using Iterator = detail::_TreeListViewItem*;

            TreeListView(GtkOrientation = GTK_ORIENTATION_VERTICAL, GtkSelectionMode = GTK_SELECTION_NONE);
            TreeListView(GtkSelectionMode);

            Iterator push_back(Widget* widget, Iterator = nullptr);
            Iterator push_front(Widget* widget, Iterator = nullptr);
            Iterator insert(size_t, Widget*, Iterator = nullptr);

            Iterator move_item_to(size_t old_position, size_t new_position, Iterator old_it, Iterator new_it);

            void remove(size_t, Iterator = nullptr);

            Widget* get_widget_at(size_t i, Iterator = nullptr);
            void set_widget_at(size_t i, Widget*, Iterator = nullptr);

            void set_enable_rubberband_selection(bool);
            void set_show_separators(bool);
            void set_single_click_activate(bool);

            SelectionModel* get_selection_model();

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

            SelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;
            GtkOrientation _orientation;
    };

    using ListView = TreeListView;
}

#include <src/list_view.inl>