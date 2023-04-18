//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>
#include <include/file_system.hpp>

#include <functional>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief hints type of mutation done to file
    enum class FileMonitorEvent
    {
        /// @brief file has changed in any way
        CHANGED = G_FILE_MONITOR_EVENT_CHANGED,

        /// @brief hints that this event is the last in a series of changes
        CHANGES_DONE_HINT = G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT,

        /// @brief file was deleted
        DELETED = G_FILE_MONITOR_EVENT_DELETED,

        /// @brief file was created
        CREATED = G_FILE_MONITOR_EVENT_CREATED,

        /// @brief an attribute such as metadata or file type changed
        ATTRIBUTE_CHANGED = G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED,

        /// @brief file was renamed
        RENAMED = G_FILE_MONITOR_EVENT_RENAMED,

        /// @brief if target is a directory, a file was moved into it
        MOVED_IN = G_FILE_MONITOR_EVENT_MOVED_IN,

        /// @brief if target is a directory, a file was moved out of it
        MOVED_OUT = G_FILE_MONITOR_EVENT_MOVED_OUT
    };


    /// @brief monitors changes to a file that may or may not yet exist
    class FileMonitor //:  public SignalEmitter
    {
        friend class FileDescriptor;

        public:
            /// @brief default ctor deleted, use FileDescriptor::create_monitor
            FileMonitor() = default;

            /// @brief default dtor, this removes any connected signals
            ~FileMonitor() = default;

            /// @brief copy ctor deleted, use FileDescriptor::create_monitor to create an additional monitor to the same file
            FileMonitor(const FileMonitor&) = delete;

            /// @brief copy assignment deleted
            FileMonitor& operator=(const FileMonitor&) = delete;

            /// @brief move ctor
            /// @param other
            FileMonitor(FileMonitor&&) noexcept;

            /// @brief move assignment
            /// @param other
            /// @return reference to self after assignment
            FileMonitor& operator=(FileMonitor&&) noexcept;

            /// @brief expose as GObject \internal
            //operator GObject*() const override;

            /// @brief cancel the file monitor, cannot be undone
            void cancel();

            /// @brief check if file monitor was cancelled
            /// @return true if cancelled, false otherwise
            bool is_cancelled() const;

            /// @brief register callback to be called when file changes
            template<typename Function_t, typename Data_t>
            void on_file_changed(Function_t f, Data_t data);

            /// @brief register callback to be called when file changes
            template<typename Function_t>
            void on_file_changed(Function_t f);

        protected:
            FileMonitor(GFileMonitor*) {};

        private:
            GFileMonitor* _native;
            static void on_changed(GFileMonitor*, GFile*, GFile*, GFileMonitorEvent, FileMonitor* instance);
            static inline gint flags = G_FILE_MONITOR_NONE | G_FILE_MONITOR_WATCH_MOVES;

    };
}

