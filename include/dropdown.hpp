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
    namespace detail {
        static void drop_down_item_finalize (GObject *object);
    }

    class DropDown : public WidgetImplementation<GtkDropDown>, public HasActivateSignal<DropDown>
    {
        friend struct _DropDownItem;
        friend void detail::drop_down_item_finalize (GObject *object);

        public:
            DropDown();
            template<typename Function_t, typename T>
            void push_back(
                Widget* list_widget,
                Widget* when_selected_label_widget,
                Function_t on_select_f,
                T on_select_data
            );

            template<typename Function_t, typename T>
            void push_back(
                Widget* list_widget,
                Widget* when_selected_label_widget,
                Function_t on_select_f
            );

            void set_selected(size_t);
            size_t get_selected();

            void set_signal_selection_blocked(bool b);

        private:
            static void on_list_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void on_label_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void noop_item_function(void*);

            GtkSignalListItemFactory* _list_factory;
            GtkSignalListItemFactory* _label_factory;
            GListStore* _model;

            static inline size_t _current_function_id = 0;
            std::map<size_t, std::function<void()>> _functions = {};

            bool* _activation_blocked = new bool(false);
    };
}

#include <src/dropdown.inl>

