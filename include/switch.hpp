//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/signal_component.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief switch, can be click dragged or clicked to change a binary state
    class Switch : public WidgetImplementation<GtkSwitch>,
        HAS_SIGNAL(Switch, activate)
    {
        public:
            /// @brief construct
            Switch();

            /// @brief get whether the switch is in the "on" position
            /// @return true if "on", false otherwise
            bool get_active() const;

            /// @brief set whether the switch is in the "on" position
            /// @param b true if "on", false otherwise
            void set_active(bool);
    };
}


