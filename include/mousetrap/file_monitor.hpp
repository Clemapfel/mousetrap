//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>
#include <mousetrap/file_system.hpp>

#include <functional>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
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

    #ifndef DOXYGEN
    namespace detail
    {
        struct _FileMonitorInternal
        {
            GObject parent;
            GFileMonitor* native;
            std::function<void(FileMonitorEvent event, FileDescriptor self, FileDescriptor other)>* f;
        };
        using FileMonitorInternal = _FileMonitorInternal;
    }
    #endif

    /// @brief monitors changes to a file that may or may not yet exist
    class FileMonitor : public SignalEmitter
    {
        friend class FileDescriptor;

        public:
            /// @brief default ctor deleted, use FileDescriptor::create_monitor
            FileMonitor() = delete;

            /// @brief expose as GObject \for_internal_use_only
            operator GObject*() const override;

            /// @brief cancel the file monitor, cannot be undone
            void cancel();

            /// @brief check if file monitor was cancelled
            /// @return true if cancelled, false otherwise
            bool is_cancelled() const;

            /// @brief register callback to be called when file changes
            /// @tparam Function_t
            /// @tparam Data_t arbitrary data
            /// @param f function with signature `(FileMonitorEvent event_type, const FileDescriptor& self, const FileDescriptor& other, Data_t) -> void`
            /// @param data arbitrary data, will be forwarded to f
            template<typename Function_t, typename Data_t>
            void on_file_changed(Function_t f, Data_t data);

            /// @brief register callback to be called when file changes
            /// @brief register callback to be called when file changes
            /// @tparam Function_t
            /// @param f function with signature `(FileMonitorEvent event_type, const FileDescriptor& self, const FileDescriptor& other) -> void`
            template<typename Function_t>
            void on_file_changed(Function_t f);

        protected:
            /// @brief create from native
            /// @param native
            FileMonitor(GFileMonitor* native);

        private:
           detail::FileMonitorInternal* _internal;
           static void on_changed(GFileMonitor* self, GFile* file, GFile* other, GFileMonitorEvent event, detail::FileMonitorInternal* instance);
    };
}

#include "inline/file_monitor.inl"

