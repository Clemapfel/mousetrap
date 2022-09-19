// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Button : public WidgetImplementation<GtkButton>
    {
        public:
            Button();

            void set_has_frame(bool b);
            void set_child(Widget*);

            template<typename T>
            using OnClickedFunction_t = std::function<void(Button*, T)>;

            // using std::function
            template<typename T>
            void connect_signal_clicked(OnClickedFunction_t<T>* f, T data);

            // bind lambda
            template<typename Function_t, typename T> requires (not std::is_pointer_v<Function_t>)
            void connect_signal_clicked(Function_t f, T data);

            // bind c function ref
            template<typename T>
            void connect_signal_clicked(void(&f)(Button*, T), T);

        private:
            OnClickedFunction_t<void*> _on_clicked_f;
            void* _on_clicked_data;

            static void on_clicked_wrapper(GtkButton*, Button*);
    };
}

#include <src/button.inl>

namespace mousetrap
{
    template<typename T>
    void Button::connect_signal_clicked(OnClickedFunction_t<T>* f, T data)
    {
        _on_clicked_f = OnClickedFunction_t<void*>(*((OnClickedFunction_t<void*>*) f));
        _on_clicked_data = data;

        connect_signal("clicked", on_clicked_wrapper, this);
    }

    template<typename Function_t, typename T> requires (not std::is_pointer_v<Function_t>)
    void Button::connect_signal_clicked(Function_t f, T data)
    {
        auto wrapped = std::function<void(Button*, T)>(f);
        connect_signal_clicked(&wrapped, data);
    }

    template<typename T>
    void Button::connect_signal_clicked(void(&f)(Button*, T), T data)
    {
        auto wrapped = std::function<void(Button*, T)>(f);
        connect_signal_clicked(&wrapped, data);
    }

    void Button::on_clicked_wrapper(GtkButton*, Button* instance)
    {
        if (instance->_on_clicked_f == nullptr)
            return;

        (instance->_on_clicked_f)(instance, instance->_on_clicked_data);
    }


}