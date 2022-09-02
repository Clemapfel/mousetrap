// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include "label.hpp"

namespace mousetrap
{
    class ListView : public Widget
    {
        public:
            ListView(bool collapsible = true, GtkSelectionMode mode = GTK_SELECTION_NONE);
            operator GtkWidget*();

            void push_front(Widget*);
            void push_back(Widget*);
            void insert(size_t, Widget*);

            void remove(size_t);
            void move_item_to(size_t to_move, size_t new_position);

            Widget* get_widget_at(size_t i);
            void set_widget_at(size_t i, Widget*);

            void set_enable_rubberband_selection(bool);
            void set_show_separators(bool);

        //protected:
            class Factory : public SignalEmitter
            {
                public:
                    Factory(void*);
                    operator GtkListItemFactory*();
                    operator GObject*() override;

                    static void on_setup(GtkSignalListItemFactory* self, void* object, void*);
                    static void on_teardown(GtkSignalListItemFactory* self, void* object, void*);
                    static void on_bind(GtkSignalListItemFactory* self, void* object, void*);
                    static void on_unbind(GtkSignalListItemFactory* self, void* object, void*);

                private:
                    GtkSignalListItemFactory* _native;
            };

            GtkListView* _native;
            GListStore* _list_store;

            GtkSelectionModel* _selection_model;
            Factory* _factory;
            GtkSelectionMode _mode;

            // collapsible
            bool _collapsible;
            GtkTreeListModel* _tree_list_model;
            WidgetWrapper<GtkTreeExpander>* _tree_expander;
    };
}

// #include <src/list_view.inl>

namespace mousetrap::detail
{
    // list item, holds a single widget

    #define G_TYPE_LIST_VIEW_ITEM_ITEM (list_view_item_get_type())

    G_DECLARE_FINAL_TYPE (ListViewItem, list_view_item, G, LIST_VIEW_ITEM_ITEM, GObject)

    struct _ListViewItem
    {
        GObject parent_instance;
        Widget* widget;
    };

    struct _ListViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (ListViewItem, list_view_item, G_TYPE_OBJECT)

    static void list_view_item_init(ListViewItem* item)
    {
        item->widget = nullptr;
    }
    static void list_view_item_class_init(ListViewItemClass*) {}

    static ListViewItem* list_view_item_new(Widget* in)
    {
        auto* item = (ListViewItem*) g_object_new(G_TYPE_LIST_VIEW_ITEM_ITEM, nullptr);
        list_view_item_init(item);
        item->widget = in;
        return item;
    }
}

namespace mousetrap
{
    ListView::Factory::Factory(void* data)
    {
        _native = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());

        connect_signal("bind", on_bind, data);
        /*
        connect_signal("setup", on_setup, new size_t(i));
        connect_signal("unbind", on_unbind, new size_t(i));
        connect_signal("teardown", on_teardown, new size_t(i));
         */
    }

    ListView::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    ListView::Factory::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    ListView::Factory::operator GtkListItemFactory*()
    {
        return GTK_LIST_ITEM_FACTORY(_native);
    }

    void ListView::Factory::on_setup(GtkSignalListItemFactory* self, void* item, void* col_data) {}

    void ListView::Factory::on_bind(GtkSignalListItemFactory* self, void* item, void* data)
    {
        auto* row = (GtkTreeListRow*) gtk_list_item_get_item(GTK_LIST_ITEM(item));
        detail::ListViewItem* list_view_item = (detail::ListViewItem*) gtk_tree_list_row_get_item(row);
        gtk_list_item_set_child(GTK_LIST_ITEM(item), list_view_item->widget->operator GtkWidget*());
    };

    void ListView::Factory::on_unbind(GtkSignalListItemFactory* self, void* item, void*)
    {}

    void ListView::Factory::on_teardown(GtkSignalListItemFactory* self, void* item, void*)
    {};

    static void tree_list_model_destroy(gpointer data)
    {

    }

    static GListModel* tree_list_model_create(void* item_in, void* user_data)
    {
        auto* item = (detail::ListViewItem*) item_in;
        if (GTK_IS_TREE_EXPANDER(item->widget->operator GtkWidget *()))
        {
            std::cout << "non-leaf" << std::endl;
            return G_LIST_MODEL(user_data);
        }
        else
        {
            std::cout << "leaf" << std::endl;
            return G_LIST_MODEL(g_list_store_new(G_TYPE_OBJECT));
        }
    }

    ListView::ListView(bool collapsible, GtkSelectionMode mode)
        : _mode(mode), _collapsible(collapsible)
    {
        _list_store = g_list_store_new(G_TYPE_OBJECT);
        static auto* _root = g_list_store_new(G_TYPE_OBJECT);
        _tree_list_model = gtk_tree_list_model_new(G_LIST_MODEL(_root), false, true, tree_list_model_create, _list_store, nullptr);
        _tree_expander = new WidgetWrapper(GTK_TREE_EXPANDER(gtk_tree_expander_new()));

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_tree_list_model)));

        _factory = new Factory(_tree_expander);
        _native = GTK_LIST_VIEW(gtk_list_view_new(_selection_model, _factory->operator GtkListItemFactory*()));

        auto* item = detail::list_view_item_new(_tree_expander);
        g_list_store_append(_root, item);
        g_object_unref(item);

        gtk_tree_expander_set_child(_tree_expander->operator _GtkTreeExpander*(), gtk_label_new("test"));
        gtk_tree_expander_set_list_row(_tree_expander->operator _GtkTreeExpander*(), gtk_tree_list_model_get_row(_tree_list_model, 0));
    }

    void ListView::push_front(Widget* in)
    {
        insert(0, in);
    }

    void ListView::push_back(Widget* in)
    {
        auto* item = detail::list_view_item_new(in);
        g_list_store_append(_list_store, item);
        g_object_unref(item);

        // gtk_tree_expander_set_list_row(_tree_expander->operator _GtkTreeExpander*(), gtk_tree_list_model_get_row(_tree_list_model, g_list_model_get_n_items(G_LIST_MODEL(_tree_list_model))));
    }

    void ListView::insert(size_t i, Widget* in)
    {
        auto* item = detail::list_view_item_new(in);
        g_list_store_insert(_list_store, 0, item);
        g_object_unref(item);
    }

    void ListView::remove(size_t i)
    {
        g_list_store_remove(_list_store, i);
    }

    Widget* ListView::get_widget_at(size_t i)
    {
        auto* item = (detail::ListViewItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), i);
        return item->widget;
    }

    void ListView::set_widget_at(size_t i, Widget* widget)
    {
        ((detail::ListViewItem*) g_list_model_get_item(G_LIST_MODEL(_list_store), i))->widget = widget;
        gtk_widget_show(GTK_WIDGET(_native));

        //
        // remove(i);
        // insert(i, widget);
    }

    void ListView::move_item_to(size_t to_move, size_t new_position)
    {
        auto* widget = get_widget_at(to_move);
        remove(to_move);
        insert(new_position, widget);
    }

    void ListView::set_enable_rubberband_selection(bool b)
    {
        gtk_list_view_set_enable_rubberband(_native, b);
    }

    void ListView::set_show_separators(bool b)
    {
        gtk_list_view_set_show_separators(_native, b);
    }
}
