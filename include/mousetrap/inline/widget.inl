//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/17/23
//

#include <mousetrap/frame_clock.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap::detail
{
    // const casting like this is awful style but it is necessary to make the definition of COMPOUND_WIDGET_REQUIRED_FUNCTION
    // have the signature () -> Widget&. Creating a compound widget should require as little knowledge about C++ as possible

    CompoundWidgetSuper::operator Widget&() const
    {
        return const_cast<Widget&>(const_cast<CompoundWidgetSuper*>(this)->COMPOUND_WIDGET_REQUIRED_FUNCTION());
    }

    CompoundWidgetSuper::operator const Widget&() const
    {
        return const_cast<CompoundWidgetSuper*>(this)->COMPOUND_WIDGET_REQUIRED_FUNCTION();
    };

    CompoundWidgetSuper::operator Widget&()
    {
        return const_cast<Widget&>(COMPOUND_WIDGET_REQUIRED_FUNCTION());
    }

    CompoundWidgetSuper::operator const Widget&()
    {
        return const_cast<Widget&>(COMPOUND_WIDGET_REQUIRED_FUNCTION());
    }
}

namespace mousetrap
{
    template<typename Function_t, typename Data_t>
    void Widget::set_tick_callback(Function_t f_in, Data_t data_in)
    {
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