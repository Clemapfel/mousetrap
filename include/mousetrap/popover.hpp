//
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/relative_position.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Popover;
    namespace detail
    {
        using PopoverInternal = GtkPopover;
        DEFINE_INTERNAL_MAPPING(Popover);
    }
    #endif

    /// @brief widget container that adds a popover to any other widget. Can be hidden fully and controlled manually
    /// \signals
    /// \signal_closed{Popover}
    /// \widget_signals{Popover}
    class Popover : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Popover, closed),
        HAS_SIGNAL(Popover, realize),
        HAS_SIGNAL(Popover, unrealize),
        HAS_SIGNAL(Popover, destroy),
        HAS_SIGNAL(Popover, hide),
        HAS_SIGNAL(Popover, show),
        HAS_SIGNAL(Popover, map),
        HAS_SIGNAL(Popover, unmap)
    {
        public:
            /// @brief construct with no child
            Popover();

            /// @brief construct from internal
            Popover(detail::PopoverInternal*);

            /// @brief destructor
            ~Popover();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief reveal the popover, does nothing if already revealed
            void popup();

            /// @brief hide the popover, does nothing if already hidden
            void popdown();

            /// @brief set the popovers child
            /// @param widget
            void set_child(const Widget& widget);

            /// @brief remove child
            void remove_child();

            /// @brief set position of popover relative to its anchor widget
            /// @param position
            void set_relative_position(RelativePosition position);

            /// @brief get position of popover relative to its anchor widget
            /// @return position
            RelativePosition get_relative_position() const;

            /// @brief set whether popover should hide automatically when it looses focus
            /// @param b true if it should hide, false otherwise
            void set_autohide(bool b);

            /// @brief get whether popover hides automatically when it looses focus
            /// @return true if it should hide, false otherwise
            bool get_autohide() const;

            /// @brief set whether popover has a triangular arrow pointing to its anchor widget
            /// @param b true if arrow should be shown, false otherwise
            void set_has_base_arrow(bool b);

            /// @brief get whether popover displays a triangular arrow pointing to its anchor widget
            /// @return true if arrow is shown, false otherwise
            bool get_has_base_arrow() const;

        private:
            detail::PopoverInternal* _internal = nullptr;
    };
}
