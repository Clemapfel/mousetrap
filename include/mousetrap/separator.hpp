//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientable.hpp>

namespace mousetrap
{
    /// @brief non-container widget that fills it's area with a dark color, used mousetrap::Widget::set_opacity to make it invisible
    /// \signals
    /// \widget_signals{Separator}
    class Separator : public WidgetImplementation<GtkSeparator>, public Orientable
    {
        public:
            /// @brief construct
            /// @param orientation if horizontal, expands horizontally by default, if vertical, expands vertically by default
            Separator(float opacity = 1, Orientation = Orientation::HORIZONTAL);

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const override;
    };
}
