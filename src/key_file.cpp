//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <include/key_file.hpp>
#include <include/log.hpp>

#include <sstream>
#include <iostream>

namespace mousetrap
{
    KeyFile::KeyFile()
        : _native(g_key_file_new())
    {
        g_key_file_ref(_native);
    }

    KeyFile::KeyFile(const std::string& path)
        : KeyFile()
    {
        load_from_file(path);
    }

    KeyFile::~KeyFile()
    {
        if (_native != nullptr)
            g_key_file_free(_native);
    }

    KeyFile::KeyFile(KeyFile&& other) noexcept
    {
        _native = other._native;
        other._native = nullptr;
    }

    KeyFile& KeyFile::operator=(KeyFile&& other) noexcept
    {
        if (_native != nullptr)
            g_key_file_free(_native);

        _native = other._native;
        other._native = nullptr;

        return *this;
    }

    KeyFile::operator std::string()
    {
        GError* error_maybe = nullptr;
        size_t length;

        auto* data = g_key_file_to_data(_native, &length, &error_maybe);
        if (error_maybe != nullptr)
        {
            log::critical(std::string("In KeyFile::operator std::string(): ") + error_maybe->message, MOUSETRAP_DOMAIN);
            return std::string();
        }

        std::string out;
        out.reserve(length);

        for (size_t i = 0; i < length; ++i)
            out.push_back(data[i]);

        return out;
    }

    std::string KeyFile::as_string()
    {
        return this->operator std::string();
    }

    bool KeyFile::load_from_file(const std::string& path)
    {
        GError* error = nullptr;
        g_key_file_load_from_file(
        _native,
        path.c_str(),
        static_cast<GKeyFileFlags>(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
        &error
        );

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::load_from_file: Unable to load file at `" << path << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return false;
        }

        return true;
    }

    bool KeyFile::load_from_string(const std::string& file)
    {
        GError* error = nullptr;

        g_key_file_load_from_data(
        _native,
        file.c_str(),
        file.size(),
        static_cast<GKeyFileFlags>(G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS),
        &error
        );

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::load_from_string: Unable to load from string\n" << file << "\n\n" << error->message;
            log::critical(str.str());
            return false;
        }

        return true;
    }

    bool KeyFile::save_to_file(const std::string& path)
    {
        GError* error = nullptr;
        g_key_file_save_to_file(_native, path.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::save_to_file: Unable to save to `" << path << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return false;
        }
        return true;
    }

    std::vector<KeyFile::KeyID> KeyFile::get_keys(GroupKey group) const
    {
        gsize length;
        GError* error = nullptr;
        auto* keys = g_key_file_get_keys(_native, group.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_keys: Unable to retrieve keys for group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return {};
        }

        std::vector<KeyFile::KeyID> out;
        for (size_t i = 0; i < length; ++i)
            out.emplace_back(keys[i]);

        return out;
    }

    bool KeyFile::has_key(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto out = g_key_file_has_key(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            log::critical(std::string("In KeyFile::has_key: ") + error->message, MOUSETRAP_DOMAIN);
            return false;
        }

        return out;
    }

    std::vector<KeyFile::GroupKey> KeyFile::get_groups() const
    {
        gsize length;
        auto* groups = g_key_file_get_groups(_native, &length);

        std::vector<KeyFile::GroupKey> out;
        for (size_t i = 0; i < length; ++i)
            out.emplace_back(groups[i]);

        return out;
    }

    bool KeyFile::has_group(GroupKey group)
    {
        return g_key_file_has_group(_native, group.c_str());
    }

    void KeyFile::add_comment_above(GroupKey group, KeyID key, const std::string& comment)
    {
        GError* error = nullptr;
        g_key_file_set_comment(_native, group.c_str(), key.c_str(), (" " + comment).c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::add_comment_above: Unable to add comment for `" << group << "." << key << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }
    }

    void KeyFile::add_comment_above(GroupKey group, const std::string& comment)
    {
        GError* error = nullptr;
        g_key_file_set_comment(_native, group.c_str(), nullptr, (" " + comment).c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::add_comment_above: Unable to add comment for `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }
    }

    std::string KeyFile::get_comment_above(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto* out = g_key_file_get_comment(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_comment_above: Unable to retrieve comment for `" << group << "." << key << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return std::string(out == nullptr ? "" : out);
    }

    std::string KeyFile::get_comment_above(GroupKey group)
    {
        GError* error = nullptr;
        auto* out = g_key_file_get_comment(_native, group.c_str(), nullptr, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_comment_above: Unable to retrieve comment for `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        return std::string(out == nullptr ? "" : out);
    }

    std::string KeyFile::get_value(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto* value = g_key_file_get_value(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return "";
        }

        return std::string(value);
    }

    template<>
    bool KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        bool value = g_key_file_get_boolean(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<bool>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return false;
        }

        return value;
    }

    template<>
    std::vector<bool> KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_boolean_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::vector<bool>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return {};
        }

        std::vector<bool> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    int KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<int>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return -1;
        }

        return value;
    }

    template<>
    std::vector<int> KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_integer_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::vector<int>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return {};
        }

        std::vector<int> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    size_t KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        int value = g_key_file_get_uint64(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<size_t>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return -1;
        }

        return value;
    }

    template<>
    std::vector<size_t> KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_integer_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::vector<int>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return {};
        }

        std::vector<size_t> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    double KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        double value = g_key_file_get_double(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<double>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return -1;
        }

        return value;
    }

    template<>
    std::vector<double> KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_double_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::vector<double>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return {};
        }

        std::vector<double> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    float KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        return get_value_as<double>(group, key);
    }

    template<>
    std::vector<float> KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_double_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::vector<float>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return {};
        }

        std::vector<float> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    std::string KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        const char* value = g_key_file_get_string(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::string>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return "";
        }

        auto string = std::string(value);
        std::string out = "";
        for (auto c : string)
            if (c != '\\')
                out.push_back(c);

        return out;
    }

    template<>
    std::vector<std::string> KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_string_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<std::vector<std::string>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message;
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return {};
        }

        std::vector<std::string> out;
        for (size_t i = 0; i < length; ++i)
        {
            auto string = std::string(value_list[i]);
            std::string to_push = "";
            for (auto c : string)
                if (c != '\\')
                    to_push.push_back(c);

            out.push_back(to_push);
        }

        return out;
    }

    template<>
    HSVA KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        auto list = get_value_as<std::vector<float>>(group, key);
        if (not (list.size() != 3 or list.size() != 4))
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<HSVA>: Unable to retrieve value for key `" << key << "` in group `" << group << ": Incorrect number of color components";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return HSVA(0, 0, 0, 0);
        }

        for (auto& e : list)
            e = glm::clamp<float>(e, 0, 1);

        return HSVA(
        list.at(0),
        list.at(1),
        list.at(2),
        list.size() == 3 ? 1 : list.at(3)
        );
    }

    template<>
    RGBA KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        auto list = get_value_as<std::vector<float>>(group, key);
        if (not (list.size() != 3 or list.size() != 4))
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<RGBA>: Unable to retrieve value for key `" << key << "` in group `" << group << ": Incorrect number of color components";
            return HSVA(0, 0, 0, 0);
        }

        for (auto& e : list)
            e = glm::clamp<float>(e, 0, 1);

        return RGBA(
        list.at(0),
        list.at(1),
        list.at(2),
        list.size() == 3 ? 1 : list.at(3)
        );
    }

    template<>
    Image KeyFile::get_value_as(GroupKey group, KeyID key)
    {
        Image out;

        auto serialized = get_value_as<std::vector<float>>(group, key);
        if (not (serialized.size() > (4 + 2)) and (serialized.size() - 2) % 4 == 0)
        {
            std::stringstream str;
            str << "In KeyFile::get_value_as<Image>: Unable to retrieve value for key `" << key << "` in group `" << group << ": Expected float vector of 4-mers but number of elements is not divisible by four";
            return out;
        }

        auto width = serialized.at(0);
        auto height = serialized.at(1);
        out.create(width, height);

        std::vector<RGBA> colors;

        for (size_t i = 2; i < serialized.size(); i += 4)
        {
            RGBA color;
            color.r = serialized.at(i + 0);
            color.g = serialized.at(i + 1);
            color.b = serialized.at(i + 2);
            color.a = serialized.at(i + 3);

            out.set_pixel((i - 2) / 4, color);
        }

        return out;
    }

    void KeyFile::set_value(GroupKey group, KeyID key, const std::string& value)
    {
        g_key_file_set_value(_native, group.c_str(), key.c_str(), value.c_str());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, std::string value)
    {
        g_key_file_set_string(_native, group.c_str(), key.c_str(), value.c_str());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, std::vector<std::string> value)
    {
        std::vector<const char*> to_add;
        for (auto& s : value)
            to_add.push_back(s.c_str());

        g_key_file_set_string_list(_native, group.c_str(), key.c_str(), to_add.data(), to_add.size());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, bool value)
    {
        g_key_file_set_boolean(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, std::vector<bool> value)
    {
        std::vector<gboolean> to_add; // convert because std::vector<bool> is bit compressed
        for (bool b : value)
            to_add.push_back(static_cast<gboolean>(b));

        g_key_file_set_boolean_list(_native, group.c_str(), key.c_str(), to_add.data(), value.size());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, int value)
    {
        g_key_file_set_integer(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, std::vector<int> value)
    {
        g_key_file_set_integer_list(_native, group.c_str(), key.c_str(), value.data(), value.size());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, size_t value)
    {
        if (value > std::numeric_limits<gint64>::max())
        {
            std::stringstream str;
            str << "[WARNING] In KeyFile::set_value_as<size_t>: Value " << value << " is too large to be stored as int";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
        }

        g_key_file_set_int64(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, std::vector<size_t> value)
    {
        std::vector<int> converted;
        converted.reserve(value.size());

        bool once = true;
        for (auto i : value)
        {
            if (i > std::numeric_limits<int>::max() and once)
            {
                std::stringstream str;
                str << "In KeyFile::set_value_as<std::vector<size_t>>: Value " << i << " is too large to be stored as int";
                log::critical(str.str(), MOUSETRAP_DOMAIN);
                once = false;
            }

            converted.push_back(static_cast<int>(i));
        }

        g_key_file_set_integer_list(_native, group.c_str(), key.c_str(), converted.data(), converted.size());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, float value)
    {
        g_key_file_set_double(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, std::vector<float> value)
    {
        std::vector<gdouble> to_add;
        for (auto& f : value)
            to_add.push_back(static_cast<gdouble>(f));

        g_key_file_set_double_list(_native, group.c_str(), key.c_str(), to_add.data(), value.size());
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, HSVA hsva)
    {
        set_value_as<std::vector<float>>(group, key, {hsva.h, hsva.s, hsva.v, hsva.a});
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, RGBA rgba)
    {
        set_value_as<std::vector<float>>(group, key, {rgba.r, rgba.g, rgba.b, rgba.a});
    }

    template<>
    void KeyFile::set_value_as(GroupKey group, KeyID key, Image image)
    {
        std::vector<float> serialized;
        auto n_pixels = image.get_size().x * image.get_size().y;
        serialized.reserve(2 + n_pixels);

        serialized.push_back((float) image.get_size().x);
        serialized.push_back((float) image.get_size().y);

        for (size_t i = 0; i < n_pixels; ++i)
        {
            auto color = image.get_pixel(i);
            serialized.push_back(color.r);
            serialized.push_back(color.g);
            serialized.push_back(color.b);
            serialized.push_back(color.a);
        }

        set_value_as<std::vector<float>>(group, key, serialized);
    }
}