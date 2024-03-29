//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/12/23
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class HeaderBar;
    namespace detail
    {
        using HeaderBarInternal = AdwHeaderBar;
        DEFINE_INTERNAL_MAPPING(HeaderBar);
    }
    #endif

    /// @brief widget that is usually used in the decoration of a window
    /// \signals
    /// \widget_signals{HeaderBar}
    class HeaderBar : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(HeaderBar, realize),
        HAS_SIGNAL(HeaderBar, unrealize),
        HAS_SIGNAL(HeaderBar, destroy),
        HAS_SIGNAL(HeaderBar, hide),
        HAS_SIGNAL(HeaderBar, show),
        HAS_SIGNAL(HeaderBar, map),
        HAS_SIGNAL(HeaderBar, unmap)
    {
        public:
            /// @brief construct
            HeaderBar();

            /// @brief construct
            /// @param layout_string see Headerbar::set_layout
            HeaderBar(const std::string& layout_string);

            /// @brief construct from internal
            HeaderBar(detail::HeaderBarInternal*);

            /// @brief destructor
            ~HeaderBar();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set layout from config string, see https://docs.gtk.org/gtk4/method.HeaderBar.set_decoration_layout.html
            /// @param layout_string
            void set_layout(const std::string& layout_string);

            /// @brief get layout config string
            /// @return string, adheres to https://docs.gtk.org/gtk4/method.HeaderBar.set_decoration_layout.html
            std::string get_layout() const;

            /// @brief set widget that will appear as the header bars title, usually in the center
            /// @param widget
            void set_title_widget(const Widget& widget);

            /// @brief remove the titlebar widget
            void remove_title_widget();

            /// @brief set whether buttons like close, minimize, maximize should be visible
            /// @param b true if visible, false otherwise
            void set_show_title_buttons(bool b);

            /// @brief get whether buttons like close, minimize, maximize are visible
            /// @return true if visible, false otherwise
            bool get_show_title_buttons() const;

            /// @brief add a widget to the start of the header bar
            /// @param widget
            void push_back(const Widget& widget);

            /// @brief add a widget to the end of the header bar
            /// @param widget
            void push_front(const Widget& widget);

            /// @brief remove a widget from the header bar
            /// @param widget
            void remove(const Widget& widget);

        private:
            detail::HeaderBarInternal* _internal = nullptr;
    };
}
