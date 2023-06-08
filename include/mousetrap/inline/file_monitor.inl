//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/24/23
//

namespace mousetrap
{
    template <typename Function_t, typename Data_t>
    void FileMonitor::on_file_changed(Function_t f_in, Data_t data_in)
    {
        (*_internal->f) = [f = f_in, data = data_in](FileMonitor& self, FileMonitorEvent event, const FileDescriptor& file_self, const FileDescriptor& file_other){
            f(self, event, file_self, file_other, data);
        };
    }

    template <typename Function_t>
    void FileMonitor::on_file_changed(Function_t f_in)
    {
        (*_internal->f) = [f = f_in](FileMonitor& self, FileMonitorEvent event, const FileDescriptor& file_self, const FileDescriptor& file_other){
            f(self, event, file_self, file_other);
        };
    }
}