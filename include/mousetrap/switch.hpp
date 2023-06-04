//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/signal_component.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Switch;
    namespace detail
    {
        using SwitchInternal = GtkSwitch;
        DEFINE_INTERNAL_MAPPING(Switch);
    }
    #endif

    /// @brief switch, can be click dragged or clicked to change a binary state
    /// \signals
    /// \signal_activate{Switch}
    /// \widget_signals{Switch}
    class Switch : public Widget,
        HAS_SIGNAL(Switch, activate)
    {
        public:
            /// @brief construct
            Switch();

            /// @brief create from internal
            Switch(detail::SwitchInternal*);

            /// @brief destructor
            ~Switch();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief get whether the switch is in the "on" position
            /// @return true if "on", false otherwise
            bool get_is_active() const;

            /// @brief set whether the switch is in the "on" position
            /// @param b true if "on", false otherwise
            void set_active(bool);

        private:
            detail::SwitchInternal* _internal = nullptr;
    };
}


