// 
// Copyright 2022 Clemens Cords
// Created on 9/22/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap::detail
{
    #define G_TYPE_GRID_VIEW_ITEM (grid_view_item_get_type())

    G_DECLARE_FINAL_TYPE (GridViewItem, grid_view_item, G, GRID_VIEW_ITEM, GObject)

    struct _GridViewItem
    {
        GObject parent_instance;
        Widget* widget;
        GtkWidget* widget_ref;
    };

    struct _GridViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (GridViewItem, grid_view_item, G_TYPE_OBJECT)

    static void grid_view_item_finalize (GObject *object)
    {
        auto* self = G_GRID_VIEW_ITEM(object);
        g_object_unref(self->widget_ref);
        G_OBJECT_CLASS (grid_view_item_parent_class)->finalize(object);
    }

    static void grid_view_item_init(GridViewItem* item)
    {
        item->widget = nullptr;
        item->widget_ref = nullptr;
    }

    static void grid_view_item_class_init(GridViewItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = grid_view_item_finalize;
    }

    static GridViewItem* grid_view_item_new(Widget* in)
    {
        auto* item = (GridViewItem*) g_object_new(G_TYPE_GRID_VIEW_ITEM, nullptr);
        grid_view_item_init(item);
        item->widget = in;
        item->widget_ref = g_object_ref(in->operator GtkWidget*());
        return item;
    }
}

namespace mousetrap
{
    GridView::GridView(GtkOrientation orientation, GtkSelectionMode mode)
            : WidgetImplementation<GtkGridView>([&]() -> GtkGridView* {

        _orientation = orientation;
        _selection_mode = mode;

        _list_store = g_list_store_new(G_TYPE_OBJECT);
        _factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(_factory, "bind", G_CALLBACK(on_list_item_factory_bind), this);
        g_signal_connect(_factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), this);
        g_signal_connect(_factory, "setup", G_CALLBACK(on_list_item_factory_setup), this);
        g_signal_connect(_factory, "teardown", G_CALLBACK(on_list_item_factory_teardown), this);

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = new MultiSelectionModel(G_LIST_MODEL(_list_store));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = new SingleSelectionModel(G_LIST_MODEL(_list_store));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = new NoSelectionModel(G_LIST_MODEL(_list_store));

        _native = GTK_GRID_VIEW(gtk_grid_view_new(_selection_model->operator GtkSelectionModel*(), GTK_LIST_ITEM_FACTORY(_factory)));
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_native), orientation);

        return _native;

    }())
    {}

    GridView::GridView(GtkSelectionMode mode)
            : GridView(GTK_ORIENTATION_VERTICAL, mode)
    {}

    void GridView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, GridView* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* object_in = detail::G_GRID_VIEW_ITEM(gtk_list_item_get_item(list_item));

        gtk_list_item_set_child(list_item, object_in->widget->operator GtkWidget*());
    }

    void GridView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, GridView* instance)
    {}

    void GridView::on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, GridView* instance)
    {}

    void GridView::on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, GridView* instance)
    {}

    void GridView::push_back(Widget* widget)
    {
        auto* item = detail::grid_view_item_new(widget);
        g_list_store_append(G_LIST_STORE(_list_store), item);
    }

    void GridView::push_front(Widget* widget)
    {
        auto* item = detail::grid_view_item_new(widget);
        g_list_store_insert(G_LIST_STORE(_list_store), 0, item);
    }

    void GridView::insert(Widget* widget, size_t i)
    {
        auto* item = detail::grid_view_item_new(widget);
        g_list_store_insert(G_LIST_STORE(_list_store), i, item);
    }

    void GridView::remove(Widget* widget)
    {
        size_t i = 0;
        g_list_store_remove(G_LIST_STORE(_list_store), i);
    }

    void GridView::set_enable_rubberband_selection(bool b)
    {
        gtk_grid_view_set_enable_rubberband(_native, b);
    }

    void GridView::set_max_columns(size_t i)
    {
        gtk_grid_view_set_max_columns(_native, i);
    }

    void GridView::set_min_columns(size_t i)
    {
        gtk_grid_view_set_min_columns(_native, i);
    }

    SelectionModel* GridView::get_selection_model()
    {
        return _selection_model;
    }
}