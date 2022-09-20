// 
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gdk/gdk.h>
#include <include/widget.hpp>
#include <include/shortcut_map.hpp>

namespace mousetrap
{
    class Window : public WidgetImplementation<GtkWindow>, public ShortcutMap
    {
        public:
            Window();
            Window(GtkWindow*);

            void set_maximized(bool);
            void set_fullscreen(bool);

            void present();

            void set_child(Widget*);
            void remove_child();

            void set_focused_widget(Widget*);

            void register_global_shortcut(ShortcutMap*, const std::string& shortcut_id, std::function<void(void*)>, void*);
            void unregister_global_shortcut(const std::string& shortcut_id);

        private:
            using ShortcutID = std::string;

            struct GlobalShortcut
            {
                ShortcutID id;
                GtkShortcutTrigger* trigger;
                std::function<void(void*)> action;
                void* argument;
                bool shift = false;
                bool control = false;
                bool alt = false;
            };

            static inline std::vector<GlobalShortcut> _global_shortcuts;

            static bool on_key_pressed(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, void*);
            static inline KeyEventController* _global_shortcut_controller = nullptr;
    };
}

#include <src/window.inl>