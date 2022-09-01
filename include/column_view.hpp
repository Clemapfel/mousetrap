// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/widget.hpp>

#include <vector>
#include <deque>

namespace mousetrap::detail
{
    // row item, hold col-number of widgets
    #define G_TYPE_VOID_POINTER_WRAPPER (row_item_get_type())

    G_DECLARE_FINAL_TYPE (RowItem, row_item, G, ROW_ITEM, GObject)

    struct _RowItem
    {
        GObject parent_instance;
        std::vector<GtkWidget*>* widgets;
    };

    struct _RowItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (RowItem, row_item, G_TYPE_OBJECT)

    static void row_item_init(RowItem*) {}
    static void row_item_class_init(RowItemClass*) {}

    static RowItem* row_item_new(std::vector<GtkWidget*> in)
    {
        auto* item = (RowItem*) g_object_new(G_TYPE_VOID_POINTER_WRAPPER, NULL);
        item->widgets = new std::vector<GtkWidget*>(in.begin(), in.end());
        return item;
    }
}

namespace mousetrap
{
    class ColumnView : public Widget
    {
        public:
            ColumnView(std::vector<std::string> titles, GtkSelectionMode = GtkSelectionMode::GTK_SELECTION_MULTIPLE);
            operator GtkWidget*() override;

            void append_row(std::vector<GtkWidget*> widgets);

        private:
            GtkColumnView* _native;

            class RowListStore
            {
                public:
                    RowListStore();
                    operator GListModel*();

                    void append(std::vector<GtkWidget*>);
                    void insert(size_t position,  std::vector<GtkWidget*>);
                    void remove(size_t position);
                    void clear();

                private:
                    GListStore* _native;
            };

            RowListStore* _row_list_store;
            GtkSelectionModel* _selection_model;

            class ColumnFactory : public SignalEmitter
            {
                public:
                    ColumnFactory(size_t column_i);
                    operator GtkListItemFactory*();
                    operator GObject*() override;

                    void set_column_index(size_t i);

                    static void on_setup(GtkSignalListItemFactory* self, GtkListItem* object, void*);
                    static void on_teardown(GtkSignalListItemFactory* self, GtkListItem* object, void*);
                    static void on_bind(GtkSignalListItemFactory* self, GtkListItem* object, void*);
                    static void on_unbind(GtkSignalListItemFactory* self, GtkListItem* object, void*);

                private:
                    GtkSignalListItemFactory* _native;
            };

            std::deque<ColumnFactory*> _column_factories;
    };
}

// #include <src/column_view.inl>

namespace mousetrap
{
    // ### LIST STORE ###

    ColumnView::RowListStore::RowListStore()
    {
        _native = g_list_store_new(G_TYPE_OBJECT);
    }

    void ColumnView::RowListStore::append(std::vector<GtkWidget*> in)
    {
        auto* item = detail::row_item_new(in);
        g_list_store_append(_native, item);
        g_object_unref(item);
    }

    ColumnView::RowListStore::operator GListModel*()
    {
        return G_LIST_MODEL(_native);
    }

    void ColumnView::RowListStore::remove(size_t position)
    {
        g_list_store_remove(_native, position);
    }

    void ColumnView::RowListStore::clear()
    {
        g_list_store_remove_all(_native);
    }

    // ### FACTORY ###

    ColumnView::ColumnFactory::ColumnFactory(size_t i)
    {
        _native = GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new());
        set_column_index(i);
    }

    ColumnView::ColumnFactory::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    ColumnView::ColumnFactory::operator GtkListItemFactory*()
    {
        return GTK_LIST_ITEM_FACTORY(_native);
    }

    void ColumnView::ColumnFactory::set_column_index(size_t i)
    {
        connect_signal("bind", on_bind, new size_t(i));
        connect_signal("unbind", on_unbind, new size_t(i));
        connect_signal("setup", on_setup, new size_t(i));
        connect_signal("teardown", on_teardown, new size_t(i));
    }

    void ColumnView::ColumnFactory::on_setup(GtkSignalListItemFactory* self, GtkListItem* item, void* col_data)
    {
        size_t row_i =  gtk_list_item_get_position(item);
        size_t col_i = *((size_t*) col_data);
        detail::RowItem* row_item = (detail::RowItem*) gtk_list_item_get_item(item);

        if (row_item->widgets->size() >= col_i)
            return;

        gtk_list_item_set_child(item, row_item->widgets.at(col_i));
    }

    void ColumnView::ColumnFactory::on_bind(GtkSignalListItemFactory* self, GtkListItem* object, void*) {};
    void ColumnView::ColumnFactory::on_unbind(GtkSignalListItemFactory* self, GtkListItem* object, void*) {};
    void ColumnView::ColumnFactory::on_teardown(GtkSignalListItemFactory* self, GtkListItem* object, void*) {};

    // ### COLUMN VIEW ###

    ColumnView::ColumnView(std::vector<std::string> titles, GtkSelectionMode mode)
    {
        _row_list_store = new ColumnView::RowListStore();

        if (mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(_row_list_store->operator GListModel *()));
        else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(_row_list_store->operator GListModel *()));
        else if (mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(_row_list_store->operator GListModel *()));

        _native = GTK_COLUMN_VIEW(gtk_column_view_new(_selection_model));

        for (size_t i = 0; i < titles.size(); ++i)
        {
            _column_factories.push_back(new ColumnView::ColumnFactory(i));
            gtk_column_view_append_column(
                _native,
              gtk_column_view_column_new(
                  titles.at(i).c_str(),
                  _column_factories.back()->operator GtkListItemFactory*()
              )
          );
        }
    }

    ColumnView::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void ColumnView::append_row(std::vector<Widget*> widgets)
    {
        std::vector<GtkWidget*> to_append;

        for (auto& w : widgets)
            to_append.push_back(w->operator GtkWidget *());

        _row_list_store->append(to_append);
    }
}

