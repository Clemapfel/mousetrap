//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/justify_mode.hpp>

namespace mousetrap
{
    /// @brief label ellipsize mode
    enum class EllipsizeMode
    {
        /// @brief do not ellipsize
        NONE = PANGO_ELLIPSIZE_NONE,

        /// @brief insert ... at the start of the string, print end
        START = PANGO_ELLIPSIZE_START,

        /// @brief insert ... in the middle of the string, print start and end
        MIDDLE = PANGO_ELLIPSIZE_MIDDLE,

        /// @brief insert ... at the end of the string, print start
        END = PANGO_ELLIPSIZE_END,
    };

    /// @brief label wrap mode, determines on which part of the label a new line can be created if necessary
    enum class LabelWrapMode
    {
        /// @brief never create a new line
        NONE = -1,

        /// @brief only create a new line on a space between words
        ONLY_ON_WORD = PANGO_WRAP_WORD,

        /// @brief only create a new line after any character
        ONLY_ON_CHAR = PANGO_WRAP_CHAR,

        /// @brief create a new line on a space between words, or if impossible, on a character
        WORD_OR_CHAR = PANGO_WRAP_WORD_CHAR
    };

    /// @brief widget displaying text, supports pango attribute syntax
    /// @see https://docs.gtk.org/Pango/pango_markup.html
    class Label : public WidgetImplementation<GtkLabel>
    {
        public:
            /// @brief construct as empty
            Label();

            /// @brief construct from string
            Label(const std::string&);

            /// @brief set text
            /// @param text
            void set_text(const std::string&);

            /// @brief get text
            /// @return text
            std::string get_text();

            /// @brief set should respect pango attribute syntax, on by default
            /// @param enable
            void set_use_markup(bool b);

            /// @brief get whether label respects pango attribute syntax
            /// @return true if enabled, false otherwise
            bool get_use_markup() const;

            /// @brief set ellipsize mode, mousetrap::EllipsizeMode::NONE by default
            /// @param mode
            void set_ellipsize_mode(EllipsizeMode);

            /// @brief get ellipsize mode
            /// @return mode
            EllipsizeMode get_ellipsize_mode() const;

            /// @brief set wrap mode, mousetrap::LabelWrapMode::NONE by default
            /// @param mode
            void set_wrap_mode(LabelWrapMode);

            /// @brief get wrap mode
            /// @return mode
            LabelWrapMode get_wrap_mode() const;

            /// @brief set justify mdoe, mousetrap::JustifyMode::LEFT by default
            /// @param mode
            void set_justify_mode(JustifyMode);

            /// @brief get justify mode
            /// @return mode
            JustifyMode get_justify_mode() const;

            /// @brief set maximum allowed number of characters in one line
            /// @param n_chars
            void set_max_width_chars(size_t n_chars);

            /// @brief get maximum allowed number of characters in one line
            /// @return size_t
            size_t get_max_width_chars() const;

            /// @brief set horizontal offset of text
            /// @param offset
            void set_x_alignment(float);

            /// @brief get horizontal offset of text
            /// @return offset
            float get_x_alignment() const;

            /// @brief set vertical offset of text
            /// @param offset
            void set_y_alignment(float);

            /// @brief get vertical offset of text
            /// @return offset
            float get_y_alignment() const;

            /// @brief set whether the user can highlight the text with the cursor
            /// @param b true if selection should be possible, false otherwise
            void set_selectable(bool);

            /// @brief get whether the user can highight thex text with the cursor
            /// @return true if selection is possible, false otherwise
            bool get_selectable() const;
    };
}
