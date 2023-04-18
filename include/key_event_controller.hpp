//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/18/23
//

#pragma once

#include <include/event_controller.hpp>
#include <include/action.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief event controller that handles keystrokes
    class KeyEventController : public EventController,
        HAS_SIGNAL(KeyEventController, key_pressed),
        HAS_SIGNAL(KeyEventController, key_released),
        HAS_SIGNAL(KeyEventController, modifiers_changed)
    {
        public:
            /// @brief construct, needs to be connected to widget to start emitting events
            KeyEventController();

            /// @brief test whether the current event of the controller is consistent with the shortcut identified via the argument
            /// @param shortcut shortcut trigger as string
            bool should_shortcut_trigger_trigger(const ShortcutTriggerID& shortcut);

            /// @brief test whether shift is pressed in the given modifier state
            /// @param modifier state, obtained during signal emission of key_pressed, key_released or modifiers_changed
            /// @return true if pressed, false otherwise
            static bool shift_pressed(ModifierState);

            /// @brief test whether control is pressed in the given modifier state
            /// @param modifier state, obtained during signal emission of key_pressed, key_released or modifiers_changed
            /// @return true if pressed, false otherwise
            static bool control_pressed(ModifierState);

            /// @brief test whether alt is pressed in the given modifier state
            /// @param modifier state, obtained during signal emission of key_pressed, key_released or modifiers_changed
            /// @return true if pressed, false otherwise
            static bool alt_pressed(ModifierState);

            /// @brief test whether left mouse button is pressed in the given modifier state
            /// @param modifier state, obtained during signal emission of key_pressed, key_released or modifiers_changed
            /// @return true if pressed, false otherwise
            static bool mouse_button_01_pressed(ModifierState);

            /// @brief test whether right mouse button is pressed in the given modifier state
            /// @param modifier state, obtained during signal emission of key_pressed, key_released or modifiers_changed
            /// @return true if pressed, false otherwise
            static bool mouse_button_02_pressed(ModifierState);
    };
}
