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

    class TreeStore
    {
        public:
            TreeStore(size_t n_columns);

            struct Iterator
            {
                Iterator();
                Iterator(GtkTreeIter*);
                ~Iterator();

                GtkTreeIter* _native = nullptr;
            };

            static inline const Iterator toplevel = Iterator(nullptr);

            void clear();

            Iterator insert_row(size_t position);
            Iterator insert_row_after(Iterator after_this);
            Iterator insert_row_before(Iterator before_this);
            Iterator prepend_row();
            Iterator append_row();

            void remove_row(Iterator to_remove);

            void set(Iterator row, size_t column_i, void* data);
            void set_row(Iterator row, std::vector<void*> data);

            size_t get_n_columns();
            size_t get_n_rows();

        private:
            size_t _n_columns;
            GtkListStore* _native;
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
            ColumnView(ListStore* model, GtkSelectionMode = GtkSelectionMode::GTK_SELECTION_SINGLE);
            operator GtkWidget*() override;

            void append_column(const std::string& label, ListItemFactory* factor);

        private:
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

    TreeStore::TreeStore(size_t n_columns)
        : _n_columns(n_columns)
    {
        std::vector<GType> types = {};
        types.reserve(n_columns);
        for (size_t i = 0; i < n_columns; ++i)
            types.push_back(GTK_TYPE_WIDGET);

        _native = gtk_list_store_newv(1, types.data());
    }

    TreeStore::Iterator::Iterator(GtkTreeIter* it)
    {
        if (it != nullptr)
            _native = gtk_tree_iter_copy(it);
    }

    TreeStore::Iterator::~Iterator() noexcept
    {
        if (_native != nullptr)
            gtk_tree_iter_free(_native);
    }

    TreeStore::Iterator::Iterator()
    {
        _native = new GtkTreeIter();
    }

    void TreeStore::clear()
    {
        gtk_list_store_clear(_native);
    }

    TreeStore::Iterator TreeStore::insert_row(size_t position)
    {
        auto out = Iterator();
        gtk_list_store_insert(_native, out._native, position);
        return out;
    }

    TreeStore::Iterator TreeStore::append_row()
    {
        auto out = Iterator();
        gtk_list_store_append(_native, out._native);
        return out;
    }

    TreeStore::Iterator TreeStore::prepend_row()
    {
        auto out = Iterator();
        gtk_list_store_prepend(_native, out._native);
        return out;
    }

    TreeStore::Iterator TreeStore::insert_row_after(Iterator after_this)
    {
        auto out = Iterator();
        gtk_list_store_insert_after(_native, out._native, after_this._native);
        return out;
    }

    TreeStore::Iterator TreeStore::insert_row_before(Iterator after_this)
    {
        auto out = Iterator();
        gtk_list_store_insert_before(_native, out._native, after_this._native);
        return out;
    }

    void TreeStore::remove_row(Iterator to_remove)
    {
        gtk_list_store_remove(_native, to_remove._native);
    }

    void TreeStore::set(Iterator row, size_t column_i, void* data)
    {
        auto value = new GValue();
        g_value_init(value, GTK_TYPE_WIDGET);
        g_value_set_pointer(value, data);
        gtk_list_store_set_value(_native, row._native, column_i, value);
    }

    void TreeStore::set_row(Iterator row, std::vector<void*> data)
    {
        for (size_t i = 0; i < data.size(); ++i)
            set(row, i, data.at(i));
    }

    size_t TreeStore::get_n_columns()
    {
        return _n_columns;
    }

    size_t TreeStore::get_n_rows()
    {
        return g_list_model_get_n_items(G_LIST_MODEL(_native));
    }

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

    void ListStore::insert(size_t position, void* ptr)
    {
        // TODO
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
}