//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/window.hpp>
#include <mousetrap/application.hpp>
#include <mousetrap/log.hpp>
#include <mousetrap/render_area.hpp>
#include <mousetrap/header_bar.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(WindowInternal, window_internal, WINDOW_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(WindowInternal, window_internal, WINDOW_INTERNAL)

        static void window_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_WINDOW_INTERNAL(object);
            G_OBJECT_CLASS(window_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(WindowInternal, window_internal, WINDOW_INTERNAL)

        static void window_internal_mark_shown(void*, void* internal)
        {
            MOUSETRAP_WINDOW_INTERNAL(internal)->is_shown = true;
        }

        static void window_internal_mark_hidden(void*, void* internal)
        {
            MOUSETRAP_WINDOW_INTERNAL(internal)->is_shown = false;
        }

        static WindowInternal* window_internal_new(AdwApplicationWindow* window, GtkApplication* app)
        {
            log::initialize();

            auto* self = (WindowInternal*) g_object_new(window_internal_get_type(), nullptr);
            window_internal_init(self);

            self->native = ADW_APPLICATION_WINDOW(window);
            self->header_bar = ADW_HEADER_BAR(adw_header_bar_new());
            self->vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
            self->content_area = ADW_BIN(adw_bin_new());
            self->header_bar_area = ADW_BIN(adw_bin_new());
            self->is_shown = false;

            gtk_application_add_window(app, GTK_WINDOW(self->native));
            gtk_window_set_focus_visible(GTK_WINDOW(self->native), true);

            adw_application_window_set_content(self->native, GTK_WIDGET(self->vbox));
            adw_bin_set_child(self->header_bar_area, GTK_WIDGET(self->header_bar));

            gtk_box_append(self->vbox, GTK_WIDGET(self->header_bar_area));
            gtk_box_append(self->vbox, GTK_WIDGET(self->content_area));

            gtk_widget_set_vexpand(GTK_WIDGET(self->header_bar_area), false);
            gtk_widget_set_valign(GTK_WIDGET(self->header_bar_area), GTK_ALIGN_START);
            gtk_widget_set_vexpand(GTK_WIDGET(self->content_area), true);
            gtk_widget_set_hexpand(GTK_WIDGET(self->content_area), true);

            g_signal_connect(self->native, "show", G_CALLBACK(detail::window_internal_mark_shown), self);
            g_signal_connect(self->native, "hide", G_CALLBACK(detail::window_internal_mark_hidden), self);

            detail::attach_ref_to(G_OBJECT(self->native), self);
            return self;
        }
    }
    
    Window::Window(Application& app)
        : Widget(adw_application_window_new(app.operator GtkApplication*())),
          CTOR_SIGNAL(Window, close_request),
          CTOR_SIGNAL(Window, activate_default_widget),
          CTOR_SIGNAL(Window, activate_focused_widget),
          CTOR_SIGNAL(Window, realize),
          CTOR_SIGNAL(Window, unrealize),
          CTOR_SIGNAL(Window, destroy),
          CTOR_SIGNAL(Window, hide),
          CTOR_SIGNAL(Window, show),
          CTOR_SIGNAL(Window, map),
          CTOR_SIGNAL(Window, unmap)
    {
        _internal = g_object_ref(detail::window_internal_new(ADW_APPLICATION_WINDOW(Widget::operator NativeWidget()), app.operator GtkApplication*()));
    }

    Window::Window(detail::WindowInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(Window, close_request),
          CTOR_SIGNAL(Window, activate_default_widget),
          CTOR_SIGNAL(Window, activate_focused_widget),
          CTOR_SIGNAL(Window, realize),
          CTOR_SIGNAL(Window, unrealize),
          CTOR_SIGNAL(Window, destroy),
          CTOR_SIGNAL(Window, hide),
          CTOR_SIGNAL(Window, show),
          CTOR_SIGNAL(Window, map),
          CTOR_SIGNAL(Window, unmap)
    {
        _internal = g_object_ref(internal);
    }

    Window::~Window()
    {
        g_object_unref(_internal);
    }

    NativeObject Window::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    Window::operator NativeObject() const
    {
        return G_OBJECT(_internal->native);
    }

    void Window::set_application(Application& app)
    {
        gtk_window_set_application(GTK_WINDOW(_internal->native), app.operator GtkApplication*());
    }

    void Window::present()
    {
        gtk_window_present(GTK_WINDOW(_internal->native));
    }

    void Window::close()
    {
        gtk_window_close(GTK_WINDOW(_internal->native));
    }

    void Window::set_maximized(bool b)
    {
        if (b)
            gtk_window_maximize(GTK_WINDOW(_internal->native));
        else
            gtk_window_unmaximize(GTK_WINDOW(_internal->native));
    }

    void Window::set_fullscreen(bool b)
    {
        if (b)
            gtk_window_fullscreen(GTK_WINDOW(_internal->native));
        else
            gtk_window_unfullscreen(GTK_WINDOW(_internal->native));
    }

    void Window::set_minimized(bool b)
    {
        if (b)
            gtk_window_minimize(GTK_WINDOW(_internal->native));
        else
            gtk_window_unminimize(GTK_WINDOW(_internal->native));
    }

    void Window::set_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Window::set_child, this, ptr);
        WARN_IF_PARENT_EXISTS(Window::set_child, widget);

        adw_bin_set_child(_internal->content_area, widget.operator NativeWidget());
    }

    void Window::remove_child()
    {
        adw_bin_set_child(_internal->content_area, nullptr);
    }

    void Window::set_hide_on_close(bool b)
    {
        gtk_window_set_hide_on_close(GTK_WINDOW(_internal->native), b);
    }

    bool Window::get_hide_on_close() const
    {
        return gtk_window_get_hide_on_close(GTK_WINDOW(_internal->native));
    }

    void Window::set_title(const std::string& str)
    {
        gtk_window_set_title(GTK_WINDOW(_internal->native), str.c_str());
    }

    std::string Window::get_title() const
    {
        auto* title = gtk_window_get_title(GTK_WINDOW(_internal->native));
        return title != nullptr ? std::string(title) : "";
    }

    void Window::set_destroy_with_parent(bool b)
    {
        gtk_window_set_destroy_with_parent(GTK_WINDOW(_internal->native), b);
    }

    bool Window::get_destroy_with_parent() const
    {
        return gtk_window_get_destroy_with_parent(GTK_WINDOW(_internal->native));
    }

    HeaderBar Window::get_header_bar() const
    {
        return HeaderBar(_internal->header_bar);
    }

    void Window::set_is_modal(bool b)
    {
        gtk_window_set_modal(GTK_WINDOW(_internal->native), b);
    }

    bool Window::get_is_modal() const
    {
        return gtk_window_get_modal(GTK_WINDOW(_internal->native));
    }

    void Window::set_transient_for(Window& partner)
    {
        gtk_window_set_transient_for(GTK_WINDOW(_internal->native), GTK_WINDOW(partner._internal->native));
    }

    void Window::set_is_decorated(bool b)
    {
        gtk_widget_set_visible(GTK_WIDGET(_internal->header_bar_area), b);
    }

    bool Window::get_is_decorated() const
    {
        return gtk_widget_get_visible(GTK_WIDGET(_internal->header_bar_area));
    }

    void Window::set_has_close_button(bool b)
    {
        gtk_window_set_deletable(GTK_WINDOW(_internal->native), b);
    }

    bool Window::get_has_close_button() const
    {
        return gtk_window_get_deletable(GTK_WINDOW(_internal->native));
    }

    void Window::set_startup_notification_identifier(const std::string& str)
    {
        gtk_window_set_startup_id(GTK_WINDOW(_internal->native), str.c_str());
    }

    void Window::set_focus_visible(bool b)
    {
        gtk_window_set_focus_visible(GTK_WINDOW(_internal->native), b);
    }

    bool Window::get_focus_visible() const
    {
        return gtk_window_get_focus_visible(GTK_WINDOW(_internal->native));
    }

    void Window::set_default_widget(const Widget& widget)
    {
        gtk_window_set_default_widget(GTK_WINDOW(_internal->native), widget.operator GtkWidget*());
    }

    void Window::destroy()
    {
        gtk_window_destroy(GTK_WINDOW(_internal->native));
    }

    bool Window::get_is_closed() const
    {
        if (G_IS_OBJECT(_internal))
            return not _internal->is_shown;
        else
            return true;
    }
}