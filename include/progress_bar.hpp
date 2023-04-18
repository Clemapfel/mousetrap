//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <include/widget.hpp>
#include <include/orientable.hpp>

namespace mousetrap
{
    /// @brief widget that displays progress
    class ProgressBar : public WidgetImplementation<GtkProgressBar>, public Orientable
    {
        public:
            /// @brief construct
            ProgressBar();

            /// @brief trigger a pulse animation that communicates that work is being done
            void pulse();

            /// @brief set the fraction of progress the bar is displaying, 0 for fully empty, 1 for full
            /// @param value: float in [0, 1]
            void set_fraction(float);

            /// @brief get the fraction the progress bar is displaying
            /// @returns float in [0, 1]
            float get_fraction() const;

            /// @brief get whether the bar filles left-to-right or right-to-left when horizontal, top-to-bottom or bottom-to-top when vertical
            /// @return true if inverted, false otherwise
            bool get_is_inverted() const;

            /// @brief set whether the bar filles left-to-right or right-to-left when horizontal, top-to-bottom or bottom-to-top when vertical
            /// @param b true if inverted, false otherwise
            void set_is_inverted(bool);

            /// @brief set text next to the progress bar. If this text is unset, a percentage will be shown instead
            /// @param text
            void set_text(const std::string&);

            /// @brief get text next to the progress bar, or std::string() if mousetrap::ProgressBar::set_text was not called yet
            std::string get_text() const;

            /// @brief set whether text (or the percentage if mousetrap::ProgressBar::set_text was not called yet) should appear next to the progress bar
            /// @param b true if text should appear, false otherwise
            void set_show_text_or_percentage(bool);

            /// @brief sgt whether text (or the percentage if mousetrap::ProgressBar::set_text was not called yet) should appear next to the progress bar
            /// @return true if text should appear, false otherwise
            bool get_show_text_or_percentage() const;

            /// @copydoc mousetrap::Orientable::set_orientation
            Orientation get_orientation() const override;

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation) override;
    };
}
