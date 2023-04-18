
//
// Created by clem on 3/23/23.
//

#include <include/file_system.hpp>
#include <include/file_monitor.hpp>
#include <include/log.hpp>

#include <sstream>
#include <iostream>

namespace mousetrap
{
    FileMonitor FileDescriptor::create_monitor() const
    {
        GError* error = nullptr;
        auto out = FileMonitor(g_file_monitor(_native, (GFileMonitorFlags) FileMonitor::flags, nullptr, &error));

        if (error != nullptr)
        {
            log::critical(std::string("In FileDescriptor::create_monitor: ") + error->message, MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    bool FileSystem::create_file_at(const FileDescriptor& destination, bool should_replace_destination)
    {
        int flags = G_FILE_CREATE_NONE;

        if (should_replace_destination)
            flags |= G_FILE_CREATE_REPLACE_DESTINATION;

        GError* error = nullptr;
        auto* stream = g_file_create(destination.operator GFile *(), static_cast<GFileCreateFlags>(flags), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In FileSystem::new_file: Unable to create file at `" << destination.get_name() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
        }

        auto out = stream == nullptr;
        g_object_unref(stream);
        return out;
    }

    bool FileSystem::create_directory_at(const FileDescriptor& destination)
    {
        return g_mkdir_with_parents(destination.get_path().c_str(), 0);
    }

    bool FileSystem::delete_at(const FileDescriptor& file)
    {
        GError* error = nullptr;
        auto out = g_file_delete(file.operator GFile *(), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In FileSystem::delete_file: Unable to delete file at `" << file.get_name() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
        }

        return out;
    }

    bool FileSystem::copy(
        const FileDescriptor& from,
        const FileDescriptor& to,
        bool allow_overwrite,
        bool make_backup,
        bool follow_symlinks
    )
    {
        int flags = G_FILE_COPY_NONE | G_FILE_COPY_ALL_METADATA;

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
        {
            std::stringstream str;
            str << "In FileSystem::copy: Unable to copy file from `" << from.get_path() << "` to `" << to.get_path() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    bool FileSystem::move(
        const FileDescriptor& from,
        const FileDescriptor& to,
        bool allow_overwrite,
        bool make_backup,
        bool follow_symlinks
    )
    {
        int flags = G_FILE_COPY_NONE | G_FILE_COPY_ALL_METADATA;

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
        {
            std::stringstream str;
            str << "In FileSystem::copy: Unable to copy file from `" << from.get_name() << "` to `" << to.get_name() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    bool FileSystem::move_to_trash(const FileDescriptor& file)
    {
        GError* error = nullptr;
        auto out = g_file_trash(file.operator GFile *(), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "[WARNING] In FileSystem::move_to_trash: Unable to move file `" << file.get_name() << "` to trash: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }
}