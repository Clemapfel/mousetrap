//
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Application;
    class Window;
    namespace detail
    {
        struct _WindowInternal
        {
            GObject parent_instance;

            AdwApplicationWindow* native;
            AdwHeaderBar* header_bar;
            GtkBox* vbox;
            AdwBin* content_area;
        };
        using WindowInternal = _WindowInternal;
        DEFINE_INTERNAL_MAPPING(Window);
    }
    #endif

    /// @brief window, registered to an application
    /// \signals
    /// \signal_close_request{Window}
    /// \signal_activate_default_widget{Window}
    /// \signal_activate_focused_widget{Window}
    /// \widget_signals{Window}
    class Window : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Window, close_request),
        HAS_SIGNAL(Window, activate_default_widget),
        HAS_SIGNAL(Window, activate_focused_widget),
        HAS_SIGNAL(Window, realize),
        HAS_SIGNAL(Window, unrealize),
        HAS_SIGNAL(Window, destroy),
        HAS_SIGNAL(Window, hide),
        HAS_SIGNAL(Window, show),
        HAS_SIGNAL(Window, map),
        HAS_SIGNAL(Window, unmap)
    {
        public:
            /// @brief construct
            /// @param application
            Window(Application& application);

            /// @brief construct from internal
            Window(detail::WindowInternal*);

            /// @brief destructor
            ~Window();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief override signal emitter
            operator NativeObject() const override;

            /// @brief link with application
            /// @param application
            void set_application(Application& application);

            /// @brief attempt to maximize the window
            /// @param b true if window should be maximize, false otherwise
            void set_maximized(bool);

            /// @brief set whether the window should attempt to enter fullscreen mode
            /// @param b true if window should enter fullscreen mode, false otherwise
            void set_fullscreen(bool);

            /// @brief attempt to minimize the window
            /// @param b true if window should be minimized, false otherwise
            void set_minimized(bool);

            /// @brief present the window to the user
            void present();

            /// @brief set whether mousetrap::Window::close should hide or destroy the window, true by default
            /// @param b true if window should only be hidden, false if it should be destroyed
            void set_hide_on_close(bool);

            /// @brief get whether mousetrap::Window::close should hide or destroy the window, true by default
            /// @return boolean
            bool get_hide_on_close() const;

            /// @brief hide the window, it will be inaccesible until mousetrap::Window::present is called again
            void close();

            /// @brief set the child of the window
            /// @param widget child
            void set_child(const Widget&);

            /// @brief remove child
            void remove_child();

            /// @brief set whether the window should be destroyed if its parent window is destroyed, true by default
            /// @param b true if it should also be destroyed, false if not
            void set_destroy_with_parent(bool);

            /// @brief get whether the window should be destroyed if its parent window is destroyed
            /// @return true if it should also be destroyed, false if not
            bool get_destroy_with_parent() const;

            /// @brief set the title of the titlebar
            /// @param title
            void set_title(const std::string& title);

            /// @brief get the title of the window
            /// @return title
            std::string get_title() const;

            /// @brief replace the titlebar with a custom widget, usually a mousetrap::HeaderBar
            /// @param widget may be null
            void set_titlebar_widget(const Widget&);

            /// @brief replace the titlebar custom widget with the default
            void remove_titlebar_widget();

            /// @brief set whether the window should be modal. A modal window prevents users from interacting with any other open application window
            /// @param b true if window should be modal, false otherwise
            void set_is_modal(bool);

            /// @brief get whether the window is modal.  A modal window prevents users from interacting with any other open application window
            /// @return true if modal, false otherwise.
            bool get_is_modal() const;

            /// @brief set whether this window should always be shown on top of the partner window
            /// @param partner other window, self will be shown above partner
            void set_transient_for(Window& partner);

            /// @brief set whether the window has a titlebar
            /// @param b true if window should have a titlebar, false othterwise
            void set_is_decorated(bool);

            /// @brief get whether the window has a titlebar
            /// @return true if window has a titlebar, false otherwise
            bool get_is_decorated() const;

            /// @brief set whether the window should have a close button
            /// @param b true if window should have a close button, false otherwise
            void set_has_close_button(bool);

            /// @brief get whether the window has a close button
            /// @return true if close button should be shown, false otherwise
            bool get_has_close_button() const;

            /// @brief set the window startup notification identifier, usually results in the users OS sending a notification "<name> is ready"
            /// @param id
            void set_startup_notification_identifier(const std::string& id);

            /// @brief set whether the focused widget should be highlighted with a rectangle
            /// @param b true if it should be highlighted, false otherwise
            void set_focus_visible(bool);

            /// @brief get whether the focused widget should be highlighted with a rectangle
            /// @return true if it should be highlighted, false otherwise
            bool get_focus_visible() const;

            /// @brief set default widget
            /// @param widget
            void set_default_widget(const Widget& widget);

            /// @brief free all internal references
            void destroy();

        private:
            detail::WindowInternal* _internal = nullptr;
    };
}