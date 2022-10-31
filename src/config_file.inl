// 
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <iostream>

namespace mousetrap
{
    ConfigFile::ConfigFile()
            : _native(g_key_file_new())
    {
        g_key_file_ref(_native);
    }

    ConfigFile::ConfigFile(const std::string& path)
            : ConfigFile()
    {
        load_from_file(path);
    }

    ConfigFile::~ConfigFile()
    {
        g_key_file_free(_native);
    }

    bool ConfigFile::load_from_file(const std::string& path)
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
            std::cerr << "[ERROR] In ConfigFile::load_from_file: Unable to load file at `" << path << "`: " << error->message << std::endl;
            return false;
        }

        return true;
    }

    bool ConfigFile::load_from_memory(const std::string& file)
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
            std::cerr << "[ERROR] In ConfigFile::load_from_memory: Unable to load from string\n" << file << "\n\n" << error->message << std::endl;
            return false;
        }

        return true;
    }

    bool ConfigFile::save_to_file(const std::string& path)
    {
        GError* error = nullptr;
        g_key_file_save_to_file(_native, path.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::save_to_file: Unable to save file to `" << path << "`: " << error->message << std::endl;
            return false;
        }
        return true;
    }

    std::vector<ConfigFile::KeyID> ConfigFile::get_keys(GroupKey group) const
    {
        gsize length;
        GError* error = nullptr;
        auto* keys = g_key_file_get_keys(_native, group.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::cerr << "[WARNING] In ConfigFile::get_keys: Unable to retrieve keys for group `" << group << "`: " << error->message << std::endl;
            return {};
        }

        std::vector<ConfigFile::KeyID> out;
        for (size_t i = 0; i < length; ++i)
            out.emplace_back(keys[i]);

        return out;
    }

    bool ConfigFile::has_key(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto out = g_key_file_has_key(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::has_key: " << error->message << std::endl;
            return false;
        }

        return out;
    }

    std::vector<ConfigFile::GroupKey> ConfigFile::get_groups() const
    {
        gsize length;
        auto* groups = g_key_file_get_groups(_native, &length);

        std::vector<ConfigFile::GroupKey> out;
        for (size_t i = 0; i < length; ++i)
            out.emplace_back(groups[i]);

        return out;
    }

    bool ConfigFile::has_group(GroupKey group)
    {
        return g_key_file_has_group(_native, group.c_str());
    }

    void ConfigFile::add_comment_above(GroupKey group, KeyID key, const std::string& comment)
    {
        GError* error = nullptr;
        g_key_file_set_comment(_native, group.c_str(), key.c_str(), (" " + comment).c_str(), &error);

        if (error != nullptr)
            std::cerr << "[ERROR] In ConfigFile::add_comment_above: Unable to add comment for `" << group << "." << key << "`: " << error->message << std::endl;
    }

    void ConfigFile::add_comment_above(GroupKey group, const std::string& comment)
    {
        GError* error = nullptr;
        g_key_file_set_comment(_native, group.c_str(), nullptr, (" " + comment).c_str(), &error);

        if (error != nullptr)
            std::cerr << "[ERROR] In ConfigFile::add_comment_above: Unable to add comment for `" << group << "`: " << error->message << std::endl;
    }

    std::string ConfigFile::get_comment_above(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto* out = g_key_file_get_comment(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
            std::cerr << "[ERROR] In ConfigFile::get_comment_above: Unable to retrieve comment for `" << group << "." << key << "`: " << error->message << std::endl;

        return std::string(out);
    }

    std::string ConfigFile::get_comment_above(GroupKey group)
    {
        GError* error = nullptr;
        auto* out = g_key_file_get_comment(_native, group.c_str(), nullptr, &error);

        if (error != nullptr)
            std::cerr << "[ERROR] In ConfigFile::get_comment_above: Unable to retrieve comment for `" << group << "`: " << error->message << std::endl;

        return std::string(out);
    }

    std::string ConfigFile::get_value(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto* value = g_key_file_get_value(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return "";
        }

        return std::string(value);
    }

    template<>
    bool ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        bool value = g_key_file_get_boolean(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<bool>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return false;
        }

        return value;
    }

    template<>
    std::vector<bool> ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_boolean_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<bool>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return {};
        }

        std::vector<bool> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    int ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        int value = g_key_file_get_integer(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<int>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return -1;
        }

        return value;
    }

    template<>
    std::vector<int> ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_integer_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<int>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return {};
        }

        std::vector<int> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    float ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        double value = g_key_file_get_double(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<double>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return -1;
        }

        return value;
    }

    template<>
    std::vector<float> ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_double_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<double>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
            return {};
        }

        std::vector<float> out;
        for (size_t i = 0; i < length; ++i)
            out.push_back(value_list[i]);

        return out;
    }

    template<>
    std::string ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        const char* value = g_key_file_get_string(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::string>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
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
    std::vector<std::string> ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_string_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<std::string>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`: " << error->message << std::endl;
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
    HSVA ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        auto list = get_value_as<std::vector<float>>(group, key);
        if (not (list.size() != 3 or list.size() != 4))
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<HSVA>: Unable to retrieve value for key `" << key << "` in group `" << group << ": Incorrect number of color components";
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
    RGBA ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        auto list = get_value_as<std::vector<float>>(group, key);
        if (not (list.size() != 3 or list.size() != 4))
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<RGBA>: Unable to retrieve value for key `" << key << "` in group `" << group << ": Incorrect number of color components";
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

    void ConfigFile::set_value(GroupKey group, KeyID key, const std::string& value)
    {
        g_key_file_set_value(_native, group.c_str(), key.c_str(), value.c_str());
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, std::string value)
    {
        g_key_file_set_string(_native, group.c_str(), key.c_str(), value.c_str());
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, std::vector<std::string> value)
    {
        std::vector<const char*> to_add;
        for (auto& s : value)
            to_add.push_back(s.c_str());

        g_key_file_set_string_list(_native, group.c_str(), key.c_str(), to_add.data(), to_add.size());
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, bool value)
    {
        g_key_file_set_boolean(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, std::vector<bool> value)
    {
        std::vector<gboolean> to_add; // convert because std::vector<bool> is bit compressed
        for (bool b : value)
            to_add.push_back(static_cast<gboolean>(b));

        g_key_file_set_boolean_list(_native, group.c_str(), key.c_str(), to_add.data(), value.size());
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, int value)
    {
        g_key_file_set_integer(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, std::vector<int> value)
    {
        g_key_file_set_integer_list(_native, group.c_str(), key.c_str(), value.data(), value.size());
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, float value)
    {
        g_key_file_set_double(_native, group.c_str(), key.c_str(), value);
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, std::vector<float> value)
    {
        std::vector<gdouble> to_add;
        for (auto& f : value)
            to_add.push_back(static_cast<gdouble>(f));

        g_key_file_set_double_list(_native, group.c_str(), key.c_str(), to_add.data(), value.size());
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, HSVA hsva)
    {
        set_value_as<std::vector<float>>(group, key, {hsva.h, hsva.s, hsva.v, hsva.a});
    }

    template<>
    void ConfigFile::set_value_as(GroupKey group, KeyID key, RGBA rgba)
    {
        set_value_as<std::vector<float>>(group, key, {rgba.r, rgba.g, rgba.b, rgba.a});
    }
}