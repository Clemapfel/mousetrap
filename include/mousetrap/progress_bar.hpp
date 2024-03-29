//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class ProgressBar;
    namespace detail
    {
        using ProgressBarInternal = GtkProgressBar;
        DEFINE_INTERNAL_MAPPING(ProgressBar);
    }
    #endif

    /// @brief widget that displays progress
    /// \signals
    /// \widget_signals{ProgressBar}
    class ProgressBar : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(ProgressBar, realize),
        HAS_SIGNAL(ProgressBar, unrealize),
        HAS_SIGNAL(ProgressBar, destroy),
        HAS_SIGNAL(ProgressBar, hide),
        HAS_SIGNAL(ProgressBar, show),
        HAS_SIGNAL(ProgressBar, map),
        HAS_SIGNAL(ProgressBar, unmap)
    {
        public:
            /// @brief construct
            ProgressBar();

            /// @brief construct from internal
            ProgressBar(detail::ProgressBarInternal*);

            /// @brief destructor
            ~ProgressBar();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief trigger a pulse animation that communicates that work is being done
            void pulse();

            /// @brief set the fraction of progress the bar is displaying, 0 for fully empty, 1 for full
            /// @param value float in [0, 1]
            void set_fraction(float value);

            /// @brief get the fraction the progress bar is displaying
            /// @returns float in [0, 1]
            float get_fraction() const;

            /// @brief get whether the bar filles left-to-right or right-to-left when horizontal, top-to-bottom or bottom-to-top when vertical
            /// @return true if inverted, false otherwise
            bool get_is_inverted() const;

            /// @brief set whether the bar filles left-to-right or right-to-left when horizontal, top-to-bottom or bottom-to-top when vertical
            /// @param b true if inverted, false otherwise
            void set_is_inverted(bool b);

            /// @brief set text next to the progress bar. If this text is unset, a percentage will be shown instead
            /// @param text
            void set_text(const std::string& text);

            /// @brief get text next to the progress bar, or std::string() if mousetrap::ProgressBar::set_text was not called yet
            std::string get_text() const;

            /// @brief set whether the percentage should be displayed, or, if `set_text` was set to a custom text, it will be displayed instead
            /// @param b
            void set_show_text(bool b);

            /// @brief set whether the percentage should be displayed, or, if `get_text` was set to a custom text, it will be displayed instead
            /// @return boolean
            bool get_show_text() const;

            /// @brief set orientation
            Orientation get_orientation() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

        private:
            detail::ProgressBarInternal* _internal = nullptr;
    };
}
