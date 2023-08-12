//
// Copyright 2022 Clemens Cords
// Created on 9/2/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/list_view.hpp>
#include <mousetrap/log.hpp>
#include <mousetrap/selection_model.hpp>

namespace mousetrap::detail
{
    #define G_TYPE_LIST_VIEW_ITEM (tree_list_view_item_get_type())

    G_DECLARE_FINAL_TYPE (ListViewItem, tree_list_view_item, G, TREE_VIEW_ITEM, GObject)

    struct _ListViewItem
    {
        GObject parent_instance;

        GtkTreeExpander* expander;
        GtkWidget* widget;
        GListStore* children;
        uint64_t depth;
    };

    struct _ListViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (ListViewItem, tree_list_view_item, G_TYPE_OBJECT)

    static void tree_list_view_item_finalize(GObject *object)
    {
        auto* self = G_TREE_VIEW_ITEM(object);
        g_object_unref(self->widget);
        g_object_unref(self->expander);
        g_object_unref(self->children);

        G_OBJECT_CLASS(tree_list_view_item_parent_class)->finalize (object);
    }

    static void tree_list_view_item_init(ListViewItem* item)
    {
        item->expander = g_object_ref(GTK_TREE_EXPANDER(gtk_tree_expander_new()));
        item->widget = nullptr;
        item->children = g_object_ref(g_list_store_new(G_TYPE_OBJECT));
        item->depth = 0;
        gtk_tree_expander_set_indent_for_icon(item->expander, true);

        #if GTK_MINOR_VERSION >= 10
            gtk_tree_expander_set_indent_for_depth(item->expander, true);
        #endif
    }

    static void tree_list_view_item_class_init(ListViewItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = tree_list_view_item_finalize;
    }

    static ListViewItem* tree_list_view_item_new(const Widget* in)
    {
        auto* item = (ListViewItem*) g_object_new(G_TYPE_LIST_VIEW_ITEM, nullptr);
        tree_list_view_item_init(item);
        item->widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_halign(item->widget, GTK_ALIGN_START);
        if (in != nullptr)
            gtk_box_append(GTK_BOX(item->widget), in->operator NativeWidget());

        g_object_ref(item->widget);
        return item;
    }
    
    ///
    struct _ListViewInternal
    {
        GObject parent;

        GtkSignalListItemFactory* factory;

        GtkListView* list_view;
        GListStore* root;
        GtkTreeListModel* tree_list_model;

        SelectionModel* selection_model;
        GtkSelectionMode selection_mode;
        GtkOrientation orientation;
    };

    DECLARE_NEW_TYPE(ListViewInternal, list_view_internal, LIST_VIEW_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(ListViewInternal, list_view_internal, LIST_VIEW_INTERNAL)

    static void list_view_internal_finalize(GObject* object)
    {
        auto* self = MOUSETRAP_LIST_VIEW_INTERNAL(object);
        G_OBJECT_CLASS(list_view_internal_parent_class)->finalize(object);
        delete self->selection_model;

        g_object_unref(self->root);
        g_object_unref(self->tree_list_model);
        g_object_unref(self->factory);
    }

    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ListViewInternal, list_view_internal, LIST_VIEW_INTERNAL)

    static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, detail::ListViewInternal*)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
        auto* tree_list_view_item = G_TREE_VIEW_ITEM(gtk_tree_list_row_get_item(tree_list_row));

        if (g_list_model_get_n_items(G_LIST_MODEL(tree_list_view_item->children)) != 0) // non-leaf
        {
            //#if GTK_MINOR_VERSION < 10
                gtk_widget_set_margin_start(tree_list_view_item->widget, 0);
            //#endif

            gtk_tree_expander_set_child(tree_list_view_item->expander, tree_list_view_item->widget);
            gtk_tree_expander_set_list_row(tree_list_view_item->expander, tree_list_row);
            gtk_list_item_set_child(list_item, GTK_WIDGET(tree_list_view_item->expander));
        }
        else // leaf
        {
           gtk_list_item_set_child(list_item, tree_list_view_item->widget);

            //#if GTK_MINOR_VERSION < 10
                gtk_widget_set_margin_start(tree_list_view_item->widget, ListView::indent_per_depth * tree_list_view_item->depth);
            //#endif
        }

        gtk_list_item_set_activatable(list_item, true);

    }

    static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, detail::ListViewInternal*)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        gtk_list_item_set_child(list_item, nullptr);
        gtk_list_item_set_activatable(list_item, false);
    }

    static GListModel* on_tree_list_model_create(void* item, void* user_data)
    {
        auto* tree_list_view_item = (detail::ListViewItem*) item;

        auto* out = g_object_ref(g_list_store_new(G_TYPE_OBJECT));

        for (uint64_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(tree_list_view_item->children)); ++i)
            g_list_store_append(out, g_list_model_get_item(G_LIST_MODEL(tree_list_view_item->children), i));

        return G_LIST_MODEL(out);
    }

    static void on_tree_list_model_destroy(void* item)
    {
        g_object_unref(item);
    }

    static ListViewInternal* list_view_internal_new(GtkListView* native, Orientation orientation, SelectionMode mode)
    {
        auto* self = (ListViewInternal*) g_object_new(list_view_internal_get_type(), nullptr);
        list_view_internal_init(self);

        self->root = g_list_store_new(G_TYPE_OBJECT);
        self->tree_list_model = gtk_tree_list_model_new(
            G_LIST_MODEL(self->root),
            false,
            false,
            on_tree_list_model_create, nullptr, on_tree_list_model_destroy);

        self->orientation = (GtkOrientation) orientation;
        self->selection_mode = (GtkSelectionMode) mode;
        self->selection_model = new SelectionModel(mode, G_LIST_MODEL(self->tree_list_model));

        self->factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(self->factory, "bind", G_CALLBACK(on_list_item_factory_bind), self);
        g_signal_connect(self->factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), self);

        self->list_view = native;
        gtk_list_view_set_model(self->list_view, self->selection_model->operator GtkSelectionModel*());
        gtk_list_view_set_factory(self->list_view, GTK_LIST_ITEM_FACTORY(self->factory));
        gtk_orientable_set_orientation(GTK_ORIENTABLE(self->list_view), self->orientation);

        g_object_ref(self->root);
        g_object_ref(self->tree_list_model);
        g_object_ref(self->factory);

        return self;
    }
}

namespace mousetrap
{
    ListView::ListView(Orientation orientation, SelectionMode mode)
        : Widget(gtk_list_view_new(nullptr, nullptr)),
          CTOR_SIGNAL(ListView, activate_item),
          CTOR_SIGNAL(ListView, realize),
          CTOR_SIGNAL(ListView, unrealize),
          CTOR_SIGNAL(ListView, destroy),
          CTOR_SIGNAL(ListView, hide),
          CTOR_SIGNAL(ListView, show),
          CTOR_SIGNAL(ListView, map),
          CTOR_SIGNAL(ListView, unmap)
    {
        _internal = g_object_ref(detail::list_view_internal_new(GTK_LIST_VIEW(operator NativeWidget()), orientation, mode));
        detail::attach_ref_to(G_OBJECT(_internal->list_view), _internal);
    }

    ListView::ListView(detail::ListViewInternal* internal)
        : Widget(GTK_WIDGET(internal->list_view)),
          CTOR_SIGNAL(ListView, activate_item),
          CTOR_SIGNAL(ListView, realize),
          CTOR_SIGNAL(ListView, unrealize),
          CTOR_SIGNAL(ListView, destroy),
          CTOR_SIGNAL(ListView, hide),
          CTOR_SIGNAL(ListView, show),
          CTOR_SIGNAL(ListView, map),
          CTOR_SIGNAL(ListView, unmap)
    {
        _internal = g_object_ref(internal);
    }

    ListView::~ListView()
    {
        g_object_unref(_internal);
    }

    NativeObject ListView::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    ListView::Iterator ListView::push_back(const Widget& widget, Iterator it)
    {
        if (widget.operator GtkWidget*() == this->operator GtkWidget*())
        {
            log::critical("In ListView::push_back: Attempting to insert ListView into itself, this would cause an infinite loop");
            return nullptr;
        }

        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_internal->root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_list_view_item_new(&widget);
        if (it != nullptr)
            item->depth = it->depth + 1;

        g_list_store_append(G_LIST_STORE(to_append_to), item);
        g_object_unref(item);

        return detail::G_TREE_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }

    ListView::Iterator ListView::push_front(const Widget& widget, Iterator it)
    {
        if (widget.operator GtkWidget*() == this->operator GtkWidget*())
        {
            log::critical("In ListView::push_front: Attempting to insert ListView into itself, this would cause an infinite loop");
            return nullptr;
        }

        return insert(widget, 0, it);
    }

    ListView::Iterator ListView::insert(const Widget& widget, uint64_t i, Iterator it)
    {
        if (widget.operator GtkWidget*() == this->operator GtkWidget*())
        {
            log::critical("In ListView::insert: Attempting to insert ListView into itself, this would cause an infinite loop");
            return nullptr;
        }

        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_internal->root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_list_view_item_new(&widget);
        if (it != nullptr)
            item->depth = it->depth + 1;

        auto n =  g_list_model_get_n_items(G_LIST_MODEL(to_append_to));
        if (i > n)
            i = n == 0 ? 0 : n - 1;

        g_list_store_insert(G_LIST_STORE(to_append_to), i, item);
        g_object_unref(item);

        return detail::G_TREE_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }

    void ListView::remove(uint64_t i, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _internal->root;
        else
            list = it->children;

        g_list_store_remove(list, i);
    }

    void ListView::clear(Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _internal->root;
        else
            list = it->children;

        gtk_selection_model_unselect_all(_internal->selection_model->operator GtkSelectionModel*());
        g_list_store_remove_all(list);
    }

    /*
    Widget* ListView::get_widget_at(uint64_t i, Iterator it)
    {
        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_internal->root);
        else
            list = G_LIST_MODEL(it->children);

        auto* item =  detail::G_TREE_VIEW_ITEM(g_list_model_get_item(list, i));
        return const_cast<Widget*>(item->widget);
    }
     */

    void ListView::set_widget_at(uint64_t i, const Widget& widget, Iterator it)
    {
        if (widget.operator GtkWidget*() == this->operator GtkWidget*())
        {
            log::critical("In ListView::push_back: Attempting to insert ListView into itself, this would cause an infinite loop");
            return;
        }

        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_internal->root);
        else
            list = G_LIST_MODEL(it->children);

        auto* item =  detail::G_TREE_VIEW_ITEM(g_list_model_get_item(list, i));
        item->widget = widget.operator GtkWidget*();
        g_list_model_items_changed(G_LIST_MODEL(list), i, 0, 0);
    }

    int ListView::find(const Widget& widget, Iterator it) const
    {
        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_internal->root);
        else
            list = G_LIST_MODEL(it->children);

        for (int i = 0; i < g_list_model_get_n_items(list); ++i)
        {
            auto* item = detail::G_TREE_VIEW_ITEM(g_list_model_get_item(list, i));
            if (gtk_widget_get_first_child(item->widget) == widget.operator NativeWidget())
                return i;
        }

        return -1;
    }

    void ListView::set_enable_rubberband_selection(bool b)
    {
        gtk_list_view_set_enable_rubberband(_internal->list_view, b);
    }

    bool ListView::get_enable_rubberband_selection() const
    {
        return gtk_list_view_get_enable_rubberband(_internal->list_view);
    }

    void ListView::set_show_separators(bool b)
    {
        gtk_list_view_set_show_separators(_internal->list_view, b);
    }

    bool ListView::get_show_separators() const
    {
        return gtk_list_view_get_show_separators(_internal->list_view);
    }

    void ListView::set_single_click_activate(bool b)
    {
        gtk_list_view_set_single_click_activate(_internal->list_view, b);
    }

    bool ListView::get_single_click_activate() const
    {
        return gtk_list_view_get_single_click_activate(_internal->list_view);
    }

    SelectionModel* ListView::get_selection_model()
    {
        return _internal->selection_model;
    }

    uint64_t ListView::get_n_items() const
    {
        return g_list_model_get_n_items(G_LIST_MODEL(_internal->root));
    }

    void ListView::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(GTK_LIST_VIEW(operator NativeWidget())), (GtkOrientation) orientation);
    }

    Orientation ListView::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(GTK_LIST_VIEW(operator NativeWidget())));
    }
}
