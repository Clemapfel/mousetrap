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

            void set_value(GroupKey, KeyID, const std::string&);

            template<typename Value_t>
            void set_value_as(GroupKey, KeyID, Value_t);

        private:
            GKeyFile* _native;
    };
}

#include <src/config_file.inl>
