//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <include/selection_model.hpp>

namespace mousetrap
{
    SelectionModel::operator GObject*() const
    {
        return G_OBJECT(operator GtkSelectionModel*());
    }

    SelectionModel::operator GtkSelectionModel*() const
    {
        return _native;
    }

    SelectionModel::SelectionModel(GtkSelectionModel* model)
        : _native(model), CTOR_SIGNAL(SelectionModel, selection_changed)
    {
        g_object_ref(_native);
    }

    SelectionModel::~SelectionModel()
    {
        g_object_unref(_native);
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

    MultiSelectionModel::MultiSelectionModel(GListModel* model)
            : SelectionModel(GTK_SELECTION_MODEL(gtk_multi_selection_new(model)))
    {}

    SingleSelectionModel::SingleSelectionModel(GListModel* model)
            : SelectionModel(GTK_SELECTION_MODEL(gtk_single_selection_new(model)))
    {}

    NoSelectionModel::NoSelectionModel(GListModel* model)
            : SelectionModel(GTK_SELECTION_MODEL(gtk_no_selection_new(model)))
    {}
}