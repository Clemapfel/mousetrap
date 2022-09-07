// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Window::Window()
        : Window(GTK_WINDOW(gtk_window_new()))
    {}

    Window::Window(GtkWindow* window)
    {
        _native = window;

        if (_global_shortcut_controller == nullptr)
        {
            _global_shortcut_controller = new KeyEventController();
            _global_shortcut_controller->connect_key_pressed(on_key_pressed);
        }

        add_controller(_global_shortcut_controller);
    }

    Window::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void Window::present()
    {
        gtk_window_present(_native);
    }

    void Window::set_maximized(bool b)
    {
        if (b)
            gtk_window_maximize(_native);
        else
            gtk_window_unmaximize(_native);
    }

    void Window::set_fullscreen(bool b )
    {
        if (b)
            gtk_window_fullscreen(_native);
        else
            gtk_window_unfullscreen(_native);
    }

    void Window::set_child(Widget* widget)
    {
        gtk_window_set_child(_native, widget->operator GtkWidget *());
    }

    void Window::remove_child()
    {
        gtk_window_set_child(_native, nullptr);
    }

    void Window::set_focused_widget(Widget* widget)
    {
        gtk_window_set_focus(_native, widget->operator GtkWidget*());
    }

    gboolean Window::on_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, void*)
    {
        std::vector<std::pair<ShortcutID, GtkShortcutTrigger*>> triggered;
        for (auto& tuple : _global_shortcuts)
        {
            auto* event = gtk_event_controller_get_current_event(GTK_EVENT_CONTROLLER(self));
            if (GDK_IS_EVENT(event))
            {
                GdkModifierType modifier = gdk_event_get_modifier_state(event);
                bool shift = modifier & GdkModifierType::GDK_SHIFT_MASK;
                bool alt = modifier & GdkModifierType::GDK_ALT_MASK;
                bool control = modifier & GdkModifierType::GDK_CONTROL_MASK;

                if (gtk_shortcut_trigger_trigger(tuple.trigger, event, false))
                {
                    tuple.action(tuple.argument);
                    triggered.emplace_back(tuple.id, tuple.trigger);
                }
            }
        }

        if (triggered.size() > 1)
        {
            std::cerr << "[WARNING] In Window::on_key_pressed: The actions ";
            for (size_t i = 0; i < triggered.size(); ++i)
                std::cerr << "\"" << triggered.at(i).first << "\"" << (i != triggered.size() - 1 ? " and " : "");

            std::cerr << " have the same global shortcut trigger: \"" << gtk_shortcut_trigger_to_string(triggered.front().second) << "\"" << std::endl;
        }

        return true;
    }

    void Window::register_global_shortcut(ShortcutMap* map, const std::string& shortcut_id, std::function<void(void*)> action, void* data)
    {
        auto it = map->_bindings.find(shortcut_id);
        if (it == map->_bindings.end())
        {
            std::cerr << "[ERROR] In Window::register_global_shortcut: Not shortcut trigger binding with id " << shortcut_id << " registered" << std::endl;
            return;
        }

        _global_shortcuts.push_back({
            shortcut_id,
            it->second,
            action,
            data
        });

        std::cout << "registered " << shortcut_id << ": " << gtk_shortcut_trigger_to_string(it->second) << std::endl;
    }

    void Window::unregister_global_shortcut(const std::string& shortcut_id)
    {
        for (size_t i = 0; i < _global_shortcuts.size(); ++i)
        {
            if (_global_shortcuts.at(i).id == shortcut_id)
            {
                delete _global_shortcuts.at(i).argument;
                _global_shortcuts.erase(_global_shortcuts.begin() + i);
            }
        }
    }
}