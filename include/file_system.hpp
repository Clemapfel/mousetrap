//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/23/23
//

#pragma once

#include <include/file_descriptor.hpp>

#include <include/gtk_common.hpp>
#include <string>
#include <vector>

namespace mousetrap
{
    /// @brief singleton file manager, mutates files
    class FileSystem
    {
        public:
            /// @brief uninstantiatable singleton instance
            FileSystem() = delete;

            /// @brief create a file at given path
            /// @param destination file descriptor
            /// @param should_replace_destination if true, replaces file at destination, if false and file exist, does nothing
            /// @return true if operation was succesfull, false otherwise
            static bool create_file_at(const FileDescriptor& destination, bool should_replace_destination);

            /// @brief create a directory, including intermediate parents if they do not already exist
            /// @param path absolute path to new directory
            /// @return true if operation was succesfull, false otherwise
            static bool create_directory_at(const FileDescriptor& destination);

            /// @brief delete a file at given path
            /// @param destination file descriptor
            /// @return true if operation was succesfull, false otherwise
            static bool delete_at(const FileDescriptor&);

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
            static bool move_to_trash(const FileDescriptor&);
    };
}

