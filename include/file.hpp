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

    class Icon
    {
        public:
            Icon(GIcon*);
            ~Icon();

        private:
            GIcon* _native;
    };

    class FileDescriptor
    {
        public:
            FileDescriptor();
            FileDescriptor(const FilePath& path);
            FileDescriptor(GFile*);
            ~FileDescriptor();

            FileDescriptor(const FileDescriptor& other);
            FileDescriptor(FileDescriptor&& other);
            FileDescriptor& operator=(const FileDescriptor& other);
            FileDescriptor& operator=(FileDescriptor&& other);

            operator GFile*();

            void create_for_path(const FilePath& path);
            void create_for_uri(const FileURI& uri);

            FilePath get_name();
            FilePath get_path();
            FileURI  get_uri();
            std::string get_extension();

            bool operator==(const FileDescriptor& other);
            bool operator!=(const FileDescriptor& other);

            bool is_folder();
            bool is_file();

            std::string get_content_type();

            /// \note list of attributes available here: https://www.freedesktop.org/software/gstreamer-sdk/data/docs/2012.5/gio/GFileInfo.html
            std::string query_info(const char* attribute_query_string);

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

        /// \param recursive: also list files in subfolders
        /// \param list_directories: should folders themself be included
        std::vector<FileDescriptor> get_all_files_in_directory(
            const FilePath& path,
            bool recursive = false,
            bool list_directories = true
        );
    };
}

#include <src/file.inl>
