//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#include <include/window.hpp>
#include <include/application.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Window::Window()
    : WidgetImplementation<GtkWindow>(GTK_WINDOW(gtk_window_new())),
      CTOR_SIGNAL(Window, close_request),
      CTOR_SIGNAL(Window, activate_default_widget),
      CTOR_SIGNAL(Window, activate_focused_widget)
    {}


    Window::Window(Application& app)
        : WidgetImplementation<GtkWindow>(GTK_WINDOW(gtk_application_window_new(app.operator GtkApplication*()))),
          CTOR_SIGNAL(Window, close_request),
          CTOR_SIGNAL(Window, activate_default_widget),
          CTOR_SIGNAL(Window, activate_focused_widget)
    {
        gtk_application_add_window(app.operator GtkApplication*(), get_native());
    }

    void Window::set_application(Application& app)
    {
        gtk_window_set_application(get_native(), app.operator GtkApplication*());
    }

    void Window::present()
    {
        gtk_window_present(get_native());
    }

    void Window::close()
    {
        gtk_window_close(get_native());
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

    void Window::set_fullscreen(bool b)
    {
        if (b)
            gtk_window_fullscreen(get_native());
        else
            gtk_window_unfullscreen(get_native());
    }

    void Window::set_child(Widget& widget)
    {
        _child = &widget;
        WARN_IF_SELF_INSERTION(Window::set_child, this, _child);
        gtk_window_set_child(get_native(), _child == nullptr ? nullptr : _child->operator GtkWidget*());
    }

    void Window::remove_child()
    {
        _child = nullptr;
        gtk_window_set_child(get_native(), nullptr);
    }

    Widget* Window::get_child() const
    {
        return const_cast<Widget*>(_child);
    }

    void Window::set_hide_on_close(bool b)
    {
        gtk_window_set_hide_on_close(get_native(), b);
    }

    void Window::set_title(const std::string& str)
    {
        gtk_window_set_title(get_native(), str.c_str());
    }

    std::string Window::get_title() const
    {
        return gtk_window_get_title(get_native());
    }

    void Window::set_destroy_with_parent(bool b)
    {
        gtk_window_set_destroy_with_parent(get_native(), b);
    }

    bool Window::get_destroy_with_parent() const
    {
        return gtk_window_get_destroy_with_parent(get_native());
    }

    void Window::set_titlebar_widget(const Widget& widget)
    {
        _titlebar_widget = &widget;
        gtk_window_set_titlebar(get_native(), widget.operator NativeWidget());
    }

    void Window::remove_titlebar_widget()
    {
        gtk_window_set_titlebar(get_native(), nullptr);
    }

    Widget* Window::get_titlebar_widget() const
    {
        return const_cast<Widget*>(_titlebar_widget);
    }

    void Window::set_is_modal(bool b)
    {
        gtk_window_set_modal(get_native(), b);
    }

    bool Window::get_is_modal() const
    {
        return gtk_window_get_modal(get_native());
    }

    void Window::set_transient_for(Window* partner)
    {
        if (partner != nullptr)
            gtk_window_set_transient_for(get_native(), partner->operator GtkWindow*());
    }

    void Window::set_is_decorated(bool b)
    {
        gtk_window_set_decorated(get_native(), b);
    }

    bool Window::get_is_decorated() const
    {
        return gtk_window_get_decorated(get_native());
    }

    void Window::set_has_close_button(bool b)
    {
        gtk_window_set_deletable(get_native(), b);
    }

    bool Window::get_has_close_button() const
    {
        return gtk_window_get_deletable(get_native());
    }

    void Window::set_startup_notification_identifier(const std::string& str)
    {
        gtk_window_set_startup_id(get_native(), str.c_str());
    }

    void Window::set_focus_visible(bool b)
    {
        gtk_window_set_focus_visible(get_native(), b);
    }

    bool Window::get_focus_visible() const
    {
        return gtk_window_get_focus_visible(get_native());
    }

    void Window::set_default_widget(const Widget& widget)
    {
        _default_widget = &widget;
        WARN_IF_SELF_INSERTION(Window::set_child, this, _default_widget);
        gtk_window_set_default_widget(get_native(), widget.operator GtkWidget*());
    }

    Widget* Window::get_default_widget() const
    {
        return const_cast<Widget*>(_default_widget);
    }
}