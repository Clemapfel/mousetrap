//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/icon.hpp>

#ifdef DOXYGEN
    #include <mousetrap/.*>
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Entry;
    namespace detail
    {
        using EntryInternal = GtkEntry;
        DEFINE_INTERNAL_MAPPING(Entry);
    }
    #endif

    /// @brief a single-line text entry
    /// \signals
    /// \signal_activate{Entry}
    /// \signal_text_changed{Entry}
    /// \widget_signals{Entry}
    class Entry : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Entry, activate),
        HAS_SIGNAL(Entry, text_changed),
        HAS_SIGNAL(Entry, realize),
        HAS_SIGNAL(Entry, unrealize),
        HAS_SIGNAL(Entry, destroy),
        HAS_SIGNAL(Entry, hide),
        HAS_SIGNAL(Entry, show),
        HAS_SIGNAL(Entry, map),
        HAS_SIGNAL(Entry, unmap)
    {
        public:
            /// @brief construct
            Entry();

            /// @brief construct from internal
            /// @param internal
            Entry(detail::EntryInternal*);

            /// @brief destructor
            ~Entry();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief expose signal handler
            operator NativeObject() const override;

            /// @brief get currently displayed test
            /// @returns string
            std::string get_text() const;

            /// @brief set text, updates display
            /// @param text
            void set_text(const std::string&);

            /// @brief set maximum number of characters for the text display or -1 for unlimited
            /// @param n
            void set_max_width_chars(int32_t);

            /// @brief get maximum number of characters for the text display, or -1 if unlimited
            /// @return size_t
            int32_t get_max_width_chars() const;

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

            /// @brief add icon to the start of entry
            /// @param icon
            void set_primary_icon(const Icon&);

            /// @brief remove icon at the start of entry
            void remove_primary_icon();

            /// @brief add icon to end of entry
            /// @param icon
            void set_secondary_icon(const Icon&);

            /// @brief remove icon at the end of entry
            void remove_secondary_icon();

        private:
            detail::EntryInternal* _internal = nullptr;
    };
}
