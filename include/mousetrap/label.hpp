//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/justify_mode.hpp>

namespace mousetrap
{
    /// @brief label ellipsize mode
    enum class EllipsizeMode : int
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
    enum class LabelWrapMode : int
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

    #ifndef DOXYGEN
    class Label;
    namespace detail
    {
        using LabelInternal = GtkLabel;
        DEFINE_INTERNAL_MAPPING(Label);
    }
    #endif

    /// @brief widget displaying text, supports pango attribute syntax
    /// @see https://docs.gtk.org/Pango/pango_markup.html
    /// \signals
    /// \widget_signals{Label}
    class Label : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Label, realize),
        HAS_SIGNAL(Label, unrealize),
        HAS_SIGNAL(Label, destroy),
        HAS_SIGNAL(Label, hide),
        HAS_SIGNAL(Label, show),
        HAS_SIGNAL(Label, map),
        HAS_SIGNAL(Label, unmap)
    {
        public:
            /// @brief construct as empty
            Label();

            /// @brief construct from string
            /// @param formatted_string
            Label(const std::string& formatted_string);

            /// @brief create from internal
            Label(detail::LabelInternal*);

            /// @brief destructor
            ~Label();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set text
            /// @param formatted_string
            void set_text(const std::string& formatted_string);

            /// @brief get text
            /// @return text
            std::string get_text();

            /// @brief set should respect pango attribute syntax, on by default
            /// @param enable
            void set_use_markup(bool enable);

            /// @brief get whether label respects pango attribute syntax
            /// @return true if enabled, false otherwise
            bool get_use_markup() const;

            /// @brief set ellipsize mode, mousetrap::EllipsizeMode::NONE by default
            /// @param mode
            void set_ellipsize_mode(EllipsizeMode mode);

            /// @brief get ellipsize mode
            /// @return mode
            EllipsizeMode get_ellipsize_mode() const;

            /// @brief set wrap mode, mousetrap::LabelWrapMode::NONE by default
            /// @param mode
            void set_wrap_mode(LabelWrapMode mode);

            /// @brief get wrap mode
            /// @return mode
            LabelWrapMode get_wrap_mode() const;

            /// @brief set justify mdoe, mousetrap::JustifyMode::LEFT by default
            /// @param mode
            void set_justify_mode(JustifyMode mode);

            /// @brief get justify mode
            /// @return mode
            JustifyMode get_justify_mode() const;

            /// @brief set maximum allowed number of characters in one line, or -1 if unlimited
            /// @param n_chars
            void set_max_width_chars(int32_t n_chars);

            /// @brief get maximum allowed number of characters in one line
            /// @return uint64_t
            int32_t get_max_width_chars() const;

            /// @brief set horizontal offset of text
            /// @param offset
            void set_x_alignment(float offset);

            /// @brief get horizontal offset of text
            /// @return offset
            float get_x_alignment() const;

            /// @brief set vertical offset of text
            /// @param offset
            void set_y_alignment(float offset);

            /// @brief get vertical offset of text
            /// @return offset
            float get_y_alignment() const;

            /// @brief set whether the user can highlight the text with the cursor
            /// @param b true if selection should be possible, false otherwise
            void set_is_selectable(bool b);

            /// @brief get whether the user can highight thex text with the cursor
            /// @return true if selection is possible, false otherwise
            bool get_is_selectable() const;

        private:
            detail::LabelInternal* _internal = nullptr;
    };
}
