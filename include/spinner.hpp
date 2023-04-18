//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    /// @brief spinning "loading" icon, signifies a task is being executed continously, see also mousetrap::ProgressBar
    class Spinner : public WidgetImplementation<GtkSpinner>
    {
        public:
            /// @brief construct
            Spinner();

            /// @brief set whether the icon should be spinning
            /// @param b true if icon should spin, false otherwise
            void set_is_spinning(bool);

            /// @brief get whether the icon is currently spinning
            /// @return true if icon is spinning, false otherwise
            bool get_is_spinning() const;

            /// @brief start the spinning animation, does nothing if already spinning
            void start();

            /// @brief stop the spinning animation, does nothing if not currently spinning
            void stop();
    };
}
