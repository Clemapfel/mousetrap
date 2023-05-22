//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/window.hpp>
#include <mousetrap/application.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Window::Window()
        : Widget(gtk_window_new()),
          CTOR_SIGNAL(Window, close_request),
          CTOR_SIGNAL(Window, activate_default_widget),
          CTOR_SIGNAL(Window, activate_focused_widget)
    {}

    Window::Window(detail::WindowInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Window, close_request),
          CTOR_SIGNAL(Window, activate_default_widget),
          CTOR_SIGNAL(Window, activate_focused_widget)
    {
        _internal = g_object_ref(internal);
    }

    Window::~Window()
    {
        g_object_unref(_internal);
    }

    Window::Window(Application& app)
        : Widget(gtk_application_window_new(app.operator GtkApplication*())),
          CTOR_SIGNAL(Window, close_request),
          CTOR_SIGNAL(Window, activate_default_widget),
          CTOR_SIGNAL(Window, activate_focused_widget)
    {
        gtk_application_add_window(app.operator GtkApplication*(), GTK_WINDOW(_internal));
    }

    NativeObject Window::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void Window::set_application(Application& app)
    {
        gtk_window_set_application(GTK_WINDOW(_internal), app.operator GtkApplication*());
    }

    void Window::present()
    {
        gtk_window_present(GTK_WINDOW(_internal));
    }

    void Window::close()
    {
        gtk_window_close(GTK_WINDOW(_internal));
    }

    void Window::set_show_menubar(bool b)
    {
        gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(GTK_WINDOW(_internal)), b);
    }

    void Window::set_maximized(bool b)
    {
        if (b)
            gtk_window_maximize(GTK_WINDOW(_internal));
        else
            gtk_window_unmaximize(GTK_WINDOW(_internal));
    }

    void Window::set_fullscreen(bool b)
    {
        if (b)
            gtk_window_fullscreen(GTK_WINDOW(_internal));
        else
            gtk_window_unfullscreen(GTK_WINDOW(_internal));
    }

    void Window::set_child(Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Window::set_child, this, ptr);
        gtk_window_set_child(GTK_WINDOW(_internal), widget.operator GtkWidget*());
    }

    void Window::remove_child()
    {
        gtk_window_set_child(GTK_WINDOW(_internal), nullptr);
    }

    void Window::set_hide_on_close(bool b)
    {
        gtk_window_set_hide_on_close(GTK_WINDOW(_internal), b);
    }

    void Window::set_title(const std::string& str)
    {
        gtk_window_set_title(GTK_WINDOW(_internal), str.c_str());
    }

    std::string Window::get_title() const
    {
        return gtk_window_get_title(GTK_WINDOW(_internal));
    }

    void Window::set_destroy_with_parent(bool b)
    {
        gtk_window_set_destroy_with_parent(GTK_WINDOW(_internal), b);
    }

    bool Window::get_destroy_with_parent() const
    {
        return gtk_window_get_destroy_with_parent(GTK_WINDOW(_internal));
    }

    void Window::set_titlebar_widget(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Window::set_titlebar_widget, this, ptr);
        gtk_window_set_titlebar(GTK_WINDOW(_internal), widget.operator NativeWidget());
    }

    void Window::remove_titlebar_widget()
    {
        gtk_window_set_titlebar(GTK_WINDOW(_internal), nullptr);
    }

    void Window::set_is_modal(bool b)
    {
        gtk_window_set_modal(GTK_WINDOW(_internal), b);
    }

    bool Window::get_is_modal() const
    {
        return gtk_window_get_modal(GTK_WINDOW(_internal));
    }

    void Window::set_transient_for(Window& partner)
    {
        gtk_window_set_transient_for(GTK_WINDOW(_internal), GTK_WINDOW(partner._internal));
    }

    void Window::set_is_decorated(bool b)
    {
        gtk_window_set_decorated(GTK_WINDOW(_internal), b);
    }

    bool Window::get_is_decorated() const
    {
        return gtk_window_get_decorated(GTK_WINDOW(_internal));
    }

    void Window::set_has_close_button(bool b)
    {
        gtk_window_set_deletable(GTK_WINDOW(_internal), b);
    }

    bool Window::get_has_close_button() const
    {
        return gtk_window_get_deletable(GTK_WINDOW(_internal));
    }

    void Window::set_startup_notification_identifier(const std::string& str)
    {
        gtk_window_set_startup_id(GTK_WINDOW(_internal), str.c_str());
    }

    void Window::set_focus_visible(bool b)
    {
        gtk_window_set_focus_visible(GTK_WINDOW(_internal), b);
    }

    bool Window::get_focus_visible() const
    {
        return gtk_window_get_focus_visible(GTK_WINDOW(_internal));
    }

    void Window::set_default_widget(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Window::set_child, this, ptr);
        gtk_window_set_default_widget(GTK_WINDOW(_internal), widget.operator GtkWidget*());
    }
}