// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/label.hpp>
#include <mousetrap/signal_component.hpp>

#include <iostream>  // TODO

namespace mousetrap
{
    #ifndef DOXYGEN
    class DropDown;
    namespace detail
    {
        struct _DropDownInternal
        {
            GObject parent;

            GtkDropDown* native;

            GtkSignalListItemFactory* list_factory;
            GtkSignalListItemFactory* label_factory;
            GListStore* model;
        };
        using DropDownInternal = _DropDownInternal;
        DEFINE_INTERNAL_MAPPING(DropDown);
    }
    #endif

    /// @brief drop down menu, present a popup with a list of items
    /// \signals
    /// \widget_signals{DropDown}
    class DropDown : public Widget
    {
        public:
            /// @brief id of an item, keep track of this to refer to items after initialization
            using ItemID = size_t;

            /// @brief construct as empty
            DropDown();

            /// @brief construct from internal
            /// @param internal
            DropDown(detail::DropDownInternal*);

            /// @brief destructor
            ~DropDown();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add an item with an accompanying action to the end of the drop down
            /// @tparam Function_t function or lambda with signature `(DropDown&, (Data_t)) -> void`
            /// @tparam Data_t arbitrary data type
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @param data
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t, typename Data_t>
            [[nodiscard]] ItemID push_back(const Widget& list_widget, const Widget& label_widget, Function_t function, Data_t data);

            /// @brief add an item with an accompanying action to the end of the drop down
            /// @tparam Function_t function or lambda with signature `(DropDown&) -> void`
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t>
            [[nodiscard]] ItemID push_back(const Widget& list_widget, const Widget& label_widget, Function_t function);

            /// @brief add an item with an accompanying action to the start of the drop down
            /// @tparam Function_t function or lambda with signature `(DropDown&, (Data_t)) -> void`
            /// @tparam Data_t arbitrary data type
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @param data
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t, typename Data_t>
            [[nodiscard]] ItemID push_front(const Widget& list_widget, const Widget& label_widget, Function_t function, Data_t data);

            /// @brief add an item with an accompanying action to the start of the drop down
            /// @tparam Function_t function or lambda with signature `(DropDown&) -> void`
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t>
            [[nodiscard]] ItemID push_front(const Widget& list_widget, const Widget& label_widget, Function_t function);

            /// @brief add an item with an accompanying action to a specified position in the drop down
            /// @tparam Function_t function or lambda with signature `(DropDown&, (Data_t)) -> void`
            /// @tparam Data_t arbitrary data type
            /// @param i index
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @param data
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t, typename Data_t>
            [[nodiscard]] ItemID insert(size_t i, const Widget& list_widget, const Widget& label_widget, Function_t function, Data_t data);

            /// @brief add an item with an accompanying action to a specified position in the drop down
            /// @tparam Function_t function or lambda with signature `(DropDown&) -> void`
            /// @param i index
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t>
            [[nodiscard]] ItemID insert(size_t i, const Widget& list_widget, const Widget& label_widget, Function_t function);

            /// @brief remove an item from the drop down
            /// @param id
            void remove(ItemID id);

            /// @brief set whether a downward arrow should be shown next to the currently active label widget
            /// @param b
            void set_always_show_arrow(bool b);

            /// @brief get whether a downward arrow is shown next to the currently active label widget
            /// @return bool
            bool get_always_show_arrow() const;

            /// @brief set currently selected item
            /// @param id
            void set_selected(ItemID id);

            /// @brief get the id of the currently selected item
            /// @return id
            DropDown::ItemID get_selected() const;

            /// @brief get the id of the item at specified position in the drop down
            /// @param index
            /// @return id
            ItemID get_item_at(size_t index) const;

        private:
            static void on_list_factory_bind(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal*);
            static void on_label_factory_bind(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal*);

            static void on_list_factory_teardown(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal*);
            static void on_label_factory_teardown(GtkSignalListItemFactory* self, void* object, detail::DropDownInternal*);

            static void on_selected_item_changed(GtkDropDown*, void*, detail::DropDownInternal* internal);

            detail::DropDownInternal* _internal = nullptr;

            static inline size_t _current_id = 0;
            bool assert_label_is_not_self(const std::string& scope, const Widget&, const Widget&);
    };
}

#include "inline/drop_down.inl"
