//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientable.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Separator;
    namespace detail
    {
        using SeparatorInternal = GtkSeparator;
        DEFINE_INTERNAL_MAPPING(Separator);
    }
    #endif

    /// @brief non-container widget that fills it's area with a dark color, used mousetrap::Widget::set_opacity to make it invisible
    /// \signals
    /// \widget_signals{Separator}
    class Separator : public Widget,
        HAS_SIGNAL(Separator, realize),
        HAS_SIGNAL(Separator, unrealize),
        HAS_SIGNAL(Separator, destroy),
        HAS_SIGNAL(Separator, hide),
        HAS_SIGNAL(Separator, show),
        HAS_SIGNAL(Separator, map),
        HAS_SIGNAL(Separator, unmap)
    {
        public:
            /// @brief construct
            /// @param opacity float in [0, 1]
            /// @param orientation if horizontal, expands horizontally by default, if vertical, expands vertically by default
            Separator(float opacity = 1, Orientation = Orientation::HORIZONTAL);

            /// @brief construct from internal
            Separator(detail::SeparatorInternal*);

            /// @brief destructor
            ~Separator();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

        private:
            detail::SeparatorInternal* _internal = nullptr;
    };
}
