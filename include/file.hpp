// 
// Copyright 2022 Clemens Cords
// Created on 10/23/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <future>

namespace mousetrap
{
    using FilePath = std::string;
    using FileURI = std::string;

    class FileDescriptor
    {
        public:
            FileDescriptor();
            FileDescriptor(const FilePath& path);
            FileDescriptor(GFile*);

            operator GFile*();

            void create_for_path(const FilePath& path);
            void create_for_uri(const FileURI& uri);

            FilePath get_name();
            FilePath get_path();
            FileURI  get_uri();

            bool operator==(const FileDescriptor& other);
            bool operator!=(const FileDescriptor& other);

        private:

            GFile* _native;
    };

    namespace FileSystem
    {
        bool create_at(FileDescriptor & destination, bool should_replace_destination);
        bool delete_at(FileDescriptor &);

        FileDescriptor duplicate(FileDescriptor&);

        bool copy(
            FileDescriptor& from,
            FileDescriptor& to,
            bool allow_overwrite = true,
            bool make_backup = false,
            bool follow_symlinks = false
        );

        void copy_async(
            FileDescriptor& from,
            FileDescriptor& to,
            int io_priority = 0,
            bool allow_overwrite = true,
            bool make_backup = false,
            bool follow_symlinks = false
        );

        bool move(
            FileDescriptor& from,
            FileDescriptor& to,
            bool allow_overwrite = true,
            bool make_backup = false,
            bool follow_symlinks = false
        );

        void move_async(
            FileDescriptor& from,
            FileDescriptor& to,
            int io_priority = 0,
            bool allow_overwrite = true,
            bool make_backup = false,
            bool follow_symlinks = false
        );

        bool move_to_trash(FileDescriptor&);

        bool file_exists(const FilePath& path);
        bool file_is_directory(const FilePath& path);
        bool file_is_symlink(const FilePath& path);
        bool file_is_executable(const FilePath& path);
        FilePath file_read_symlink(const FilePath& path);

        FileURI file_path_to_file_uri(const FilePath& path);
        FilePath file_uri_to_file_path(const FileURI& uri);
    };
}

//

namespace mousetrap
{
    FileDescriptor::FileDescriptor()
        : _native(g_file_new_for_path(""))
    {}

    FileDescriptor::FileDescriptor(const FilePath& path)
        : _native(g_file_new_for_path(path.c_str()))
    {}

    FileDescriptor::FileDescriptor(GFile * file)
        : _native(file)
    {}

    void FileDescriptor::create_for_path(const FilePath& path)
    {
        _native = g_file_new_for_path(path.c_str());
    }

    void FileDescriptor::create_for_uri(const FileURI& uri)
    {
        _native = g_file_new_for_uri(uri.c_str());
    }

    FileDescriptor::operator GFile*()
    {
        return _native;
    }

    FilePath FileDescriptor::get_name()
    {
        return g_file_get_basename(_native);
    }

    FilePath FileDescriptor::get_path()
    {
        return g_file_get_path(_native);
    }

    FileURI FileDescriptor::get_uri()
    {
        return g_file_get_uri(_native);
    }

    bool FileDescriptor::operator==(const FileDescriptor& other)
    {
        return g_file_equal(this->_native, other._native);
    }

    bool FileDescriptor::operator!=(const FileDescriptor& other)
    {
        return not this->operator==(other);
    }

    FileDescriptor FileSystem::duplicate(FileDescriptor& in)
    {
        return FileDescriptor(g_file_dup(in.operator GFile *()));
    }

    bool FileSystem::create_at(FileDescriptor& destination, bool should_replace_destination)
    {
        int flags = G_FILE_CREATE_NONE;

        if (should_replace_destination)
            flags |= G_FILE_CREATE_REPLACE_DESTINATION;

        GError* error = nullptr;
        auto* stream = g_file_create(destination.operator GFile *(), static_cast<GFileCreateFlags>(flags), nullptr, &error);

        if (error != nullptr)
            std::cerr << "[WARNING] In FileSystem::new_file: Unable to create file at `" << destination.get_name() << "`: " << error->message
 << std::endl;

        return stream == nullptr;
    }

    bool FileSystem::delete_at(FileDescriptor& file)
    {
        GError* error = nullptr;
        auto out = g_file_delete(file.operator GFile *(), nullptr, &error);

        if (error != nullptr)
            std::cerr << "[WARNING] In FileSystem::delete_file: Unable to delete file at `" << file.get_name() << "`: " << error->message << std::endl;

        return out;
    }

    bool FileSystem::copy(
        FileDescriptor& from,
        FileDescriptor& to,
        bool allow_overwrite,
        bool make_backup,
        bool follow_symlinks)
    {
        int flags = G_FILE_COPY_NONE;

        if (allow_overwrite)
            flags |= G_FILE_COPY_OVERWRITE;

        if (make_backup)
            flags |= G_FILE_COPY_BACKUP;

        if (not follow_symlinks)
            flags |= G_FILE_COPY_NOFOLLOW_SYMLINKS;

        GError* error = nullptr;
        auto out = g_file_copy(
            from.operator GFile *(),
            to.operator GFile *(),
            static_cast<GFileCopyFlags>(flags),
            nullptr, nullptr, nullptr,
            &error
        );

        if (error != nullptr)
            std::cerr << "[WARNING] In FileSystem::copy: Unable to copy file from `" << from.get_name() << "` to `" << to.get_name() << "`: " << error->message << std::endl;

        return out;
    }

    void FileSystem::copy_async(
            FileDescriptor& from,
            FileDescriptor& to,
            int io_priority,
            bool allow_overwrite,
            bool make_backup,
            bool follow_symlinks)
    {
        int flags = G_FILE_COPY_NONE;

        if (allow_overwrite)
            flags |= G_FILE_COPY_OVERWRITE;

        if (make_backup)
            flags |= G_FILE_COPY_BACKUP;

        if (not follow_symlinks)
            flags |= G_FILE_COPY_NOFOLLOW_SYMLINKS;

        g_file_copy_async(
            from.operator GFile *(),
            to.operator GFile *(),
            static_cast<GFileCopyFlags>(flags),
            io_priority,
            nullptr, nullptr, nullptr,
            G_CALLBACK(async_callback),
            nullptr
        );
    }

    bool FileSystem::move(
            FileDescriptor& from,
            FileDescriptor& to,
            bool allow_overwrite,
            bool make_backup,
            bool follow_symlinks)
    {
        int flags = G_FILE_COPY_NONE;

        if (allow_overwrite)
            flags |= G_FILE_COPY_OVERWRITE;

        if (make_backup)
            flags |= G_FILE_COPY_BACKUP;

        if (not follow_symlinks)
            flags |= G_FILE_COPY_NOFOLLOW_SYMLINKS;

        GError* error = nullptr;
        auto out = g_file_move(
                from.operator GFile *(),
                to.operator GFile *(),
                static_cast<GFileCopyFlags>(flags),
                nullptr, nullptr, nullptr,
                &error
        );

        if (error != nullptr)
            std::cerr << "[WARNING] In FileSystem::copy: Unable to copy file from `" << from.get_name() << "` to `" << to.get_name() << "`: " << error->message << std::endl;

        return out;
    }

    void FileSystem::move_async(
            FileDescriptor& from,
            FileDescriptor& to,
            int io_priority,
            bool allow_overwrite,
            bool make_backup,
            bool follow_symlinks)
    {
        int flags = G_FILE_COPY_NONE;

        if (allow_overwrite)
            flags |= G_FILE_COPY_OVERWRITE;

        if (make_backup)
            flags |= G_FILE_COPY_BACKUP;

        if (not follow_symlinks)
            flags |= G_FILE_COPY_NOFOLLOW_SYMLINKS;

        g_file_move_async(
                from.operator GFile *(),
                to.operator GFile *(),
                static_cast<GFileCopyFlags>(flags),
                io_priority,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
        );
    }

    bool FileSystem::move_to_trash(FileDescriptor& file)
    {
        GError* error = nullptr;
        auto out = g_file_trash(file.operator GFile *(), nullptr, &error);

        if (error != nullptr)
            std::cerr << "[WARNING] In FileSystem::move_to_trash: Unable to move file `" << file.get_name() << "` to trash: " << error->message << std::endl;

        return out;
    }

    bool FileSystem::file_exists(const FilePath& path)
    {
        return g_file_test(path.c_str(), G_FILE_TEST_EXISTS);
    }

    bool FileSystem::file_is_directory(const FilePath& path)
    {
        return g_file_test(path.c_str(), G_FILE_TEST_IS_DIR);
    }

    bool FileSystem::file_is_executable(const FilePath& path)
    {
        return g_file_test(path.c_str(), G_FILE_TEST_IS_EXECUTABLE);
    }

    bool FileSystem::file_is_symlink(const FilePath& path)
    {
        return g_file_test(path.c_str(), G_FILE_TEST_IS_SYMLINK);
    }

    FilePath FileSystem::file_read_symlink(const FilePath& path)
    {
        GError* error = nullptr;
        auto* out = g_file_read_link(path.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In FileSystem::file_read_symlink: " << error->message << std::endl;
            return "";
        }

        return FilePath(out);
    }

    FileURI FileSystem::file_path_to_file_uri(const FilePath& path)
    {
        GError* error = nullptr;
        auto* out = g_filename_to_uri(path.c_str(), nullptr, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In FileSystem::file_path_to_file_uri: " << error->message << std::endl;
            return "";
        }

        return out;
    }

    FilePath file_uri_to_file_path(const FileURI& uri)
    {
        GError* error = nullptr;
        auto* out = g_filename_from_uri(uri.c_str(), nullptr, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In FileSystem::file_uri_to_file_path: " << error->message << std::endl;
            return "";
        }

        return out;
    }
}