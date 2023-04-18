//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/12/23
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class HeaderBar : public WidgetImplementation<GtkHeaderBar>
    {
        public:
            /// @brief construct
            HeaderBar();

            /// @brief set layout from config string, see https://docs.gtk.org/gtk4/method.HeaderBar.set_decoration_layout.html
            /// @param layout_string
            void set_layout(const std::string&);

            /// @brief get layout config string
            /// @return string, adheres to https://docs.gtk.org/gtk4/method.HeaderBar.set_decoration_layout.html
            std::string get_layout() const;

            /// @brief set widget that will appear as the header bars title, usually in the center
            /// @param widget
            void set_title_widget(const Widget&);

            /// @brief get widget that will appear as the header bars title, usually in the center
            /// @return widget, may be nullptr
            Widget* get_title_widget() const;

            /// @brief remove the titlebar widget
            void remove_title_widget();

            /// @brief set whether buttons like close, minimize, maximize should be visible
            /// @param b true if visible, false otherwise
            void set_show_title_buttons(bool);

            /// @brief get whether buttons like close, minimize, maximize are visible
            /// @return true if visible, false otherwise
            bool get_show_title_buttons() const;

            /// @brief add a widget to the start of the header bar
            /// @param widget
            void push_back(const Widget&);

            /// @brief add a widget to the end of the header bar
            /// @param widget
            void push_front(const Widget&);

            /// @brief remove a widget from the header bar
            /// @param widget
            void remove(const Widget&);

        private:
            const Widget* _title_widget = nullptr;
    };
}
