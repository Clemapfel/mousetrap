//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/19/23
//

#pragma once

#include <include/widget.hpp>
#include <include/justify_mode.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief multi-line text entry
    /// @note obfuscates many of GtkTextViews functionalities for the sake of brevity and simplicity
    class TextView : public WidgetImplementation<GtkTextView>,
        HAS_SIGNAL(TextView, text_changed),
        HAS_SIGNAL(TextView, undo),
        HAS_SIGNAL(TextView, redo)
    {
        public:
            /// @brief construct
            TextView();

            /// @brief get current text
            /// @return text
            [[nodiscard]] std::string get_text() const;
            
            /// @brief replace displayed text
            /// @param text
            void set_text(const std::string& text) const;

            /// @brief set whether the text cursor should be visible
            /// @param b true if visible, false otherwise
            void set_cursor_visible(bool);

            /// @brief get whether the text cursor should be visible
            /// @return true if visible, false otherwise
            bool get_cursor_visible() const;

            /// @brief undo last action, if possible
            void undo();
            
            /// @brief redo last action, if possible
            void redo();

            /// @brief mark whether the text buffer changed state manually, usually used to reset the state so the program can keep track of user interaction using mousetrap::TextView::get_was_modified
            /// @param b true if marked as modified, false otherwise
            void set_was_modified(bool);
            
            /// @brief get if the text buffer was modified, automatically set to true after any user modification
            /// @return true if modified, false otherwise
            bool get_was_modified() const;

            /// @brief get whether the text view should be editable
            /// @param b true if editable, false otherwise
            void set_editable(bool);
            
            /// @brief set whether the text view should be editable
            /// @return true if editable, false otherwise
            bool get_editable() const;

            /// @brief set justify mode of the text
            /// @param mode
            void set_justify_mode(JustifyMode);
            
            /// @brief get justify mode of text
            /// @return mode
            JustifyMode get_justify_mode() const;

            /// @brief set left margin of the text, minimum distance between the left side of the widget and the start of each line
            /// @param distance in pixels
            void set_left_margin(float);

            /// @brief get left margin of the text, minimum distance between the left side of the widget and the start of each line
            /// @return distance in pixels
            float get_left_margin() const;

            /// @brief get left margin of the text, minimum distance between the right side of the widget and the end of each line
            /// @return distance in pixels
            void set_right_margin(float);

            /// @brief get right margin of the text, minimum distance between the right side of the widget and the end of each line
            /// @return distance in pixels
            float get_right_margin() const;

            /// @brief set top margin of the text, minimum distance between the top of the widget and the top of the first line of text
            /// @param distance in pixels
            void set_top_margin(float);

            /// @brief get top margin of the text, minimum distance between the top of the widget and the top of the last line of text
            /// @param distance in pixels
            float get_top_margin() const;

            /// @brief set bottom margin of the text, minimum distance between the bottom of the widget and the bottom of the first line of text
            /// @param distance in pixels
            void set_bottom_margin(float);
            
            /// @brief get bottom margin of the text, minimum distance between the top of the widget and the top of the first line of text
            /// @return distance in pixels
            float get_bottom_margin() const;
    };
}
