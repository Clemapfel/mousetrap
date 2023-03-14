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

#include <src/switch.inl>
