// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    inline SelectionModel::operator GObject*()
    {
        return G_OBJECT(operator GtkSelectionModel*());
    }

    inline SelectionModel::operator GtkSelectionModel*()
    {
        return _native;
    }

    inline SelectionModel::SelectionModel(GtkSelectionModel* model)
            : signals::SelectionChanged<SelectionModel>(this), _native(model)
    {}

    inline std::vector<size_t> SelectionModel::get_selection()
    {
        auto* bitset = gtk_selection_model_get_selection(operator GtkSelectionModel*());
        std::vector<size_t> out;
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            out.push_back(gtk_bitset_get_nth(bitset, i));

        return out;
    }

    inline void SelectionModel::select_all()
    {
        gtk_selection_model_select_all(operator GtkSelectionModel*());
    }

    inline void SelectionModel::unselect_all()
    {
        gtk_selection_model_unselect_all(operator GtkSelectionModel*());
    }

    inline void SelectionModel::select(size_t i, bool unselect_others)
    {
        gtk_selection_model_select_item(operator GtkSelectionModel*(), i, unselect_others);
    }

    inline void SelectionModel::unselect(size_t i)
    {
        gtk_selection_model_unselect_item(operator GtkSelectionModel*(), i);
    }

    inline MultiSelectionModel::MultiSelectionModel(GListModel* model)
            : SelectionModel(GTK_SELECTION_MODEL(gtk_multi_selection_new(model)))
    {}

    inline SingleSelectionModel::SingleSelectionModel(GListModel* model)
            : SelectionModel(GTK_SELECTION_MODEL(gtk_single_selection_new(model)))
    {}

    inline NoSelectionModel::NoSelectionModel(GListModel* model)
            : SelectionModel(GTK_SELECTION_MODEL(gtk_no_selection_new(model)))
    {}
}