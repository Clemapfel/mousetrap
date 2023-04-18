//
// Created by clem on 3/31/23.
//

#include <include/file_descriptor.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    FileDescriptor::FileDescriptor()
        : FileDescriptor(g_file_new_for_path(""))
    {}

    FileDescriptor::FileDescriptor(const FilePath& path)
        : FileDescriptor(g_file_new_for_path(path.c_str()))
    {}

    FileDescriptor::FileDescriptor(GFile* file)
    : _native(file)
    {
        if (_native != nullptr)
            g_object_ref(file);
    }

    FileDescriptor::~FileDescriptor()
    {
        if (_native != nullptr)
            g_object_unref(_native);
    }

    bool FileDescriptor::create_from_path(const FilePath& path)
    {
        _native = g_file_new_for_path(path.c_str());
        return _native != nullptr and exists();
    }

    bool FileDescriptor::create_from_uri(const FileURI& uri)
    {
        _native = g_file_new_for_uri(uri.c_str());
        return _native != nullptr and exists();
    }

    FileDescriptor::FileDescriptor(const FileDescriptor& other)
    : FileDescriptor(g_file_dup(other._native))
    {}

    FileDescriptor::FileDescriptor(FileDescriptor&& other)
    : _native(other._native)
    {
        other._native = nullptr;
    }

    FileDescriptor& FileDescriptor::operator=(const FileDescriptor& other)
    {
        if (_native != nullptr)
            g_object_unref(_native);

        _native = g_file_dup(other._native);
        g_object_ref(_native);
        return *this;
    }

    FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other)
    {
        _native = other._native;
        other._native = nullptr;
        return *this;
    }

    FileDescriptor::operator GFile*() const
    {
        return _native;
    }

    bool FileDescriptor::exists() const
    {
        if (_native == nullptr)
            return false;

        return g_file_test(get_path().c_str(), G_FILE_TEST_EXISTS);
    }

    bool FileDescriptor::is_file() const
    {
        if (_native == nullptr)
            return false;

        return g_file_test(get_path().c_str(), G_FILE_TEST_IS_REGULAR);
    }

    bool FileDescriptor::is_folder() const
    {
        if (_native == nullptr)
            return false;

        return g_file_test(get_path().c_str(), G_FILE_TEST_IS_DIR);
    }

    bool FileDescriptor::is_symlink() const
    {
        if (_native == nullptr)
            return false;

        return g_file_test(get_path().c_str(), G_FILE_TEST_IS_SYMLINK);
    }

    bool FileDescriptor::is_executable() const
    {
        if (_native == nullptr)
            return false;

        return g_file_test(get_path().c_str(), G_FILE_TEST_IS_EXECUTABLE);
    }

    FileDescriptor FileDescriptor::read_symlink() const
    {
        GError* error = nullptr;
        auto* out = g_file_read_link(get_path().c_str(), &error);

        if (error != nullptr)
        {
            log::critical(std::string("In FileDescriptor::read_symlink: ") + error->message, MOUSETRAP_DOMAIN);
            g_error_free(error);
            return FileDescriptor();
        }

        return FileDescriptor(std::string(out));
    }

    std::string FileDescriptor::get_name() const
    {
        if (_native == nullptr)
            return "";

        return g_file_get_basename(_native);
    }

    FilePath FileDescriptor::get_path() const
    {
        if (_native == nullptr)
            return "";

        auto* path = g_file_get_path(_native);
        return path == nullptr ? "" : path;
    }

    FilePath FileDescriptor::get_path_relative_to(const FileDescriptor& other) const
    {
        if (_native == nullptr or other._native == nullptr)
            return "";

        auto* path = g_file_get_relative_path(_native, other._native);
        return path != nullptr ? path : "";
    }

    FileDescriptor FileDescriptor::get_parent() const
    {
        if (_native == nullptr)
            return FileDescriptor();

        return g_file_get_parent(_native);
    }

    FileURI FileDescriptor::get_uri() const
    {
        if (_native == nullptr)
            return "";

        auto* uri = g_file_get_uri(_native);
        return uri == nullptr ? "" : uri;
    }

    std::string FileDescriptor::get_file_extension() const
    {
        if (_native == nullptr)
            return "";

        auto name = get_name();

        size_t i = name.size()-1;
        while (i > 0 and name.at(i) != '.')
            i -= 1;

        return std::string(name.begin() + i, name.end());
    }

    std::string FileDescriptor::get_content_type() const
    {
        return query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE);
    }

    std::string FileDescriptor::query_info(const char* attribute_query_string) const
    {
        GError* error = nullptr;
        auto* info = g_file_query_info(_native, attribute_query_string, G_FILE_QUERY_INFO_NONE, nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In FileDescriptor::query_info: Unable to retrieve info for attribute `" << attribute_query_string << "`: " << error->message << std::endl;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return "";
        }

        auto* out = g_file_info_get_attribute_as_string(info, attribute_query_string);
        return out == nullptr ? "" : out;
    }

    bool FileDescriptor::operator==(const FileDescriptor& other) const
    {
        if (this->_native == nullptr or other._native == nullptr)
            return false;

        return g_file_equal(this->_native, other._native);
    }

    bool FileDescriptor::operator!=(const FileDescriptor& other) const
    {
        if (this->_native == nullptr or other._native == nullptr)
            return false;

        return not this->operator==(other);
    }

    size_t FileDescriptor::hash() const
    {
        return g_file_hash(_native);
    }

    std::vector<FileDescriptor> FileDescriptor::get_children(bool recursive)
    {
        std::vector<FileDescriptor> out;
        if (not is_folder())
            return out;

        std::function<void(GFile*, std::vector<FileDescriptor>*)> enumerate;
        enumerate = [enumerate, recursive](GFile* file, std::vector<FileDescriptor>* out)
        {
            GError* error = nullptr;
            auto* enumerator = g_file_enumerate_children(
            file,
            G_FILE_ATTRIBUTE_STANDARD_NAME,
            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
            nullptr,
            &error
            );

            if (error != nullptr)
            {
                std::stringstream str;
                str << "In filesystem::get_all_files_in: Error when trying to enumerate children of file at `" << g_file_get_path(file) << "`: " << error->message;
                log::critical(str.str(), MOUSETRAP_DOMAIN);
                g_error_free(error);
                return;
            }

            if (enumerator == nullptr)
                return;

            auto* current = g_file_enumerator_next_file(enumerator, nullptr, &error);
            while (current != nullptr)
            {
                if (error != nullptr)
                {
                    std::stringstream str;
                    str << "In filesystem::get_all_files_in: Error when trying to enumerate children of file at `" << g_file_get_path(file) << "`: " << error->message;
                    log::critical(str.str(), MOUSETRAP_DOMAIN);
                    g_error_free(error);
                    return;
                }

                auto to_push = FileDescriptor(g_file_enumerator_get_child(enumerator, current));
                out->emplace_back(to_push);
                current = g_file_enumerator_next_file(enumerator, nullptr, &error);

                if (to_push.is_folder() and recursive)
                    enumerate(to_push.operator GFile*(), out);
            }

            g_object_unref(enumerator);
        };

        enumerate(_native, &out);
        return out;
    }
}
