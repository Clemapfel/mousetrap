
//
// Created by clem on 3/23/23.
//

#include <mousetrap/file_system.hpp>
#include <mousetrap/file_monitor.hpp>
#include <mousetrap/file_descriptor.hpp>
#include <mousetrap/log.hpp>

#include <sstream>
#include <iostream>

namespace mousetrap
{
    FileMonitor FileDescriptor::create_monitor() const
    {
        GError* error = nullptr;
        auto out = FileMonitor(g_file_monitor(_native, (GFileMonitorFlags) (G_FILE_MONITOR_NONE | G_FILE_MONITOR_WATCH_MOVES), nullptr, &error));
        if (error != nullptr)
        {
            log::critical(std::string("In FileDescriptor::create_monitor: ") + error->message, MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    bool file_system::create_file_at(const FileDescriptor& destination, bool should_replace_destination)
    {
        int flags = G_FILE_CREATE_NONE;

        if (should_replace_destination)
            flags |= G_FILE_CREATE_REPLACE_DESTINATION;

        GError* error = nullptr;
        auto* stream = g_file_create(destination.operator GFile *(), static_cast<GFileCreateFlags>(flags), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In file_system::new_file: Unable to create file at `" << destination.get_name() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
        }

        auto out = stream == nullptr;
        g_object_unref(stream);
        return out;
    }

    bool file_system::create_directory_at(const FileDescriptor& destination)
    {
        return g_mkdir_with_parents(destination.get_path().c_str(), 0);
    }

    bool file_system::delete_at(const FileDescriptor& file)
    {
        GError* error = nullptr;
        auto out = g_file_delete(file.operator GFile *(), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In file_system::delete_file: Unable to delete file at `" << file.get_name() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
        }

        return out;
    }

    bool file_system::copy(
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
            str << "In file_system::copy: Unable to copy file from `" << from.get_path() << "` to `" << to.get_path() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    bool file_system::move(
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
            str << "In file_system::copy: Unable to copy file from `" << from.get_name() << "` to `" << to.get_name() << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    bool file_system::move_to_trash(const FileDescriptor& file)
    {
        GError* error = nullptr;
        auto out = g_file_trash(file.operator GFile *(), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In file_system::move_to_trash: Unable to move file `" << file.get_name() << "` to trash: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return out;
    }

    void detail::open_file_callback(GObject* source, GAsyncResult* result, void* data)
    {
        GError* error_maybe = nullptr;
        bool success = gtk_file_launcher_launch_finish(GTK_FILE_LAUNCHER(source), result, &error_maybe);

        auto file = FileDescriptor(G_FILE(data));

        if (error_maybe != nullptr)
        {
            if (error_maybe->code != 2) // not dismissed by user
                log::critical("In file_system::open_file: When trying to open `" + file.get_path() + "`: " + std::string(error_maybe->message), MOUSETRAP_DOMAIN);

            g_error_free(error_maybe);
        }

        g_object_unref(G_FILE(data));
        g_object_unref(GTK_FILE_LAUNCHER(source));
    }

    void file_system::open_file(const FileDescriptor& file)
    {
        detail::throw_if_gtk_is_uninitialized();

        auto* g_file = g_object_ref(file.operator GFile*());
        auto* launcher = gtk_file_launcher_new(g_file);
        gtk_file_launcher_launch(
            launcher,
            nullptr,
            nullptr,
            (GAsyncReadyCallback) detail::open_file_callback,
            g_file
        );
    }

    void detail::show_in_file_explorer_callback(GObject* source, GAsyncResult* result, void* data)
    {
        GError* error_maybe = nullptr;
        bool success = gtk_file_launcher_open_containing_folder_finish(GTK_FILE_LAUNCHER(source), result, &error_maybe);

        auto file = FileDescriptor(G_FILE(data));

        if (error_maybe != nullptr)
        {
            if (error_maybe->code != 2) // dismissed by user
                log::critical("In file_system::open_folder: When trying to open `" + file.get_path() + "`: " + std::string(error_maybe->message), MOUSETRAP_DOMAIN);

            g_error_free(error_maybe);
        }

        g_object_unref(G_FILE(data));
        g_object_unref(GTK_FILE_LAUNCHER(source));
    }

    void file_system::show_in_file_explorer(const FileDescriptor& file)
    {
        detail::throw_if_gtk_is_uninitialized();

        auto* g_file = g_object_ref(file.operator GFile*());
        auto* launcher = gtk_file_launcher_new(g_file);
        gtk_file_launcher_open_containing_folder(
        launcher,
        nullptr,
        nullptr,
        (GAsyncReadyCallback) detail::show_in_file_explorer_callback,
        g_file
        );
    }

    void detail::open_uri_callback(GObject* source, GAsyncResult* result, void* data)
    {
        GError* error_maybe = nullptr;
        bool success = gtk_uri_launcher_launch_finish(GTK_URI_LAUNCHER(source), result, &error_maybe);

        auto* uri = (std::string*) data;

        if (error_maybe != nullptr)
        {
            if (error_maybe->code != 2) // dismissed by user
                log::critical("In file_system::open_url: When trying to open `" + *((std::string*) data) + "`: " + std::string(error_maybe->message), MOUSETRAP_DOMAIN);

            g_error_free(error_maybe);
        }

        delete uri;
        g_object_unref(GTK_URI_LAUNCHER(source));
    }

    void file_system::open_url(const std::string& url)
    {
        detail::throw_if_gtk_is_uninitialized();

        auto* launcher = gtk_uri_launcher_new(url.c_str());
        gtk_uri_launcher_launch(
            launcher,
            nullptr,
            nullptr,
            (GAsyncReadyCallback) detail::open_uri_callback,
            new std::string(url)
        );
    }
}