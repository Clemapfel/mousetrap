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
            virtual ~Entry();

            std::string get_text() const;
            void set_text(const std::string&);

            GtkWidget* get_native() override;

            void set_width_chars(size_t);
            void set_has_frame(bool);

        protected:
            virtual void on_activate(GtkEntry* self, gpointer user_data);
            virtual void on_paste_clipboard(GtkEntry* self, gpointer user_data);
            virtual void on_cut_clipboard(GtkEntry* self, gpointer user_data);
            virtual void on_copy_clipboard(GtkEntry* self, gpointer user_data);

        private:
            static void on_activate_wrapper(GtkEntry* self, void* instance);
            static void on_paste_clipboard_wrapper(GtkEntry* self, void* instance);
            static void on_cut_clipboard_wrapper(GtkEntry* self, void* instance);
            static void on_copy_clipboard_wrapper(GtkEntry* self, void* instance);

            GtkEntry* _entry;
            GtkEntryBuffer* _buffer;
    };
}

#include <src/entry.inl>
