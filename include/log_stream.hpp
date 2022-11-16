// 
// Copyright 2022 Clemens Cords
// Created on 11/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <iostream>
#include <sstream>

#include <include/file.hpp>
#include <include/time.hpp>

namespace mousetrap
{
    enum LogMessageType
    {
        LOG,
        WARNING,
        ERROR,
        FATAL
    };

    class LogStream
    {
        public:
            LogStream(const std::string& path);
            ~LogStream();

            void add_line(const std::string&, LogMessageType);

        private:
            GFileIOStream* _io_stream;
    };
}

//

namespace mousetrap
{
    LogStream::LogStream(const std::string& path)
    {
        auto* file = g_file_new_for_path(path.c_str());

        GError* error = nullptr;
        _io_stream = g_file_create_readwrite(file, G_FILE_CREATE_REPLACE_DESTINATION, nullptr, &error);

        if (error != nullptr)
            std::cerr << "[ERROR] Unable to create log file at `" << path << "`: " << error->message << std::endl;

        g_object_unref(file);
    }

    LogStream::~LogStream()
    {
        GError* error = nullptr;
        g_io_stream_close(G_IO_STREAM(_io_stream), nullptr, &error);
    }

    void LogStream::add_line(const std::string& message, LogMessageType type)
    {
        std::stringstream str;

        if (type == LOG)
            str << "[LOG]";
        else if (type == WARNING)
            str << "[WARNING]";
        else if (type == ERROR)
            str << "[ERROR]";
        else if (type == FATAL)
            str << "[FATAL]";

        str << "[" << get_timestamp_now() << "] ";
        str << message;

        g_write
    }
}