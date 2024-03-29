//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <mousetrap/file_descriptor.hpp>

#include <mousetrap/gtk_common.hpp>
#include <string>
#include <vector>

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        #if GTK_MINOR_VERSION >= 10
        void open_uri_callback(GObject* source, GAsyncResult* result, void* data);
        void open_file_callback(GObject* source, GAsyncResult* result, void* data);
        void show_in_file_explorer_callback(GObject* source, GAsyncResult* result, void* data);
        #endif
    }
    #endif

    /// @brief singleton file manager, mutates files
    struct file_system
    {
        /// @brief uninstantiatable singleton instance
        file_system() = delete;

        /// @brief create a file at given path
        /// @param destination file descriptor
        /// @param should_replace_destination if true, replaces file at destination, if false and file exist, does nothing
        /// @return true if operation was succesfull, false otherwise
        static bool create_file_at(const FileDescriptor& destination, bool should_replace_destination);

        /// @brief create a directory, including intermediate parents if they do not already exist
        /// @param destination absolute path to new directory
        /// @return true if operation was succesfull, false otherwise
        static bool create_directory_at(const FileDescriptor& destination);

        /// @brief delete a file at given path
        /// @param destination file descriptor
        /// @return true if operation was succesfull, false otherwise
        static bool delete_at(const FileDescriptor& destination);

        /// @brief copy a file to somewhere else
        /// @param from original file that should be copied
        /// @param to target destination of file
        /// @param allow_overwrite if true and a file at destination already exists, will overwrite that file. If false and file exists, does nothing
        /// @param make_backup should a backup of a potentially overwritten file be created
        /// @param follow_symlinks should symlinks be followed
        static bool copy(
            const FileDescriptor& from,
            const FileDescriptor& to,
            bool allow_overwrite = true,
            bool make_backup = true,
            bool follow_symlinks = false
        );

        /// @param from original file that should be copied
        /// @param to target destination of file
        /// @param allow_overwrite if true and a file at destination already exists, will overwrite that file. If false and file exists, does nothing
        /// @param make_backup should a backup of a potentially overwritten file be created
        /// @param follow_symlinks should symlinks be followed
        static bool move(
            const FileDescriptor& from,
            const FileDescriptor& to,
            bool allow_overwrite = true,
            bool make_backup = true,
            bool follow_symlinks = false
        );

        /// @brief attempt to move a file to the trash
        /// @param file target file to move
        static bool move_to_trash(const FileDescriptor& file);

        #if GTK_MINOR_VERSION >= 10
        /// @brief open a file or folder, this may present the user with an app selection
        /// @param file
        static void open_file(const FileDescriptor& file);

        /// @brief open the folder that contains this file (or folder)
        /// @param folder
        static void show_in_file_explorer(const FileDescriptor& file);

        /// @brief open an url such as link to a website, or a URI
        /// @param url
        static void open_url(const std::string& url);
        #endif
    };

}

