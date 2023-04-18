//
// Created by clem on 3/24/23.
//

#include <include/file_monitor.hpp>
#include <iostream>

namespace mousetrap
{
    /*
    FileMonitor::FileMonitor(FileMonitor&& other) noexcept
        : CTOR_SIGNAL(FileMonitor, file_changed)
    {
        _native = other._native;
        other._native = nullptr;
    }

    FileMonitor& FileMonitor::operator=(FileMonitor&& other) noexcept
    {
        _native = other._native;
        other._native = nullptr;
        return *this;
    }

    FileMonitor::FileMonitor(GFileMonitor* native)
        : _native(native), CTOR_SIGNAL(FileMonitor, file_changed)
    {
        g_object_ref(_native);
        g_signal_connect(_native, "changed", G_CALLBACK(on_changed), this);
    }

    FileMonitor::~FileMonitor()
    {
        if (_native != nullptr)
            g_object_unref(_native);
    }

    void FileMonitor::on_changed(GFileMonitor*, GFile*, GFile*, GFileMonitorEvent, FileMonitor* instance)
    {
        std::cout << "changed" << std::endl;
    }

    FileMonitor::operator GObject*() const
    {
        return G_OBJECT(_native);
    }

    void FileMonitor::cancel()
    {
        g_file_monitor_cancel(_native);
    }

    bool FileMonitor::is_cancelled() const
    {
        return g_file_monitor_is_cancelled(_native);
    }
     */
}