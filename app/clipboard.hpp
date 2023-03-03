//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/3/23
//

#pragma once

#include <gtk/gtk.h>
#include <string>

#include <include/image.hpp>

namespace mousetrap
{
    class ContentFormats
    {
        public:
            ContentFormats(GdkContentFormats*);

            void from_string(const std::string&);
            std::string to_string() const;

            bool operator==(const ContentFormats& other);
            bool operator!=(const ContentFormats& other);

        private:
            GdkContentFormats* _native;
    };

    class Clipboard
    {
        public:
            Clipboard(GdkClipboard*);
            ContentFormats get_formats() const;
            bool is_local() const;

            void set_text(const std::string&);
            std::string read_text() const;

            void set_image(const Image&);

        private:
            GdkClipboard* _native;
            static void read_text_callback(GObject* clipboard, GAsyncResult* result, gpointer instance);
    };
}
