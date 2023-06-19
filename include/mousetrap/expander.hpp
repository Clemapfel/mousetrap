//
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Expander;
    namespace detail
    {
        using ExpanderInternal = GtkExpander;
        DEFINE_INTERNAL_MAPPING(Expander);
    }
    #endif

    /// @brief collapsible list widget, hides the child until clicked, usually display and arrow next to the label
    /// \signals
    /// \signal_activate{Expander}
    /// \widget_signals{Expander}
    class Expander : public Widget,
        HAS_SIGNAL(Expander, activate),
        HAS_SIGNAL(Expander, realize),
        HAS_SIGNAL(Expander, unrealize),
        HAS_SIGNAL(Expander, destroy),
        HAS_SIGNAL(Expander, hide),
        HAS_SIGNAL(Expander, show),
        HAS_SIGNAL(Expander, map),
        HAS_SIGNAL(Expander, unmap)
    {
        public:
            /// @brief expand
            Expander();

            /// @brief construct from internal
            /// @param internal
            Expander(detail::ExpanderInternal* internal);

            /// @brief destructor
            ~Expander();

            /// @brief expose internal object
            NativeObject get_internal() const override;

            /// @brief set child widget, this is the widget that will be hidden when collapsed
            /// @param widget
            void set_child(const Widget&);

            /// @brief remove child widget
            void remove_child();

            /// @brief set label widget, this label will always be displayed
            /// @param widget
            void set_label_widget(const Widget&);

            /// @brief remove label widget
            void remove_label_widget();

            /// @brief get whether expander is expanded
            /// @return false if child widget is currently hidden, true otherwise
            bool get_expanded();

            /// @brief set the expander state
            /// @param b false if child should be hidden, true otherwise
            void set_expanded(bool);

        private:
            detail::ExpanderInternal* _internal = nullptr;
    };
}
