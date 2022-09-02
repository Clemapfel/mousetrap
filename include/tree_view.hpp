// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <vector>

#include <include/widget.hpp>

namespace mousetrap::detail
{
    #define G_TYPE_LIST_VIEW_ITEM_ITEM (tree_view_item_get_type())

    G_DECLARE_FINAL_TYPE (TreeViewItem, tree_view_item, G, TREE_VIEW_ITEM, GObject)

    struct _TreeViewItem
    {
        GObject parent_instance;

        GtkTreeExpander* expander;
        Widget* widget;

        GListStore* children;
    };

    struct _TreeViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (TreeViewItem, tree_view_item, G_TYPE_OBJECT)

    static void tree_view_item_init(TreeViewItem* item)
    {
        item->expander = GTK_TREE_EXPANDER(gtk_tree_expander_new());
        item->widget = nullptr;
        item->children = g_list_store_new(G_TYPE_OBJECT);
    }

    static void tree_view_item_class_init(TreeViewItemClass*) {}

    static TreeViewItem* tree_view_item_new(Widget* in)
    {
        auto* item = (TreeViewItem*) g_object_new(G_TYPE_LIST_VIEW_ITEM_ITEM, nullptr);
        tree_view_item_init(item);
        item->widget = in;
        return item;
    }
}

namespace mousetrap
{

    /// \note wrapper for GtkList with GtkTreeListModel, **not** GtkTreeView
    class TreeView : public Widget
    {
        public:
            using Iterator = detail::_TreeViewItem*;

            TreeView(GtkSelectionMode = GTK_SELECTION_NONE);
            operator GtkWidget*() override;

            Iterator push_back(Widget* widget, Iterator it = nullptr);

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
            GtkSelectionMode _mode;
    };
}

namespace mousetrap
{
    TreeView::TreeView(GtkSelectionMode mode)
    {
        _root = g_list_store_new(G_TYPE_OBJECT);
        _tree_list_model = gtk_tree_list_model_new(G_LIST_MODEL(_root), false, true, on_tree_list_model_create, nullptr, on_tree_list_model_destroy);

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_tree_list_model)));

        _factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_factory, "bind", G_CALLBACK(on_list_item_factory_bind), nullptr);
        g_signal_connect(_factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), nullptr);
        g_signal_connect(_factory, "setup", G_CALLBACK(on_list_item_factory_setup), nullptr);
        g_signal_connect(_factory, "teardown", G_CALLBACK(on_list_item_factory_teardown), nullptr);

        _list_view = GTK_LIST_VIEW(gtk_list_view_new(_selection_model, GTK_LIST_ITEM_FACTORY(_factory)));
    }

    TreeView::operator GtkWidget*()
    {
        return GTK_WIDGET(_list_view);
    }

    GListModel* TreeView::on_tree_list_model_create(void* item, void* user_data)
    {
        auto* tree_view_item = (detail::TreeViewItem*) item;
        std::cout << g_list_model_get_n_items(G_LIST_MODEL(tree_view_item->children)) << std::endl;

        if (g_list_model_get_n_items(G_LIST_MODEL(tree_view_item->children)) == 0)
            return nullptr;
        else
            return G_LIST_MODEL(tree_view_item->children);
    }

    void TreeView::on_tree_list_model_destroy(void* item)
    {}

    void TreeView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void*)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
        detail::TreeViewItem* tree_view_item = (detail::TreeViewItem*) gtk_tree_list_row_get_item(tree_list_row);
        std::cout << g_list_model_get_n_items(G_LIST_MODEL(tree_view_item->children)) << std::endl;

        if (g_list_model_get_n_items(G_LIST_MODEL(tree_view_item->children)) != 0) // non-leaf
        {
            gtk_tree_expander_set_child(tree_view_item->expander, tree_view_item->widget->operator GtkWidget*());
            gtk_tree_expander_set_list_row(tree_view_item->expander, tree_list_row);
            gtk_list_item_set_child(list_item, GTK_WIDGET(tree_view_item->expander));
        }
        else // leaf
            gtk_list_item_set_child(list_item, GTK_WIDGET(tree_view_item->widget->operator GtkWidget*()));
    }

    void TreeView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
        detail::TreeViewItem* tree_view_item = (detail::TreeViewItem*) gtk_tree_list_row_get_item(tree_list_row);

        gtk_tree_expander_set_child(tree_view_item->expander, nullptr);
    }

    void TreeView::on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*)
    {}

    void TreeView::on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*)
    {}

    TreeView::Iterator TreeView::push_back(Widget* widget, Iterator it)
    {
        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_view_item_new(widget);
        g_list_store_append(G_LIST_STORE(to_append_to), item);
        g_object_unref(item);

        return detail::G_TREE_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }
}
