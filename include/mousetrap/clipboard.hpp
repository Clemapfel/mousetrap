//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/3/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <string>
#include <mousetrap/file_system.hpp>

#include <mousetrap/image.hpp>
#include <mousetrap/widget.hpp>

namespace mousetrap
{
    class Clipboard;

    #ifndef DOXYGEN
    namespace detail
    {
        struct _ClipboardInternal
        {
            GObject parent;

            GdkClipboard* native;
            std::function<void(const Clipboard&, const std::string&)> get_string_f;
            std::function<void(const Clipboard&, const Image&)> get_image_f;
        };
        using ClipboardInternal = _ClipboardInternal;
    }
    #endif

    /// @brief object representing the data in the systems clipboard
    class Clipboard
    {
        friend class Widget;

        public:
            /// @brief has no public constructor, use Widget::get_clipboard to create a clipboard
            Clipboard() = delete;

            /// @brief construct from internal
            Clipboard(detail::ClipboardInternal*);

            /// @brief destructor
            ~Clipboard();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief expose as GdkClipboard \for_internal_use_only
            operator GdkClipboard*();

            /// @brief check if the clipboard contains a string
            /// @return true if string, false otherwise
            bool contains_string() const;

            /// @brief check if the cliboard contains an image
            /// @return true if image, false otherwise
            bool contains_image() const;

            /// @brief check if the clipboard contains a file, use get_string to retrieve the file path
            /// @return true if file, false otherwise
            bool contains_file() const;

            /// @brief check if the clipboards content was set from within the same application the widget belongs to
            /// @return boolean
            bool get_is_local() const;

            /// @brief override the data in the clipboard with a string
            /// @param string
            void set_string(const std::string& string);

            /// @brief concurrently read the data in the clipboard. Once reading is done, <tt>on_string_read</tt>> will be called with the returned string
            /// @param on_string_read lamdba with signature <tt>(Clipboard*, const std::string&, Data_t) -> void</tt>
            /// @param data arbitrary data
            /// @return true if data can be serialized into a string, false otherwise
            template<typename Function_t, typename Data_t>
            bool get_string(Function_t on_string_read, Data_t data);

            /// @brief concurrently read the data in the clipboard. Once reading is done, <tt>on_string_read</tt>> will be called with the returned string
            /// @param on_string_read lamdba with signature <tt>(Clipboard*, const std::string&) -> void</tt>
            /// @return true if data can be serialized into a string, false otherwise
            template<typename Function_t>
            bool get_string(Function_t on_string_read);

            /// @brief override data in clipboard with an image
            /// @param image
            void set_image(const Image& image);

            /// @brief concurrently read the data in the clipboard. Once reading is done, <tt>on_string_read</tt>> will be called with the returned image
            /// @param on_image_read lamdba with signature <tt>(Clipboard*, const Image&, Data_t) -> void</tt>
            /// @param data arbitrary data
            /// @return true if data can be serialized into an image, false otherwise
            template<typename Function_t, typename Data_t>
            bool get_image(Function_t on_image_read, Data_t data);

            /// @brief concurrently read the data in the clipboard. Once reading is done, <tt>on_string_read</tt>> will be called with the returned image
            /// @param on_image_read lamdba with signature <tt>(Clipboard*, const Image&) -> void</tt>
            /// @return true if data can be serialized into an image, false otherwise
            template<typename Function_t>
            bool get_image(Function_t on_image_read);

            /// @brief override data with a file
            /// @param file
            void set_file(const FileDescriptor& file);

        protected:
            /// @brief constructor from widget, usually a window
            Clipboard(const Widget*);

        private:
            detail::ClipboardInternal* _internal = nullptr;

            static void get_string_callback_wrapper(GObject* clipboard, GAsyncResult* result, detail::ClipboardInternal* data);
            static void get_image_callback_wrapper(GObject* clipboard, GAsyncResult* result, detail::ClipboardInternal* data);
            static void get_file_callback_wrapper(GObject* clipboard, GAsyncResult* result, detail::ClipboardInternal* data);
    };
}

#include "inline/clipboard.hpp"

