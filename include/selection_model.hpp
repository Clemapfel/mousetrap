// 
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    class SelectionModel : public SignalEmitter,
            public HasSelectionChangedSignal<SelectionModel>
    {
        public:
            operator GtkSelectionModel*();
            operator GObject*() override;

            std::vector<size_t> get_selection();
            void select_all();
            void unselect_all();

            void select(size_t i, bool unselect_others = true);
            void unselect(size_t i);

        protected:
            SelectionModel(GtkSelectionModel*);

            GtkSelectionModel* _native;
    };

    struct MultiSelectionModel : public SelectionModel
    {
        MultiSelectionModel(GListModel*);
    };

    struct SingleSelectionModel : public SelectionModel
    {
        SingleSelectionModel(GListModel*);
    };

    struct NoSelectionModel : public SelectionModel
    {
        NoSelectionModel(GListModel*);
    };
}

//#include <src/selection_model.hpp>

namespace mousetrap
{
    SelectionModel::operator GObject*()
    {
        return G_OBJECT(operator GtkSelectionModel*());
    }

    SelectionModel::operator GtkSelectionModel*()
    {
        return _native;
    }

    SelectionModel::SelectionModel(GtkSelectionModel* model)
        : _native(model), HasSelectionChangedSignal<SelectionModel>(this)
    {}

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