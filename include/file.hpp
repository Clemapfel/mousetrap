// 
// Copyright 2022 Clemens Cords
// Created on 10/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>

namespace mousetrap
{
    class File
    {
        friend struct FileSystem;

        public:
            File(GFile*);
            operator GFile*();

            std::string get_name();
            std::string get_path();

        private:
            GFile* _native;
    };

    struct FileSystem
    {

    };
}

//

namespace mousetrap
{
    File::File(GFile* file)
        : _native(file)
    {}

    File::operator GFile*()
    {
        return _native;
    }

    std::string File::get_name()
    {
        return g_file_get_basename(_native);
    }

    std::string File::get_path()
    {
        return g_file_get_path(_native);
    }
}