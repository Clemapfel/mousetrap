//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/30/23
//

#pragma once

#include <include/gtk_common.hpp>
#include <string>
#include <vector>

namespace mousetrap
{
    class FileMonitor;

    /// @brief absolute path
    using FilePath = std::string;

    /// @brief uri, formatted according to
    using FileURI = std::string;

    /// @brief non-mutating file descriptor, may point to any file, directory, or no file at all. Changing an instance of this class does not modify the associated file in any way
    class FileDescriptor
    {
        public:
            /// @brief default ctor, unitialized
            FileDescriptor();

            /// @brief construct from path
            /// @param path absolute path
            FileDescriptor(const FilePath& path);

            /// @brief construct from GFile \internal
            /// @param file GFile
            FileDescriptor(GFile*);

            /// @brief destruct, does not mutate any data on disk
            ~FileDescriptor();

            /// @brief copy ctor
            /// @param other
            FileDescriptor(const FileDescriptor& other);

            /// @brief move ctor
            /// @param other
            FileDescriptor(FileDescriptor&& other);

            /// @brief copy assignment
            /// @param other
            /// @return reference to self after assignment
            FileDescriptor& operator=(const FileDescriptor& other);

            /// @brief move assignment
            /// @param other
            /// @return reference to self after assignment
            FileDescriptor& operator=(FileDescriptor&& other);

            /// @brief expose as GFile \internal
            operator GFile*() const;

            /// @brief create for given path
            /// @param path absolute path
            /// @return true if path points to a valid file or directory, false otherwse
            bool create_from_path(const FilePath& path);

            /// @brief create for given uri
            /// @param uri uri
            /// @return true if uri points to a valid file or directory, false otherwise
            bool create_from_uri(const FileURI& uri);

            /// @brief get name of self, including file extension, excluding any parent directories
            /// @return name, or "" if unintialized
            std::string get_name() const;

            /// @brief get absolute path
            /// @return absolute path, even if file does not exist, or "" if uninitialized
            FilePath get_path() const;

            /// @brief get uri
            /// @return uri, even if file does not exist, or "" if uninitialized
            FileURI  get_uri() const;

            /// @brief get a relative path from self to other
            /// @param other file descriptor
            /// @return relative file path, or "" if self or other uninitialized
            FilePath get_path_relative_to(const FileDescriptor& other) const;

            /// @brief get file descriptor to parent directory
            /// @return file descriptor, may not exist
            [[nodiscard]] FileDescriptor get_parent() const;

            /// @brief get file extension
            /// @return extension or "" if file descriptor points to a folder or is unitialized
            std::string get_file_extension() const;

            /// @brief test if two file descriptors point to the same object
            /// @param other
            /// @return true if both point to the same object, false otherwise
            bool operator==(const FileDescriptor& other) const;

            /// @brief test if two file descriptors point to the same object
            /// @param other
            /// @return false if both point to the same object, true otherwise
            bool operator!=(const FileDescriptor& other) const;

            /// @brief check if self points to a valid file or directory on disk
            /// @return true if exists, false otherwise
            bool exists() const;

            /// @brief check if descriptor points to a folder
            /// @return true if folder, false otherwise
            bool is_folder() const;

            /// @brief check if descriptor points to a file (as opposed to a directory)
            /// @return true if file, false otherwise
            bool is_file() const;

            /// @brief check if descriptor is a symlink to another file
            /// @return true if symlink, false otherwise
            bool is_symlink() const;

            /// @brief if descriptor is symlink, return descriptor to file self points to
            /// @return descriptor, if self is not symlink, descriptor will be invalid
            [[nodiscard]] FileDescriptor read_symlink() const;

            /// @brief check if file is an executable
            /// @return true if executable, false otherwise
            bool is_executable() const;

            /// @brief get the content type of the file
            /// @return standardized content type as string, or "" if uninitialized
            std::string get_content_type() const;

            /// @brief query arbitrary info about self, for a list of attributes available here: https://www.freedesktop.org/software/gstreamer-sdk/data/docs/2012.5/gio/GFileInfo.html
            /// @param attribute_query_string attribute name
            /// @return result as string, or "" if uninitialized or attribute does not exist
            std::string query_info(const char* attribute_query_string) const;

            /// @brief get unique hash for the self
            /// @return hash
            size_t hash() const;

            /// @brief create a file monitor, monitoring self
            /// @return monitor to self,
            [[nodiscard]] FileMonitor create_monitor() const;

            /// @brief if descriptor points to a folder, get a list of all files contained
            /// @param recursive should directories be enumerated recursively
            /// @return vector of file descriptors, excluding self. If self is not a directory, vector will be empty
            [[nodiscard]] std::vector<FileDescriptor> get_children(bool recursive = false);

        private:
            GFile* _native = nullptr;
    };
}