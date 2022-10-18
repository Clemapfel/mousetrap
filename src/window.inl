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
        : WidgetImplementation<GtkWindow>(window)
    {
        if (_global_shortcut_controller == nullptr)
        {
            _global_shortcut_controller = new KeyEventController();
            _global_shortcut_controller->connect_signal_key_pressed(on_key_pressed, nullptr);
        }

        add_controller(_global_shortcut_controller);
    }

    void Window::present()
    {
        gtk_window_present(get_native());
    }

    void Window::set_show_menubar(bool b)
    {
        gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(get_native()), b);
    }

    void Window::set_maximized(bool b)
    {
        if (b)
            gtk_window_maximize(get_native());
        else
            gtk_window_unmaximize(get_native());
    }

    void Window::set_fullscreen(bool b )
    {
        if (b)
            gtk_window_fullscreen(get_native());
        else
            gtk_window_unfullscreen(get_native());
    }

    void Window::set_child(Widget* widget)
    {
        gtk_window_set_child(get_native(), widget->operator GtkWidget *());
    }

    void Window::remove_child()
    {
        gtk_window_set_child(get_native(), nullptr);
    }

    void Window::set_focused_widget(Widget* widget)
    {
        gtk_window_set_focus(get_native(), widget->operator GtkWidget*());
    }

    void Window::set_hide_on_close(bool b)
    {
        gtk_window_set_hide_on_close(get_native(), b);
    }

    void Window::set_title(const std::string& str)
    {
        gtk_window_set_title(get_native(), str.c_str());
    }

    void Window::set_destroy_with_parent(bool b)
    {
        gtk_window_set_destroy_with_parent(get_native(), b);
    }

    void Window::set_titlebar_layout(const char* layout)
    {
        auto* titlebar = GTK_HEADER_BAR(gtk_header_bar_new());
        gtk_header_bar_set_decoration_layout(titlebar, layout);
        gtk_window_set_titlebar(get_native(), GTK_WIDGET(titlebar));
    }

    bool Window::on_key_pressed(KeyEventController* self, guint keyval, guint keycode, GdkModifierType state, void*)
    {
        std::vector<std::pair<ShortcutID, GtkShortcutTrigger*>> triggered;
        auto* as_controller = self->operator GtkEventController *();
        auto* event = gtk_event_controller_get_current_event(as_controller);
        if (GDK_IS_EVENT(event))
        {
            for (auto& tuple: _global_shortcuts)
            {
                GdkModifierType modifier = gdk_event_get_modifier_state(event);
                bool shift = modifier & GdkModifierType::GDK_SHIFT_MASK;
                bool alt = modifier & GdkModifierType::GDK_ALT_MASK;
                bool control = modifier & GdkModifierType::GDK_CONTROL_MASK;

                if (gtk_shortcut_trigger_trigger(tuple.trigger, event, false) and
                    shift == tuple.shift and
                    control == tuple.control and
                    alt == tuple.alt)
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

        return false;
    }

    template<typename T>
    void Window::register_global_shortcut(ShortcutMap* map, const std::string& shortcut_id, std::function<void(T)> action, T data)
    {
        auto it = map->_bindings.find(shortcut_id);
        if (it == map->_bindings.end())
        {
            std::cerr << "[ERROR] In Window::register_global_shortcut: Not shortcut trigger binding with id " << shortcut_id << " registered" << std::endl;
            return;
        }

        std::string normalized = gtk_shortcut_trigger_to_string(it->second);
        auto shift = normalized.find("Shift") != std::string::npos;

        auto temp = std::function<void(T)>(action);
        _global_shortcuts.push_back({
            shortcut_id,
            it->second,
            std::function<void(void*)>(*((std::function<void(void*)>*) &temp)),
            reinterpret_cast<void*>(data),
            shift,
            normalized.find("Control") != std::string::npos,
            normalized.find("Alt") != std::string::npos,
        });

        if (shift)
        {
            auto start = normalized.find_last_of(">");
            auto non_modifier = normalized.substr(start == std::string::npos ? 0 : start + 1, std::string::npos);

            if (non_modifier.size() != 1 or not (static_cast<int>(non_modifier.at(0)) >= 97 and static_cast<int>(non_modifier.at(0)) <= 122))
            {
                std::cerr << "[WARNING] In Window::register_global_shortcut: Shortcut \"" << normalized << "\" for action \""
                          << shortcut_id << "\" may be invalid on certain keyboard layouts. Consider using another modifier other than <Shift>." << std::endl;
            }
        }
    }

    void Window::unregister_global_shortcut(const std::string& shortcut_id)
    {
        for (size_t i = 0; i < _global_shortcuts.size(); ++i)
        {
            if (_global_shortcuts.at(i).id == shortcut_id)
                _global_shortcuts.erase(_global_shortcuts.begin() + i);
        }
    }
}