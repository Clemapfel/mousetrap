// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    enum class EllipsizeMode
    {
        NONE = PANGO_ELLIPSIZE_NONE,
        START = PANGO_ELLIPSIZE_START,
        MIDDLE = PANGO_ELLIPSIZE_MIDDLE,
        END = PANGO_ELLIPSIZE_END,
    };

    enum class LabelWrapMode
    {
        NONE = -1,
        ONLY_ON_WORD = PANGO_WRAP_WORD,
        ONLY_ON_CHAR = PANGO_WRAP_CHAR,
        WORD_OR_CHAR = PANGO_WRAP_WORD_CHAR
    };

    enum class JustifyMode
    {
        LEFT = GTK_JUSTIFY_LEFT,
        RIGHT = GTK_JUSTIFY_RIGHT,
        CENTER = GTK_JUSTIFY_CENTER,
        FILL = GTK_JUSTIFY_FILL
    };

    class Label : public WidgetImplementation<GtkLabel>
    {
        public:
            Label();
            Label(const std::string&);

            void set_text(const std::string&);
            std::string get_text();
            void set_use_markup(bool b);

            void set_ellipsize_mode(EllipsizeMode);
            void set_wrap_mode(LabelWrapMode);
            void set_justify_mode(JustifyMode);
            void set_max_width_chars(size_t n_chars);
    };
}
