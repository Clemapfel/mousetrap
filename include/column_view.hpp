// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/widget.hpp>
#include <include/menu_model.hpp>

#include <vector>
#include <deque>

#include <thread>

namespace mousetrap
{
    namespace detail { struct _ColumnViewRowItem; }

    class ColumnView : public Widget
    {
        public:
            ColumnView(std::vector<std::string> titles, GtkSelectionMode = GtkSelectionMode::GTK_SELECTION_MULTIPLE);
            operator GtkWidget*() override;

            void append_row(std::vector<Widget*> widgets);
            void insert_row(size_t, std::vector<Widget*> widgets);
            void remove_row(size_t);

            void move_row_to(size_t to_move, size_t new_position);

            void set_columns_reorderable(bool);
            void set_enable_rubberband_selection(bool);

            void set_show_column_separator(bool);
            void set_show_row_separator(bool);

            void set_column_expand(size_t i, bool);
            void set_column_fixed_width(size_t i, int);
            void set_column_title(size_t i, const std::string&);
            void set_column_header_menu(size_t i, MenuModel*);

            void set_column_resizable(size_t i, bool);

            size_t get_n_columns() const;
            size_t get_n_rows() const;

            std::vector<Widget*> get_widgets_in_row(size_t i);
            std::vector<Widget*> get_widgets_in_column(size_t i);
            Widget* get_widget_at(size_t row_i, size_t col_i);
            void set_widget_at(size_t row_i, size_t col_i, Widget*);

        private:
            GtkColumnView* _native;

            class RowListStore
            {
                public:
                    RowListStore();
                    operator GListModel*();
                    operator GListStore*();

                    void append(std::vector<Widget*>);
                    void insert(size_t position,  std::vector<Widget*>);
                    void remove(size_t position);
                    void clear();

                    detail::_ColumnViewRowItem* at(size_t);
                    size_t get_n_items();

                private:
                    GListStore* _native;
            };

            RowListStore* _row_list_store;

            GtkSelectionMode _selection_mode;
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

            GtkColumnViewColumn* get_column(size_t i);
    };
}

// #include <src/column_view.inl>

namespace mousetrap::detail
{
    // row item, holds col-number of widgets

    #define G_TYPE_COLUMN_VIEW_ROW_ITEM (column_view_row_item_get_type())

    G_DECLARE_FINAL_TYPE (ColumnViewRowItem, column_view_row_item, G, COLUMN_VIEW_ROW_ITEM, GObject)

    struct _ColumnViewRowItem
    {
        GObject parent_instance;
        std::vector<Widget*>* widgets;
    };

    struct _ColumnViewRowItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (ColumnViewRowItem, column_view_row_item, G_TYPE_OBJECT)

    static void column_view_row_item_init(ColumnViewRowItem* item)
    {
        item->widgets = new std::vector<Widget*>();
    }
    static void column_view_row_item_class_init(ColumnViewRowItemClass*) {}

    static ColumnViewRowItem* column_view_row_item(std::vector<Widget*> in)
    {
        auto* item = (ColumnViewRowItem*) g_object_new(G_TYPE_COLUMN_VIEW_ROW_ITEM, NULL);
        column_view_row_item_init(item);

        for (size_t i = 0; i < in.size(); ++i)
            item->widgets->push_back(in.at(i));

        return item;
    }
}

namespace mousetrap
{
    // ### LIST STORE ###

    ColumnView::RowListStore::RowListStore()
    {
        _native = g_list_store_new(G_TYPE_OBJECT);
    }

    void ColumnView::RowListStore::append(std::vector<Widget*> in)
    {
        auto* item = detail::column_view_row_item(in);
        g_list_store_append(_native, item);
        g_object_unref(item);
    }

    void ColumnView::RowListStore::insert(size_t i, std::vector<Widget*> in)
    {
        auto* item = detail::column_view_row_item(in);
        g_list_store_insert(_native, i, item);
        g_object_unref(item);
    }

    ColumnView::RowListStore::operator GListModel*()
    {
        return G_LIST_MODEL(_native);
    }

    ColumnView::RowListStore::operator GListStore*()
    {
        return _native;
    }

    void ColumnView::RowListStore::remove(size_t position)
    {
        g_list_store_remove(_native, position);
    }

    void ColumnView::RowListStore::clear()
    {
        g_list_store_remove_all(_native);
    }

    detail::ColumnViewRowItem* ColumnView::RowListStore::at(size_t i)
    {
        return (detail::ColumnViewRowItem*) g_list_model_get_item(G_LIST_MODEL(_native), i);
    }

    size_t ColumnView::RowListStore::get_n_items()
    {
        g_list_model_get_n_items(G_LIST_MODEL(_native));
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

        /*
        connect_signal("setup", on_setup, new size_t(i));
        connect_signal("unbind", on_unbind, new size_t(i));
        connect_signal("teardown", on_teardown, new size_t(i));
         */
    }

    void ColumnView::ColumnFactory::on_setup(GtkSignalListItemFactory* self, GtkListItem* item, void* col_data) {}

    void ColumnView::ColumnFactory::on_bind(GtkSignalListItemFactory* self, GtkListItem* item, void* col_data)
    {
        [[maybe_unused]] size_t row_i =  gtk_list_item_get_position(item);
        size_t col_i = *((size_t*) col_data);
        detail::ColumnViewRowItem* row_item = (detail::ColumnViewRowItem*) gtk_list_item_get_item(item);

        gtk_list_item_set_child(item, row_item->widgets->at(col_i)->operator GtkWidget *());
    };

    void ColumnView::ColumnFactory::on_unbind(GtkSignalListItemFactory* self, GtkListItem* item, void*)
    {
        gtk_list_item_set_child(item, nullptr);
    };

    void ColumnView::ColumnFactory::on_teardown(GtkSignalListItemFactory* self, GtkListItem* item, void*)
    {
        gtk_list_item_set_child(item, nullptr);
    };

    // ### COLUMN VIEW ###

    ColumnView::ColumnView(std::vector<std::string> titles, GtkSelectionMode mode)
        : _selection_mode(mode)
    {
        _row_list_store = new ColumnView::RowListStore();

        if (_selection_mode == GTK_SELECTION_MULTIPLE)
            _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(_row_list_store->operator GListModel *()));
        else if (_selection_mode == GTK_SELECTION_SINGLE or _selection_mode == GTK_SELECTION_BROWSE)
            _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(_row_list_store->operator GListModel *()));
        else if (_selection_mode == GTK_SELECTION_NONE)
            _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(_row_list_store->operator GListModel *()));

        _native = GTK_COLUMN_VIEW(gtk_column_view_new(_selection_model));

        for (size_t i = 0; i < titles.size(); ++i)
        {
            _column_factories.push_back(new ColumnView::ColumnFactory(i));
            static std::vector<GtkColumnViewColumn*> temp;

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
        _row_list_store->append(widgets);
    }

    void ColumnView::insert_row(size_t i, std::vector<Widget*> widgets)
    {
       _row_list_store->insert(i, widgets);
    }

    void ColumnView::remove_row(size_t i)
    {
        _row_list_store->remove(i);
    }

    void ColumnView::set_columns_reorderable(bool b)
    {
        gtk_column_view_set_reorderable(_native, b);
    }

    void ColumnView::set_enable_rubberband_selection(bool b)
    {
        gtk_column_view_set_enable_rubberband_selection(_native, b);
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
        if (b)
            std::cerr << "[WARNING] In ColumnView::set_column_resizable: resizing right-mose column to it's smallest size may lead to freeze. This feature is not safe to use." << std::endl;

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

    size_t ColumnView::get_n_rows() const
    {
        return g_list_model_get_n_items(_row_list_store->operator GListModel *());
    }

    void ColumnView::move_row_to(size_t to_move, size_t new_position)
    {
        auto widgets = get_widgets_in_row(to_move);

        _row_list_store->remove(to_move);
        _row_list_store->insert(new_position, widgets);
    }

    std::vector<Widget*> ColumnView::get_widgets_in_row(size_t i)
    {
        auto* item = _row_list_store->at(i);

        std::vector<Widget*> widgets;
        for (size_t i = 0; i < item->widgets->size(); ++i)
            widgets.push_back(item->widgets->at(i));

        return widgets;
    }

    std::vector<Widget*> ColumnView::get_widgets_in_column(size_t column_i)
    {
        std::vector<Widget*> out;

        for (size_t i = 0; i < _row_list_store->get_n_items(); ++i)
        {
            auto* item = _row_list_store->at(i);
            out.push_back(item->widgets->at(column_i));
        }

        return out;
    }

    Widget* ColumnView::get_widget_at(size_t row_i, size_t col_i)
    {
        return _row_list_store->at(row_i)->widgets->at(col_i);
    }

    void ColumnView::set_widget_at(size_t row_i, size_t col_i, Widget* widget)
    {
        _row_list_store->at(row_i)->widgets->at(col_i) = widget;
    }
}

