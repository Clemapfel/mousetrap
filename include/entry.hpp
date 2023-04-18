//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief a single-line text entry
    class Entry : public WidgetImplementation<GtkEntry>,
        HAS_SIGNAL(Entry, activate),
        HAS_SIGNAL(Entry, text_changed)
    {
        public:
            /// @brief construct
            Entry();

            /// @brief get currently displayed test
            /// @returns string
            std::string get_text() const;

            /// @brief set text, updates display
            /// @param text
            void set_text(const std::string&);

            /// @brief set maximum number of characters for the text display or -1 for unlimited
            /// @param n
            void set_max_length(int);

            /// @brief get maximum number of characters for the text display, or -1 if unlimited
            /// @return size_t
            int get_max_length() const;

            /// @brief should the text entry have a dark outline
            /// @param b
            void set_has_frame(bool);

            /// @brief get whether the text entry should have a dark outline
            /// @return bool
            bool get_has_frame() const;

            /// @brief enter password mode, all charactesr will be replaced with an identical symbol
            /// @param b
            void set_text_visible(bool);

            /// @brief get whether the text entry is currently in password mode
            bool get_text_visible() const;
    };
}
