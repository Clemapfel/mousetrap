//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/17/23
//

#include <include/frame_clock.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    template<typename T>
    WidgetImplementation<T>::WidgetImplementation(T* in)
        : _native(GTK_WIDGET(in))
    {
        if (not GTK_IS_WIDGET(in))
            log::fatal("In WidgetImplementation::WidgetImplementation(T*): Object is not a widget");

        g_object_ref(_native);
    }

    template<typename T>
    WidgetImplementation<T>::~WidgetImplementation()
    {
        if (_native != nullptr)
            g_object_unref(_native);
    }

    template<typename T>
    WidgetImplementation<T>::operator T*() const
    {
        return (T*) _native;
    }

    template<typename T>
    WidgetImplementation<T>::operator GtkWidget*() const
    {
        return _native;
    }

    template<typename T>
    T* WidgetImplementation<T>::get_native() const
    {
        return (T*) _native;
    }

    template<typename T>
    void WidgetImplementation<T>::override_native(GtkWidget* new_native)
    {
        auto* old_native = _native;
        _native = g_object_ref(new_native);
        g_object_unref(old_native);
    }

    template<typename T>
    WidgetImplementation<T>::WidgetImplementation(WidgetImplementation<T>&& other)
    {
        _native = other._native;
        other._native = nullptr;
    }

    template<typename T>
    WidgetImplementation<T>& WidgetImplementation<T>::operator=(WidgetImplementation<T>&& other)
    {
        _native = other._native;
        other._native = nullptr;
        return *this;
    }

    template<typename Function_t, typename Data_t>
    void Widget::set_tick_callback(Function_t f_in, Data_t data_in)
    {
        initialize();
        remove_tick_callback();
        _internal->tick_callback = [f = f_in, data = data_in](GdkFrameClock* clock) -> TickCallbackResult {
            return f(FrameClock(clock), data);
        };

        _internal->tick_callback_id = gtk_widget_add_tick_callback(
            operator GtkWidget*(),
            (GtkTickCallback) G_CALLBACK(tick_callback_wrapper),
            _internal,
            nullptr
        );
    }

    template<typename Function_t>
    void Widget::set_tick_callback(Function_t f_in)
    {
        initialize();
        remove_tick_callback();
        _internal->tick_callback = [f = f_in](GdkFrameClock* clock) -> TickCallbackResult {
            return f(FrameClock(clock));
        };

        _internal->tick_callback_id = gtk_widget_add_tick_callback(
            operator GtkWidget*(),
            (GtkTickCallback) G_CALLBACK(tick_callback_wrapper),
            _internal,
            nullptr
        );
    }
}