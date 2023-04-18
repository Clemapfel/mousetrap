//
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/relative_position.hpp>

namespace mousetrap
{
    /// @brief widget container that adds a popover to any other widget. Can be hidden fully and controlled manually
    class Popover : public WidgetImplementation<GtkPopover>,
        HAS_SIGNAL(Popover, closed)
    {
        public:
            /// @brief construct with no child
            Popover();

            /// @brief reveal the popover, does nothing if already revealed
            void popup();

            /// @brief hide the popover, does nothing if already hidden
            void popdown();

            /// @brief allocate size for the popover in the layout of it's possible container, usually called automatically
            void present();

            /// @brief set the popovers child
            /// @param widget
            void set_child(Widget*);

            /// @brief get child
            /// @return child
            Widget* get_child() const;

            /// @brief attach popover to another widget, that widget will become the popovers anchor
            /// @param widget
            void attach_to(Widget*);

            /// @brief set position of popover relative to its anchor widget
            /// @param position
            void set_relative_position(RelativePosition);

            /// @brief get position of popover relative to its anchor widget
            /// @return position
            RelativePosition get_relative_position() const;

            /// @brief set whether popover should hide automatically when it looses focus
            /// @param b true if it should hide, false otherwise
            void set_autohide(bool);

            /// @brief get whether popover hides automatically when it looses focus
            /// @return true if it should hide, false otherwise
            bool get_autohide() const;

            /// @brief set whether popover has a triangular arrow pointing to its anchor widget
            /// @param b true if arrow should be shown, false otherwise
            void set_has_base_arrow(bool);

            /// @brief get whether popover displays a triangular arrow pointing to its anchor widget
            /// @return true if arrow is shown, false otherwise
            bool get_has_base_arrow() const;

        private:
            Widget* _child = nullptr;
    };
}
