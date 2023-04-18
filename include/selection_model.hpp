//
// Copyright 2022 Clemens Cords
// Created on 9/30/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief selection mode of a container widget that has selectable children
    enum class SelectionMode
    {
        /// @brief it is impossible to select any items
        NONE = GTK_SELECTION_NONE,

        /// @brief 0 or one item is selected at any time
        SINGLE = GTK_SELECTION_SINGLE,

        /// @brief 0 or more items are selected at any tiem
        MULTIPLE = GTK_SELECTION_MULTIPLE,
    };

    /// @brief selection model, provides interface and signals for selectable widgets
    class SelectionModel : public SignalEmitter,
        HAS_SIGNAL(SelectionModel, selection_changed)
    {
        public:
            /// @brief construct from GtkSelectionModel, for interal use only. Use <tt>get_selection_model</tt> to acquire a selection model from a selectable widget
            SelectionModel(GtkSelectionModel*);

            /// @brief default ctor deleted. Use <tt>get_selection_model</tt> to acquire a selection model from a selectable widget
            SelectionModel() = delete;

            /// @brief destruct
            ~SelectionModel();

            /// @brief expose as GtkSelectionModel \internal
            operator GtkSelectionModel*() const;

            /// @brief expose as GObject \internal
            operator GObject*() const override;

            /// @brief get indices of selected items
            /// @return vector of indices, may be empty
            [[nodiscard]] std::vector<size_t> get_selection();

            /// @brief select all items or, if selection mode is single, select the first item
            void select_all();

            /// @brief unselect all itmes
            void unselect_all();

            /// @brief select item at specific index
            /// @param i index
            /// @param unselect_others if selection mode is multiple, should all other items be unselected
            void select(size_t i, bool unselect_others = true);

            /// @brief unselect item at specific item
            /// @param i index
            void unselect(size_t i);

        protected:
            GtkSelectionModel* _native;
    };

    /// @brief selection model implementation for mousetrap::SelectionMode::MULTIPLE
    struct MultiSelectionModel : public SelectionModel
    {
        /// @brief construct, for internal us only
        MultiSelectionModel(GListModel*);
    };

    /// @brief selection model implementation for mousetrap::SelectionMode::SINGLE
    struct SingleSelectionModel : public SelectionModel
    {
        /// @brief construct, for internal us only
        SingleSelectionModel(GListModel*);
    };

    /// @brief selection model implementation for mousetrap::SelectionMode::NONE
    struct NoSelectionModel : public SelectionModel
    {
        /// @brief construct, for internal us only
        NoSelectionModel(GListModel*);
    };
}

