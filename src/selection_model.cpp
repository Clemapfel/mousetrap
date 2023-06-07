//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/selection_model.hpp>

namespace mousetrap
{
    SelectionModel::operator NativeObject() const
    {
        return G_OBJECT(operator GtkSelectionModel*());
    }

    SelectionModel::operator GtkSelectionModel*() const
    {
        return GTK_SELECTION_MODEL(_internal);
    }

    SelectionModel::SelectionModel(detail::SelectionModelInternal* internal)
        : _internal(internal),
          CTOR_SIGNAL(SelectionModel, selection_changed)
    {
        g_object_ref(_internal);
    }

    SelectionModel::SelectionModel(SelectionMode mode, GListModel* model)
        : SelectionModel([&]() -> detail::SelectionModelInternal*
        {
            if (mode == SelectionMode::SINGLE)
                return GTK_SELECTION_MODEL(gtk_single_selection_new(model));
            else if (mode == SelectionMode::MULTIPLE)
                return GTK_SELECTION_MODEL(gtk_single_selection_new(model));
            else
                return GTK_SELECTION_MODEL(gtk_multi_selection_new(model));
        }())
    {}

    SelectionModel::~SelectionModel()
    {
        g_object_unref(_internal);
    }

    NativeObject SelectionModel::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    std::vector<size_t> SelectionModel::get_selection()
    {
        auto* bitset = gtk_selection_model_get_selection(operator GtkSelectionModel*());
        std::vector<size_t> out;
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            out.push_back(gtk_bitset_get_nth(bitset, i));

        return out;
    }

    void SelectionModel::select_all()
    {
        gtk_selection_model_select_all(operator GtkSelectionModel*());
    }

    void SelectionModel::unselect_all()
    {
        gtk_selection_model_unselect_all(operator GtkSelectionModel*());
    }

    void SelectionModel::select(size_t i, bool unselect_others)
    {
        gtk_selection_model_select_item(operator GtkSelectionModel*(), i, unselect_others);
    }

    void SelectionModel::unselect(size_t i)
    {
        gtk_selection_model_unselect_item(operator GtkSelectionModel*(), i);
    }
}