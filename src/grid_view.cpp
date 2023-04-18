// 
// Copyright 2022 Clemens Cords
// Created on 9/22/22 by clem (mail@clemens-cords.com)
//

#include <include/grid_view.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap::detail
{
    #define G_TYPE_GRID_VIEW_ITEM (grid_view_item_get_type())

    G_DECLARE_FINAL_TYPE (GridViewItem, grid_view_item, G, GRID_VIEW_ITEM, GObject)

    struct _GridViewItem
    {
        GObject parent_instance;
        GtkWidget* widget;
    };

    struct _GridViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (GridViewItem, grid_view_item, G_TYPE_OBJECT)

    static void grid_view_item_finalize (GObject *object)
    {
        auto* self = G_GRID_VIEW_ITEM(object);
        g_object_unref(self->widget);
        G_OBJECT_CLASS (grid_view_item_parent_class)->finalize(object);
    }

    static void grid_view_item_init(GridViewItem* item)
    {
        item->widget = nullptr;
    }

    static void grid_view_item_class_init(GridViewItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = grid_view_item_finalize;
    }

    static GridViewItem* grid_view_item_new(const Widget* in)
    {
        auto* item = (GridViewItem*) g_object_new(G_TYPE_GRID_VIEW_ITEM, nullptr);
        grid_view_item_init(item);
        item->widget = in != nullptr ? in->operator NativeWidget() : nullptr;
        g_object_ref(item->widget);
        return item;
    }
    
    ///

    struct _GridViewInternal
    {
        GObject parent;

        GtkGridView* native;
        GtkSignalListItemFactory* factory;
        GListStore* list_store;

        SelectionModel* selection_model;
        GtkSelectionMode selection_mode;
        GtkOrientation orientation;
    };

    DECLARE_NEW_TYPE(GridViewInternal, grid_view_internal, GRID_VIEW_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(GridViewInternal, grid_view_internal, GRID_VIEW_INTERNAL)

    static void grid_view_internal_finalize(GObject* object)
    {
        auto* self = MOUSETRAP_GRID_VIEW_INTERNAL(object);
        G_OBJECT_CLASS(grid_view_internal_parent_class)->finalize(object);
        delete self->selection_model;
    }

    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(GridViewInternal, grid_view_internal, GRID_VIEW_INTERNAL)

    static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, detail::GridViewInternal* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        auto* object_in = detail::G_GRID_VIEW_ITEM(gtk_list_item_get_item(list_item));

        gtk_list_item_set_child(list_item, object_in->widget);
    }

    static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, detail::GridViewInternal* instance)
    {
        auto* list_item = GTK_LIST_ITEM(object);
        gtk_list_item_set_child(list_item, nullptr);
    }

    static GridViewInternal* grid_view_internal_new(GtkGridView* native, Orientation orientation, SelectionMode mode)
    {
        auto* self = (GridViewInternal*) g_object_new(grid_view_internal_get_type(), nullptr);
        grid_view_internal_init(self);

        self->orientation = (GtkOrientation) orientation;
        self->selection_mode = (GtkSelectionMode) mode;

        self->list_store = g_list_store_new(G_TYPE_OBJECT);
        self->factory = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        g_signal_connect(self->factory, "bind", G_CALLBACK(on_list_item_factory_bind), self);
        g_signal_connect(self->factory, "unbind", G_CALLBACK(on_list_item_factory_unbind), self);

        if (self->selection_mode == GTK_SELECTION_MULTIPLE)
            self->selection_model = new MultiSelectionModel(G_LIST_MODEL(self->list_store));
        else if (self->selection_mode == GTK_SELECTION_SINGLE or self->selection_mode == GTK_SELECTION_BROWSE)
            self->selection_model = new SingleSelectionModel(G_LIST_MODEL(self->list_store));
        else if (self->selection_mode == GTK_SELECTION_NONE)
            self->selection_model = new NoSelectionModel(G_LIST_MODEL(self->list_store));

        self->native = native;
        gtk_grid_view_set_model(self->native, self->selection_model->operator GtkSelectionModel*());
        gtk_grid_view_set_factory(self->native, GTK_LIST_ITEM_FACTORY(self->factory));
        gtk_orientable_set_orientation(GTK_ORIENTABLE(self->native), self->orientation);

        return self;
    }
}

namespace mousetrap
{
    GridView::GridView(Orientation orientation, SelectionMode mode)
        : WidgetImplementation<GtkGridView>(GTK_GRID_VIEW(gtk_grid_view_new(nullptr, nullptr))),
          CTOR_SIGNAL(GridView, activate)
    {
        _internal =  detail::grid_view_internal_new(get_native(), orientation, mode);
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
    }

    void GridView::push_back(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(GridView::push_back, this, ptr);

        auto* item = detail::grid_view_item_new(ptr);
        g_list_store_append(G_LIST_STORE(_internal->list_store), item);
    }

    void GridView::push_front(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(GridView::push_front, this, ptr);

        auto* item = detail::grid_view_item_new(ptr);
        g_list_store_insert(G_LIST_STORE(_internal->list_store), 0, item);
    }

    void GridView::insert(const Widget& widget, size_t i)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(GridView::insert, this, ptr);

        auto* item = detail::grid_view_item_new(&widget);
        g_list_store_insert(G_LIST_STORE(_internal->list_store), i, item);
    }

    void GridView::clear()
    {
        g_list_store_remove_all(_internal->list_store);
    }

    void GridView::remove(const Widget& widget)
    {
        size_t i = 0;
        g_list_store_remove(G_LIST_STORE(_internal->list_store), i);
    }

    void GridView::set_enable_rubberband_selection(bool b)
    {
        gtk_grid_view_set_enable_rubberband(_internal->native, b);
    }

    bool GridView::get_enable_rubberband_selection() const
    {
        return gtk_grid_view_get_enable_rubberband(_internal->native);
    }

    void GridView::set_max_n_columns(size_t n)
    {
        auto min = get_min_n_columns();
        if (min > n)
            log::warning("In GridView::set_max_n_columns: Maximum number of columns is lower than minimum number", MOUSETRAP_DOMAIN);

        gtk_grid_view_set_max_columns(_internal->native, n);
    }

    size_t GridView::get_max_n_columns() const
    {
        return gtk_grid_view_get_max_columns(_internal->native);
    }

    void GridView::set_min_n_columns(size_t n)
    {
        gtk_grid_view_set_min_columns(_internal->native, n);
    }

    size_t GridView::get_min_n_columns() const
    {
        return gtk_grid_view_get_min_columns(_internal->native);
    }

    SelectionModel* GridView::get_selection_model()
    {
        return _internal->selection_model;
    }

    size_t GridView::get_n_items() const
    {
        return g_list_model_get_n_items(G_LIST_MODEL(_internal->selection_model->operator GtkSelectionModel*()));
    }

    void GridView::set_single_click_activate(bool b)
    {
        gtk_grid_view_set_single_click_activate(_internal->native, b);
    }

    bool GridView::get_single_click_activate() const
    {
        return gtk_grid_view_get_single_click_activate(_internal->native);
    }

    void GridView::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation GridView::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}