//
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    class Application;

    /// @brief window, registered to an application
    class Window : public WidgetImplementation<GtkWindow>,
        HAS_SIGNAL(Window, close_request),
        HAS_SIGNAL(Window, activate_default_widget),
        HAS_SIGNAL(Window, activate_focused_widget)
    {
        public:
            /// @brief construct
            Window();

            /// @brief construct
            /// @param application
            Window(Application&);

            /// @brief link with application
            /// @param application
            void set_application(Application&);

            /// @brief attempt to maximize the window
            /// @param b true if window should be maximize, false otherwise
            void set_maximized(bool);

            /// @brief set whether the window should attempt to enter fullscreen mode
            /// @param b true if window should enter fullscreen mode, false otherwise
            void set_fullscreen(bool);

            /// @brief present the window to the user
            void present();

            /// @brief set whether the window should show the menubar, by default, only the toplevel window shows the applications menubar
            /// @param b true if menubar should be visible, false otherwise
            void set_show_menubar(bool);

            /// @brief set whether mousetrap::Window::close should hide or destroy the window, true by default
            /// @param b true if window should only be hidden, false if it should be destroyed
            void set_hide_on_close(bool);

            /// @brief hide the window, it will be inaccesible until mousetrap::Window::present is called again
            void close();

            /// @brief set the child of the window
            /// @param widget child
            void set_child(Widget&);

            /// @brief remove child
            void remove_child();

            /// @brief get child
            /// @return widget
            Widget* get_child() const;

            /// @brief set whether the window should be destroyed if its parent window is destroyed, true by default
            /// @param b true if it should also be destroyed, false if not
            void set_destroy_with_parent(bool);

            /// @brief get whether the window should be destroyed if its parent window is destroyed
            /// @return true if it should also be destroyed, false if not
            bool get_destroy_with_parent() const;

            /// @brief set the title of the titlebar
            /// @param title
            void set_title(const std::string&);

            /// @brief get the title of the window
            /// @return title
            std::string get_title() const;

            /// @brief replace the titlebar with a custom widget, usually a mousetrap::HeaderBar
            /// @param widget may be null
            void set_titlebar_widget(const Widget&);

            /// @brief replace the titlebar custom widget with the default
            void remove_titlebar_widget();

            /// @brief get custom titlebar widget, or nullptr if there is not widget or the titlebar widget was not set via mousetrap::Window::set_titlebar_widget
            Widget* get_titlebar_widget() const;

            /// @brief set whether the window should be modal. A modal window prevents users from interacting with any other open application window
            /// @param b true if window should be modal, false otherwise
            void set_is_modal(bool);

            /// @brief get whether the window is modal.  A modal window prevents users from interacting with any other open application window
            /// @return true if modal, false otherwise.
            bool get_is_modal() const;

            /// @brief set whether this window should always be shown on top of the partner window
            /// @param partner other window, self will be shown above partner
            void set_transient_for(Window* partner);

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
            void set_startup_notification_identifier(const std::string&);

            /// @brief set whether the focused widget should be highlighted with a rectangle
            /// @param b true if it should be highlighted, false otherwise
            void set_focus_visible(bool);

            /// @brief get whether the focused widget should be highlighted with a rectangle
            /// @return true if it should be highlighted, false otherwise
            bool get_focus_visible() const;

            /// @brief set default widget
            /// @param widget
            void set_default_widget(const Widget& widget);

            /// @brief get default widget
            /// @return widget, may be nullptr
            Widget* get_default_widget() const;

        private:
            const Widget* _child = nullptr;
            const Widget* _titlebar_widget = nullptr;
            const Widget* _default_widget = nullptr;
    };
}