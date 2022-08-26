// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Entry : public Widget
    {
        public:
            Entry();

            std::string get_text() const;
            void set_text(const std::string&);

            operator GtkWidget*() override;

            void set_n_chars(size_t);
            void set_has_frame(bool);

        private:
            GtkEntry* _entry;
            GtkEntryBuffer* _buffer;
    };
}

#include <src/entry.inl>
