//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/20/23
//

#pragma once

#include <include/widget.hpp>
#include <include/orientable.hpp>
#include <include/adjustment.hpp>

namespace mousetrap
{
    /// @brief a scrollbar, allows users to scroll a window or range using the scrollwheel or cursor
    class Scrollbar : public WidgetImplementation<GtkScrollbar>, public Orientable
    {
        public:
            /// @brief construct
            /// @param orientation
            Scrollbar(Orientation);

            /// @brief destroy
            ~Scrollbar();

            /// @brief expose adjustment, modifying it will modify the scrollbar. Connect to the adjustments events if you want to monitor the scrollbars state
            /// @returns adjustment
            Adjustment& get_adjustment();

            /// @brief expose adjustment as const reference
            /// @returns adjustment, const
            const Adjustment& get_adjustment() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const override;

        private:
            Adjustment* _adjustment = nullptr;
    };
}
