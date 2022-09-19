// 
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/label.hpp>

namespace mousetrap
{
    class DropDown : public WidgetImplementation<GtkDropDown>
    {
        public:
            DropDown();

            // push back and create label widget as Label(id)
            using OnSelectSignature = void (*)(void*);

            void push_back(
                    Widget*,
                    const std::string& id,
                    OnSelectSignature on_select_f = nullptr,
                    void* on_select_data = nullptr
            );

            void push_back(
                    Widget* list_widget,
                    Widget* when_selected_label_widget,
                    OnSelectSignature on_select_f = nullptr,
                    void* on_select_data = nullptr
            );

            template<typename T>
            using on_activate_function_t = void(DropDown*, T);

            template<typename Function_t, typename T>
            void connect_signal_activate(Function_t, T);

        private:
            static void on_activate_wrapper(GtkDropDown*, DropDown* instance);

            std::function<on_activate_function_t<void*>> _on_activate_f;
            void* _on_activate_data;

            static void on_list_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void on_label_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void noop_item_function(void*);

            GtkSignalListItemFactory* _list_factory;
            GtkSignalListItemFactory* _label_factory;
            GListStore* _model;
    };
}

#include <src/dropdown.inl>
