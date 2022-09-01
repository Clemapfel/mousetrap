//
// Copyright 2022 Clemens Cords
// Created on 8/28/22 by clem (mail@clemens-cords.com)
//

#include <include/widget.hpp>

namespace mousetrap
{
    class ListItemFactory : public SignalEmitter
    {
        public:
            ListItemFactory();
            operator GtkListItemFactory*();
            operator GObject*() override;

            using SignalSignature = void(*)(GtkSignalListItemFactory* self, GtkListItem* object, void*);
            void connect_bind(SignalSignature, void*);
            void connect_unbind(SignalSignature, void*);
            void connect_setup(SignalSignature, void*);
            void connect_teardown(SignalSignature, void*);

        private:
            GtkSignalListItemFactory* _native;
    };

    class ListStore
    {
        public:
            ListStore();
            operator GListModel*();

            void append(void*);
            void insert(size_t position, void*);
            void remove(size_t position);
            void clear();

        private:
            GListStore* _native;
    };

    class ColumnView : public Widget
    {
        public:
            ColumnView(ListStore* model, GtkSelectionMode = GtkSelectionMode::GTK_SELECTION_MULTIPLE);
            operator GtkWidget*() override;

            void append_column(const std::string& label, ListItemFactory* factor);
            void set_columns_reorderable(bool);
            void set_enable_rubberband(bool);

            void set_show_column_separator(bool);
            void set_show_row_separator(bool);

            void set_column_expand(size_t i, bool);
            void set_column_fixed_width(size_t i, int);
            void set_column_resizable(size_t i, bool);
            void set_column_title(size_t i, const std::string&);
            void set_column_header_menu(size_t i, MenuModel*);

            size_t get_n_columns() const;

        private:
            GtkColumnViewColumn* get_column(size_t i);

            GtkSelectionModel* _selection_model;
            GtkColumnView* _native;

            std::vector<GtkColumnViewColumn*> _columns;
    };
}

// #include <src/column_view.inl>

// declare void pointer wrapper to inject data into g_list_store
// c.f. https://gitlab.gnome.org/GNOME/gtk/-/blob/main/demos/gtk-demo/listview_ucd.c
#define G_TYPE_VOID_POINTER_WRAPPER (void_pointer_wrapper_get_type())
G_DECLARE_FINAL_TYPE (VoidPointerWrapper, void_pointer_wrapper, G, VOID_POINTER_WRAPPER, GObject)

struct _VoidPointerWrapper
{
    GObject parent_instance;
    void* data;
};

struct _VoidPointerWrapperClass
{
    GObjectClass parent_class;
};

G_DEFINE_TYPE (VoidPointerWrapper, void_pointer_wrapper, G_TYPE_OBJECT)

static void void_pointer_wrapper_init(VoidPointerWrapper*) {}
static void void_pointer_wrapper_class_init(VoidPointerWrapperClass*) {}

static VoidPointerWrapper* void_pointer_wrapper_new(void* data)
{
    auto* item = (VoidPointerWrapper*) g_object_new(G_TYPE_VOID_POINTER_WRAPPER, NULL);
    item->data = data;
    return item;
}

namespace mousetrap
{
    // ### LIST FACTORY ###

    ListItemFactory::ListItemFactory()
    {
        _native = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
    }

    ListItemFactory::operator GtkListItemFactory*()
    {
        return GTK_LIST_ITEM_FACTORY(_native);
    }

    void ListItemFactory::connect_bind(SignalSignature f, void* data)
    {
        connect_signal("bind", f, data);
    }

    void ListItemFactory::connect_unbind(SignalSignature f, void* data)
    {
        connect_signal("unbind", f, data);
    }

    void ListItemFactory::connect_setup(SignalSignature f, void* data)
    {
        connect_signal("setup", f, data);
    }

    void ListItemFactory::connect_teardown(SignalSignature f, void* data)
    {
        connect_signal("teardown", f, data);
    }

    ListItemFactory::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    /// ### TREE STORE ###



    // ### LIST STORE ###

    ListStore::ListStore()
    {
        _native = g_list_store_new(G_TYPE_OBJECT);
        assert(_native != nullptr);
    }

    void ListStore::append(void* ptr)
    {
        auto* item = void_pointer_wrapper_new(ptr);
        g_list_store_append(_native, item);
        g_object_unref(item);
    }

    ListStore::operator GListModel*()
    {
        return G_LIST_MODEL(_native);
    }

    void ListStore::remove(size_t position)
    {
        g_list_store_remove(_native, position);
    }

    void ListStore::clear()
    {
        g_list_store_remove_all(_native);
    }

    // ### COLUMN VIEW ###

    ColumnView::ColumnView(ListStore* model, GtkSelectionMode mode)
    {
        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(model->operator GListModel *()));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(model->operator GListModel *()));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(model->operator GListModel *()));

        _native = GTK_COLUMN_VIEW(gtk_column_view_new(_selection_model));
    }

    ColumnView::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void ColumnView::append_column(const std::string& label, ListItemFactory* factor)
    {
        _columns.push_back(gtk_column_view_column_new(label.c_str(), factor->operator GtkListItemFactory *()));
        gtk_column_view_append_column(_native, _columns.back());
    }

    void ColumnView::set_columns_reorderable(bool b)
    {
        gtk_column_view_set_reorderable(_native, b);
    }

    void ColumnView::set_enable_rubberband(bool b)
    {
        gtk_column_view_set_enable_rubberband(_native, b);
    }

    void ColumnView::set_show_column_separator(bool b)
    {
        gtk_column_view_set_show_column_separators(_native, b);
    }

    void ColumnView::set_show_row_separator(bool b)
    {
        gtk_column_view_set_show_row_separators(_native, b);
    }

    GtkColumnViewColumn* ColumnView::get_column(size_t i)
    {
        return (GtkColumnViewColumn*) g_list_model_get_item(gtk_column_view_get_columns(_native), i);
    }

    void ColumnView::set_column_expand(size_t i, bool b)
    {
        gtk_column_view_column_set_expand(get_column(i), b);
    }

    void ColumnView::set_column_fixed_width(size_t i, int width)
    {
        gtk_column_view_column_set_fixed_width(get_column(i), width);
    }

    void ColumnView::set_column_header_menu(size_t i, MenuModel* model)
    {
        gtk_column_view_column_set_header_menu(get_column(i), model->operator GMenuModel*());
    }

    void ColumnView::set_column_resizable(size_t i, bool b)
    {
        gtk_column_view_column_set_resizable(get_column(i), b);
    }

    void ColumnView::set_column_title(size_t i, const std::string& name)
    {
        gtk_column_view_column_set_title(get_column(i), name.c_str());
    }

    size_t ColumnView::get_n_columns() const
    {
        return g_list_model_get_n_items(gtk_column_view_get_columns(_native));
    }
}