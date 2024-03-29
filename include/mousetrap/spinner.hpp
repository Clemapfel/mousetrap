//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Spinner;
    namespace detail
    {
        using SpinnerInternal = GtkSpinner;
        DEFINE_INTERNAL_MAPPING(Spinner);
    }
    #endif

    /// @brief spinning "loading" icon, signifies a task is being executed continously, see also mousetrap::ProgressBar
    /// \signals
    /// \widget_signals{Spinner}
    class Spinner : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Spinner, realize),
        HAS_SIGNAL(Spinner, unrealize),
        HAS_SIGNAL(Spinner, destroy),
        HAS_SIGNAL(Spinner, hide),
        HAS_SIGNAL(Spinner, show),
        HAS_SIGNAL(Spinner, map),
        HAS_SIGNAL(Spinner, unmap)
    {
        public:
            /// @brief construct
            Spinner();

            /// @brief create from internal
            Spinner(detail::SpinnerInternal*);

            /// @brief destructor
            ~Spinner();

            /// @brief expose internal
            NativeObject get_internal() const;

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

        private:
            detail::SpinnerInternal* _internal = nullptr;
    };
}
