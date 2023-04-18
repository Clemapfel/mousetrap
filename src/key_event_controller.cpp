//
// Created by clem on 3/18/23.
//

#include <include/key_event_controller.hpp>

namespace mousetrap
{
    KeyEventController::KeyEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_key_new())),
          CTOR_SIGNAL(KeyEventController, key_pressed),
          CTOR_SIGNAL(KeyEventController, key_released),
          CTOR_SIGNAL(KeyEventController, modifiers_changed)
    {}

    bool KeyEventController::should_shortcut_trigger_trigger(const ShortcutTriggerID& shortcut)
    {
        auto* trigger = gtk_shortcut_trigger_parse_string(shortcut.c_str());
        auto* event = gtk_event_controller_get_current_event(_native);
        bool out = gtk_shortcut_trigger_trigger(trigger, event, false);
        g_object_unref(trigger);
        return out;
    };

    bool KeyEventController::shift_pressed(ModifierState state)
    {
        return state & GDK_SHIFT_MASK;
    }

    bool KeyEventController::alt_pressed(ModifierState state)
    {
        return state & GDK_ALT_MASK;
    }

    bool KeyEventController::control_pressed(ModifierState state)
    {
        return state & GDK_CONTROL_MASK;
    }

    bool KeyEventController::mouse_button_01_pressed(ModifierState state)
    {
        return state & GDK_BUTTON1_MASK;
    }

    bool KeyEventController::mouse_button_02_pressed(ModifierState state)
    {
        return state & GDK_BUTTON2_MASK;
    }
}
