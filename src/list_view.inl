// 
// Copyright 2022 Clemens Cords
// Created on 9/2/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap::detail
{
    #define G_TYPE_LIST_VIEW_ITEM_ITEM (tree_list_view_item_get_type())

    G_DECLARE_FINAL_TYPE (TreeListViewItem, tree_list_view_item, G, TREE_VIEW_ITEM, GObject)

    struct _TreeListViewItem
    {
        GObject parent_instance;

        GtkTreeExpander* expander;
        Widget* widget;
        GtkWidget* widget_ref;
        GListStore* children;
    };

    struct _TreeListViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (TreeListViewItem, tree_list_view_item, G_TYPE_OBJECT)

    static void tree_list_view_item_finalize (GObject *object)
    {
        auto* self = G_TREE_VIEW_ITEM(object);
        g_object_unref(self->expander);
        g_object_unref(self->children);
        g_object_unref(self->widget_ref);

        G_OBJECT_CLASS (tree_list_view_item_parent_class)->finalize (object);
    }

    static void tree_list_view_item_init(TreeListViewItem* item)
    {
        static size_t i = 0;
        item->expander = g_object_ref(GTK_TREE_EXPANDER(gtk_tree_expander_new()));
        item->widget = nullptr;
        item->widget_ref = nullptr;
        item->children = g_object_ref(g_list_store_new(G_TYPE_OBJECT));
    }

    static void tree_list_view_item_class_init(TreeListViewItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = tree_list_view_item_finalize;
    }

    static TreeListViewItem* tree_list_view_item_new(Widget* in)
    {
        auto* item = (TreeListViewItem*) g_object_new(G_TYPE_LIST_VIEW_ITEM_ITEM, nullptr);
        tree_list_view_item_init(item);
        item->widget = in;
        item->widget_ref = g_object_ref(in->operator GtkWidget*());
        return item;
    }
}

namespace mousetrap
{
    TreeListView::TreeListView(GtkSelectionMode mode)
    {
        _root = g_list_store_new(G_TYPE_OBJECT);
        _tree_list_model = gtk_tree_list_model_new(G_LIST_MODEL(_root), false, false, on_tree_list_model_create, nullptr, on_tree_list_model_destroy);

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_tree_list_model)));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_tree_list_model)));

        _factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_factory, "bind", G_CALLBACK(on_list_item_factory_bind), this);
        g_signal_connect(_factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), this);
        g_signal_connect(_factory, "setup", G_CALLBACK(on_list_item_factory_setup), this);
        g_signal_connect(_factory, "teardown", G_CALLBACK(on_list_item_factory_teardown), this);

        _list_view = GTK_LIST_VIEW(gtk_list_view_new(_selection_model, GTK_LIST_ITEM_FACTORY(_factory)));
    }

    TreeListView::operator GtkWidget*()
    {
        return GTK_WIDGET(_list_view);
    }

    GListModel* TreeListView::on_tree_list_model_create(void* item, void* user_data)
    {
        auto* tree_list_view_item = (detail::TreeListViewItem*) item;

        auto* out = g_list_store_new(G_TYPE_OBJECT);

        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(tree_list_view_item->children)); ++i)
            g_list_store_append(out, g_list_model_get_item(G_LIST_MODEL(tree_list_view_item->children), i));

        return G_LIST_MODEL(out);
    }

    void TreeListView::on_tree_list_model_destroy(void* item)
    {}

    void TreeListView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
        detail::TreeListViewItem* tree_list_view_item = (detail::TreeListViewItem*) gtk_tree_list_row_get_item(tree_list_row);

        if (g_list_model_get_n_items(G_LIST_MODEL(tree_list_view_item->children)) != 0) // non-leaf
        {
            gtk_tree_expander_set_child(tree_list_view_item->expander, tree_list_view_item->widget->operator GtkWidget*());
            gtk_tree_expander_set_list_row(tree_list_view_item->expander, tree_list_row);
            gtk_list_item_set_child(list_item, GTK_WIDGET(tree_list_view_item->expander));
        }
        else // leaf
            gtk_list_item_set_child(list_item, GTK_WIDGET(tree_list_view_item->widget->operator GtkWidget*()));
    }

    void TreeListView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
    }

    void TreeListView::on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*)
    {}

    void TreeListView::on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*)
    {}

    TreeListView::Iterator TreeListView::push_back(Widget* widget, Iterator it)
    {
        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_list_view_item_new(widget);
        g_list_store_append(G_LIST_STORE(to_append_to), item);
        g_object_unref(item);

        return detail::G_TREE_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }

    TreeListView::Iterator TreeListView::push_front(Widget* widget, Iterator it)
    {
        insert(0, widget, it);
    }

    TreeListView::Iterator TreeListView::insert(size_t i, Widget* widget, Iterator it)
    {
        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_list_view_item_new(widget);
        g_list_store_insert(G_LIST_STORE(to_append_to), i, item);
        g_object_unref(item);

        return detail::G_TREE_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }

    void TreeListView::remove(size_t i, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        g_list_store_remove(list, i);
    }

    Widget* TreeListView::get_widget_at(size_t i, Iterator it)
    {
        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_root);
        else
            list = G_LIST_MODEL(it->children);

        auto* item =  detail::G_TREE_VIEW_ITEM(g_list_model_get_item(list, i));
        return item->widget;
    }

    void TreeListView::set_widget_at(size_t i, Widget* widget, Iterator it)
    {
        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_root);
        else
            list = G_LIST_MODEL(it->children);

        auto* item =  detail::G_TREE_VIEW_ITEM(g_list_model_get_item(list, i));
        item->widget = widget;

        g_object_unref(item->widget_ref);
        item->widget_ref = g_object_ref(widget->operator GtkWidget*());
    }

    void TreeListView::set_enable_rubberband_selection(bool b)
    {
        gtk_list_view_set_enable_rubberband(_list_view, b);
    }

    void TreeListView::set_show_separators(bool b)
    {
        gtk_list_view_set_show_separators(_list_view, b);
    }

    TreeListView::Iterator TreeListView::move_item_to(size_t old_position, size_t new_position, Iterator old_it, Iterator new_it)
    {
        auto* widget = get_widget_at(old_position, old_it);
        remove(old_position, old_it);
        return insert(new_position, widget, new_it);
    }
}
