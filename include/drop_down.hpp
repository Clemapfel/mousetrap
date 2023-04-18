// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/label.hpp>
#include <include/signal_component.hpp>

#include <iostream>  // TODO

namespace mousetrap {
    /// @brief drop down menu, present a popup with a list of items
    class DropDown : public WidgetImplementation<GtkDropDown>
    {
        public:
            /// @brief id of an item, keep track of this to refer to items after initialization
            using ItemID = struct
            {
                size_t _value;
            };

            /// @brief construct as empty
            DropDown();

            /// @brief destructor
            ~DropDown();

            /// @brief add an item with an accompanying action to the end of the drop down
            /// @tparam Function_t function or lambda with signature <tt>(Data_t) -> void</tt>
            /// @tparam Data_t arbitrary data type
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @param data
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t, typename Data_t>
            [[nodiscard]] ItemID push_back(Widget* list_widget, Widget* label_widget, Function_t, Data_t);

            /// @brief add an item with an accompanying action to the end of the drop down
            /// @tparam Function_t function or lambda with signature <tt>() -> void</tt>
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t>
            [[nodiscard]] ItemID push_back(Widget* list_widget, Widget* label_widget, Function_t);

            /// @brief add an item with an accompanying action to the start of the drop down
            /// @tparam Function_t function or lambda with signature <tt>(Data_t) -> void</tt>
            /// @tparam Data_t arbitrary data type
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @param data
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t, typename Data_t>
            [[nodiscard]] ItemID push_front(Widget* list_widget, Widget* label_widget, Function_t, Data_t);

            /// @brief add an item with an accompanying action to the start of the drop down
            /// @tparam Function_t function or lambda with signature <tt>() -> void</tt>
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t>
            [[nodiscard]] ItemID push_front(Widget* list_widget, Widget* label_widget, Function_t);

            /// @brief add an item with an accompanying action to a specified position in the drop down
            /// @tparam Function_t function or lambda with signature <tt>(Data_t) -> void</tt>
            /// @tparam Data_t arbitrary data type
            /// @param i index
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @param data
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t, typename Data_t>
            [[nodiscard]] ItemID insert(size_t i, Widget* list_widget, Widget* label_widget, Function_t, Data_t);

            /// @brief add an item with an accompanying action to a specified position in the drop down
            /// @tparam Function_t function or lambda with signature <tt>() -> void</tt>
            /// @param i index
            /// @param list_widget widget to display when the itme is selected and the drop down popover is closed
            /// @param label_widget widget to display for that item inside the drop down popoever
            /// @param function function to trigger when the item is selected
            /// @return item id, keep track of this to be able to refer to the item later
            template <typename Function_t>
            [[nodiscard]] ItemID insert(size_t i, Widget* list_widget, Widget* label_widget, Function_t);

            /// @brief remove an item from the drop down
            /// @param id
            void remove(ItemID);

            /// @brief set whether a downward arrow should be shown next to the currently active label widget
            /// @param b
            void set_show_arrow(bool);

            /// @brief get whether a downward arrow is shown next to the currently active label widget
            /// @return bool
            bool get_show_arrow() const;

            /// @brief set currently selected item
            /// @param id
            void set_selected(ItemID);

            /// @brief get the id of the currently selected item
            /// @return id
            ItemID get_selected() const;

            /// @brief get the id of the item at specified position in the drop down
            /// @param index
            /// @return id
            ItemID get_item_at(size_t index) const;

        private:
            static void on_list_factory_bind(GtkSignalListItemFactory* self, void* object, DropDown*);
            static void on_label_factory_bind(GtkSignalListItemFactory* self, void* object, DropDown*);

            static void on_list_factory_unbind(GtkSignalListItemFactory* self, void* object, DropDown*);
            static void on_label_factory_unbind(GtkSignalListItemFactory* self, void* object, DropDown*);

            static void on_selected_item_changed(GtkDropDown*, void*, DropDown*);

            GtkSignalListItemFactory* _list_factory;
            GtkSignalListItemFactory* _label_factory;
            GListStore* _model;

            static inline size_t _current_id = 0;
            void assert_label_is_not_self(const std::string& scope, Widget*, Widget*);
    };
}

#include <src/drop_down.inl>
