//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 6/8/23
//

namespace mousetrap
{
    template<typename Function_t, typename Data_t>
    void FileChooser::on_accept(Function_t function, Data_t data)
    {
        _internal->on_accept = new std::function<void(FileChooser&, const std::vector<FileDescriptor>&)>([f = function, d = data](FileChooser& self, const std::vector<FileDescriptor>& files)
        {
            f(self, files, d);
        });
    }

    template<typename Function_t>
    void FileChooser::on_accept(Function_t function)
    {
        _internal->on_accept = new std::function<void(FileChooser&, const std::vector<FileDescriptor>&)>([f = function](FileChooser& self, const std::vector<FileDescriptor>& files)
        {
            f(self, files);
        });
    }

    template<typename Function_t, typename Data_t>
    void FileChooser::on_cancel(Function_t function, Data_t data)
    {
        _internal->on_cancel = new std::function<void(FileChooser&)>([f = function, d = data](FileChooser& self)
        {
            f(self, d);
        });
    }

    template<typename Function_t>
    void FileChooser::on_cancel(Function_t function)
    {
        _internal->on_cancel = new std::function<void(FileChooser&)>([f = function](FileChooser& self)
        {
            f(self);
        });
    }
}