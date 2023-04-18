//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief collapsible list widget, hides the child until clicked, usually display and arrow next to the label
    class Expander : public WidgetImplementation<GtkExpander>,
        HAS_SIGNAL(Expander, activate)
    {
        public:
            /// @brief expand
            Expander();

            /// @brief set child widget, this is the widget that will be hidden when collapsed
            /// @param widget
            void set_child(Widget*);

            /// @brief get child widget
            /// @return widget
            Widget* get_child() const;

            /// @brief set label widget, this label will always be displayed
            /// @param widget
            void set_label_widget(Widget*);

            /// @brief get label widget
            /// @return widget
            Widget* get_label_widget() const;

            /// @brief get whether expander is expanded
            /// @return false if child widget is currently hidden, true otherwise
            bool get_expanded();

            /// @brief set the expander state
            /// @param b false if child should be hidden, true otherwise
            void set_expanded(bool);

        private:
            Widget* _child = nullptr;
            Widget* _label_widget = nullptr;
    };
}
