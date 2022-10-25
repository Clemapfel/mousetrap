// 
// Copyright 2022 Clemens Cords
// Created on 10/24/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>

namespace mousetrap
{
    class ConfigFile
    {
        public:
            using KeyID = std::string;
            using GroupKey = std::string;

            ConfigFile();
            ConfigFile(const std::string& path);
            ~ConfigFile();

            bool load_from_file(const std::string& path);
            bool load_from_memory(const std::string& file);

            bool save_to_file(const std::string& path);

            std::vector<GroupKey> get_groups() const;
            std::vector<KeyID> get_keys(GroupKey) const;

            bool has_key(GroupKey, KeyID);
            bool has_group(GroupKey);

            std::string get_value(GroupKey, KeyID);

            /// \tparam Return_t: One of std::string, bool, int, float or std::vector<T> where T is one of std::string, bool, int, float
            template<typename Return_t>
            Return_t get_value_as(GroupKey, KeyID);

        private:
            GKeyFile* _native;
    };
}

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

    std::string ConfigFile::get_value(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        auto* value = g_key_file_get_value(_native, group.c_str(), key.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<bool>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<bool>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<int>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<int>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<double>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<double>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
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
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::string>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
            return "";
        }

        return std::string(value);
    }

    template<>
    std::vector<std::string> ConfigFile::get_value_as(GroupKey group, KeyID key)
    {
        GError* error = nullptr;
        gsize length;
        auto* value_list = g_key_file_get_string_list(_native, group.c_str(), key.c_str(), &length, &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ConfigFile::get_value_as<std::vector<std::string>>: Unable to retrieve value for key `" << key << "` in group `" << group << "`" << error->message << std::endl;
            return {};
        }

        std::vector<std::string> out;
        for (size_t i = 0; i < length; ++i)
            out.emplace_back(value_list[i]);

        return out;
    }
}