// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Entry : public WidgetImplementation<GtkEntry>, public HasActivateSignal<Entry>
    {
        public:
            Entry();

            std::string get_text() const;
            void set_text(const std::string&);

            void set_n_chars(size_t);
            void set_has_frame(bool);
    };
}

#include <src/entry.inl>
