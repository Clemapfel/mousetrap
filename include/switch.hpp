// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    class Switch : public WidgetImplementation<GtkSwitch>,
            public HasStateSetSignal<Switch>
    {
        public:
            Switch();

            bool get_active();
            void set_active(bool);

            enum class State : bool
            {
                ON = true,
                OFF = false
            };

            // only changes visual indicator, on = highlighted
            void set_state(State);
            State get_state();

        private:
            static void synch_state_on_activate(GtkSwitch* instance);
    };
}

//

namespace mousetrap
{
    Switch::Switch()
        : WidgetImplementation<GtkSwitch>(GTK_SWITCH(gtk_switch_new())),
          HasStateSetSignal<Switch>(this)
   {}

    bool Switch::get_active()
    {
        return gtk_switch_get_active(get_native());
    }

    void Switch::set_active(bool b)
    {
        gtk_switch_set_active(get_native(), b);
    }

    void Switch::set_state(State state)
    {
        gtk_switch_set_state(get_native(), (bool) state);
    }

    Switch::State Switch::get_state()
    {
        return (State) gtk_switch_get_state(get_native());
    }
}