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
        for (auto& tuple : _global_shortcuts)
            if (gtk_shortcut_trigger_trigger(tuple.trigger, gtk_event_controller_get_current_event(GTK_EVENT_CONTROLLER(self)), false))
               tuple.action(tuple.argument);
    }

    void Window::register_global_shortcut(ShortcutMap* map, const std::string& shortcut_id, std::function<void(void*)> action, void* data)
    {
        _global_shortcuts.push_back({
            shortcut_id,
            map->_bindings.at(shortcut_id),
            action,
            data
        });
    }

    void Window::unregister_global_shortcut(const std::string& shortcut_id)
    {
        for (size_t i = 0; i < _global_shortcuts.size(); ++i)
            if (_global_shortcuts.at(i).id == shortcut_id)
                _global_shortcuts.erase(_global_shortcuts.begin() + i);
    }
}