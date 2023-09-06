//
// Created by clem on 3/24/23.
//

#include <mousetrap/file_monitor.hpp>
#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(FileMonitorInternal, file_monitor_internal, FILE_MONITOR_INTERNAL)

        static void file_monitor_internal_finalize(GObject *object)
        {
            auto* self = MOUSETRAP_FILE_MONITOR_INTERNAL(object);
            G_OBJECT_CLASS(file_monitor_internal_parent_class)->finalize(object);

            delete self->f;
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(FileMonitorInternal, file_monitor_internal, FILE_MONITOR_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(FileMonitorInternal, file_monitor_internal, FILE_MONITOR_INTERNAL)

        static FileMonitorInternal* file_monitor_internal_new(GFileMonitor* native)
        {
            auto* self = (FileMonitorInternal*) g_object_new(file_monitor_internal_get_type(), nullptr);
            file_monitor_internal_init(self);

            self->native = native;
            self->f = new std::function<void(FileMonitor&, FileMonitorEvent event, const FileDescriptor& self, const FileDescriptor& other)>();
            return self;
        }
    }
    
    FileMonitor::FileMonitor(GFileMonitor* native)
        : _internal(detail::file_monitor_internal_new(native))
    {
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
        g_object_ref_sink(_internal);
        g_signal_connect(_internal->native, "changed", G_CALLBACK(on_changed), _internal);
    }

    FileMonitor::FileMonitor(detail::FileMonitorInternal* internal)
    {
        _internal = g_object_ref(internal);
    }

    FileMonitor::~FileMonitor()
    {
        g_object_unref(_internal);
    }

    NativeObject FileMonitor::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void FileMonitor::on_changed(GFileMonitor*, GFile* file, GFile* other, GFileMonitorEvent event, detail::FileMonitorInternal* instance)
    {
        if (not (*instance->f))
            return;

        auto self_descriptor = G_IS_FILE(file) ? FileDescriptor(file) : FileDescriptor("");
        auto other_descriptor = G_IS_FILE(other) ? FileDescriptor(other) : FileDescriptor("");
        auto self = FileMonitor(instance);
        (*instance->f)(self, (FileMonitorEvent) event, self_descriptor, other_descriptor);
    }

    FileMonitor::operator GObject*() const
    {
        return G_OBJECT(_internal->native);
    }

    void FileMonitor::cancel()
    {
        g_file_monitor_cancel(_internal->native);
    }

    bool FileMonitor::is_cancelled() const
    {
        if (not G_IS_OBJECT(_internal->native))
            return true;

        return g_file_monitor_is_cancelled(_internal->native);
    }
}