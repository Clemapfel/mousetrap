// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/label.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    class DropDown : public WidgetImplementation<GtkDropDown>, public HasActivateSignal<DropDown>
    {
        public:
            DropDown();

            // push back and create label widget as Label(id)

            template<typename T>
            using on_select_function_t = void (*)(void*);

            template<typename Function_t, typename T>
            void push_back(
                Widget* list_widget,
                Widget* when_selected_label_widget,
                Function_t on_select_f = nullptr,
                T on_select_data = nullptr
            );

            void set_selected(size_t);
            size_t get_selected();

        private:
            static void on_list_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void on_label_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void noop_item_function(void*);

            GtkSignalListItemFactory* _list_factory;
            GtkSignalListItemFactory* _label_factory;
            GListStore* _model;
    };
}

#include <src/dropdown.inl>
