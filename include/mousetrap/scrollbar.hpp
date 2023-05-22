//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientable.hpp>
#include <mousetrap/adjustment.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Scrollbar;
    namespace detail
    {
        using ScrollbarInternal = GtkScrollbar;
        DEFINE_INTERNAL_MAPPING(Scrollbar);
    }
    #endif

    /// @brief a scrollbar, allows users to scroll a window or range using the scrollwheel or cursor
    /// \signals
    /// \widget_signals{Scrollbar}
    class Scrollbar : public Widget
    {
        public:
            /// @brief construct
            /// @param orientation
            Scrollbar(Orientation);

            /// @brief construct from internal
            Scrollbar(detail::ScrollbarInternal*);

            /// @brief destroy
            ~Scrollbar();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief expose adjustment, modifying it will modify the scrollbar. Connect to the adjustments events if you want to monitor the scrollbars state
            /// @returns adjustment
            Adjustment get_adjustment() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

        private:
           detail::ScrollbarInternal* _internal = nullptr;
    };
}
