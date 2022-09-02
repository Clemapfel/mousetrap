// 
// Copyright 2022 Clemens Cords
// Created on 9/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

#include <vector>

namespace mousetrap
{
    namespace detail { struct _TreeColumnViewItem; }

    class TreeColumnView : public Widget
    {
        public:
            using Iterator = detail::_TreeColumnViewItem*;

            TreeColumnView(std::vector<std::string> titles, GtkSelectionMode = GtkSelectionMode::GTK_SELECTION_MULTIPLE);
            operator GtkWidget*() override;

            Iterator push_back(std::vector<Widget*>, Iterator = nullptr);
            Iterator push_front(std::vector<Widget*>, Iterator = nullptr);
            Iterator insert(size_t, std::vector<Widget*>, Iterator = nullptr);

            Iterator move_item_to(size_t old_position, size_t new_position, Iterator old_it, Iterator new_it);
            void remove(size_t, Iterator = nullptr);

            Widget* get_widget_at(size_t row_i, size_t col_i, Iterator = nullptr);
            void set_widget_at(size_t row_i, size_t col_i, Widget*, Iterator = nullptr);

            void set_widgets_in_row(size_t row_i, std::vector<Widget*>, Iterator = nullptr);
            std::vector<Widget*> get_widgets_in_row(size_t, Iterator = nullptr);

            void set_widgets_in_column(size_t col_i, std::vector<Widget*>);
            std::vector<Widget*> get_widgets_in_column(size_t col_i);

            void set_column_expand(size_t i, bool);
            void set_column_fixed_width(size_t i, int);
            void set_column_title(size_t i, const std::string&);
            void set_column_header_menu(size_t i, MenuModel*);
            void set_column_visible(size_t i, bool b);
            void set_column_resizable(size_t i, bool);

        private:
            static void on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*);

            static GListModel* on_tree_list_model_create(void* item, void* user_data);
            static void on_tree_list_model_destroy(void* item);

            std::vector<GtkSignalListItemFactory*> _factories;

            GtkColumnView* _column_view;

            GListStore* _root;
            GtkTreeListModel* _tree_list_model;

            GtkSelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;
    };

    // using ColumnView = TreeColumnView;
}

// #include <src/column_view.inl>

namespace mousetrap::detail
{
    #define G_TYPE_TREE_COLUMN_VIEW_ITEM (tree_column_view_item_get_type())

    G_DECLARE_FINAL_TYPE (TreeColumnViewItem, tree_column_view_item, G, TREE_COLUMN_VIEW_ITEM, GObject)

    struct _TreeColumnViewItem
    {
        GObject parent_instance;

        GtkTreeExpander* expander;
        
        std::vector<Widget*>* widgets;
        std::vector<GtkWidget*>* widget_refs;
        GListStore* children;
    };

    struct _TreeColumnViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (TreeColumnViewItem, tree_column_view_item, G_TYPE_OBJECT)

    static void tree_column_view_item_finalize (GObject *object)
    {
        auto* self = G_TREE_COLUMN_VIEW_ITEM(object);
        g_object_unref(self->expander);
        g_object_unref(self->children);
        
        for (auto* w : *(self->widget_refs))
            g_object_unref(w);
        
        delete self->widgets;
        delete self->widget_refs;
        
        G_OBJECT_CLASS (tree_column_view_item_parent_class)->finalize (object);
    }

    static void tree_column_view_item_init(TreeColumnViewItem* item)
    {
        static size_t i = 0;
        item->expander = g_object_ref(GTK_TREE_EXPANDER(gtk_tree_expander_new()));
        item->widgets = new std::vector<Widget*>();
        item->widget_refs = new std::vector<GtkWidget*>();
        item->children = g_object_ref(g_list_store_new(G_TYPE_OBJECT));
    }

    static void tree_column_view_item_class_init(TreeColumnViewItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = tree_column_view_item_finalize;
    }

    static TreeColumnViewItem* tree_column_view_item_new(const std::vector<Widget*>& in)
    {
        auto* item = (TreeColumnViewItem*) g_object_new(G_TYPE_TREE_COLUMN_VIEW_ITEM, nullptr);
        tree_column_view_item_init(item);
        
        for (auto* w : in)
        {
            item->widgets->push_back(w);
            item->widget_refs->push_back(g_object_ref(w->operator GtkWidget*()));
        }
        
        return item;
    }
}

namespace mousetrap
{
    TreeColumnView::TreeColumnView(std::vector<std::string> titles, GtkSelectionMode mode)
    {
        _root = g_list_store_new(G_TYPE_OBJECT);
        _tree_list_model = gtk_tree_list_model_new(G_LIST_MODEL(_root), false, false, on_tree_list_model_create, nullptr, on_tree_list_model_destroy);

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_tree_list_model)));

        _column_view = GTK_COLUMN_VIEW(gtk_column_view_new(_selection_model));

        for (size_t i = 0; i < titles.size(); ++i)
        {
            _factories.push_back(GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new()));
            g_signal_connect(_factories.back(), "bind", G_CALLBACK(on_list_item_factory_bind), new size_t(i));
            g_signal_connect(_factories.back(), "unbind", G_CALLBACK(on_list_item_factory_unbind), new size_t(i));
            g_signal_connect(_factories.back(), "setup", G_CALLBACK(on_list_item_factory_setup), new size_t(i));
            g_signal_connect(_factories.back(), "teardown", G_CALLBACK(on_list_item_factory_teardown), new size_t(i));

            auto* column = gtk_column_view_column_new(titles.at(i).c_str(), GTK_LIST_ITEM_FACTORY(_factories.back()));
            gtk_column_view_column_set_expand(column, true);
            gtk_column_view_append_column(_column_view, column);
        }
    }
    
    TreeColumnView::operator GtkWidget*()
    {
        return GTK_WIDGET(_column_view);
    }

    GListModel* TreeColumnView::on_tree_list_model_create(void* item, void* user_data)
    {
        auto* column_view_item = (detail::TreeColumnViewItem*) item;

        auto* out = g_list_store_new(G_TYPE_OBJECT);

        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(column_view_item->children)); ++i)
            g_list_store_append(out, g_list_model_get_item(G_LIST_MODEL(column_view_item->children), i));

        return G_LIST_MODEL(out);
    }

    void TreeColumnView::on_tree_list_model_destroy(void* item)
    {}

    void TreeColumnView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void* col_data)
    {
        size_t col_i = *((size_t*) col_data);
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
        detail::TreeColumnViewItem* column_view_item = (detail::TreeColumnViewItem*) gtk_tree_list_row_get_item(tree_list_row);

        if (g_list_model_get_n_items(G_LIST_MODEL(column_view_item->children)) != 0 and col_i == 0) // non-leaf
        {
            gtk_tree_expander_set_child(column_view_item->expander, column_view_item->widgets->at(col_i)->operator GtkWidget *());
            gtk_tree_expander_set_list_row(column_view_item->expander, tree_list_row);
            gtk_list_item_set_child(list_item, GTK_WIDGET(column_view_item->expander));
        }
        else // leaf
            gtk_list_item_set_child(list_item, GTK_WIDGET(column_view_item->widgets->at(col_i)->operator GtkWidget*()));
    }

    void TreeColumnView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*)
    {}

    void TreeColumnView::on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*)
    {}

    void TreeColumnView::on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*)
    {}

    TreeColumnView::Iterator TreeColumnView::push_back(std::vector<Widget*> widgets, Iterator it)
    {
        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_column_view_item_new(widgets);
        g_list_store_append(G_LIST_STORE(to_append_to), item);
        g_object_unref(item);

        return detail::G_TREE_COLUMN_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }

    TreeColumnView::Iterator TreeColumnView::push_front(std::vector<Widget*> widgets, Iterator it)
    {
        insert(0, widgets, it);
    }

    TreeColumnView::Iterator TreeColumnView::insert(size_t i, std::vector<Widget*> widgets, Iterator it)
    {
        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_root);
        else
            list = G_LIST_MODEL(it->children);

        auto* item = detail::tree_column_view_item_new(widgets);
        g_list_store_insert(G_LIST_STORE(list), 0, item);
        g_object_unref(item);

        return detail::G_TREE_COLUMN_VIEW_ITEM(g_list_model_get_item(list, g_list_model_get_n_items(to_append_to) - 1));
    }

    TreeColumnView::Iterator TreeColumnView::move_item_to(size_t old_position, size_t new_position, Iterator old_iterator, Iterator new_iterator)
    {
        std::vector<Widget*> widgets = get_widgets_in_row(old_position, old_iterator);
        remove(old_position, old_iterator);
        return insert(new_position, widgets, new_iterator);
    }

    void TreeColumnView::remove(size_t i, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        g_list_store_remove(list, i);
    }

    Tree

}