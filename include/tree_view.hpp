// 
// Copyright 2022 Clemens Cords
// Created on 9/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <vector>

namespace mousetrap
{
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
}

// ###

namespace mousetrap
{
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
}