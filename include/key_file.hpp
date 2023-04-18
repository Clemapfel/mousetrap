//
// Copyright 2022 Clemens Cords
// Created on 10/24/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <string>
#include <vector>

#include <include/image.hpp>

namespace mousetrap
{
    /// @brief glib key identifier file, supports .ini files
    class KeyFile
    {
        public:
            /// @brief id of a key
            using KeyID = std::string;

            /// @brief id of group, enclosed in []
            using GroupKey = std::string;

            /// @brief construct as empty file in RAM
            KeyFile();

            /// @brief construct from file on disk
            /// @param path
            KeyFile(const std::string& path);

            /// @brief free file in ram, does not save to disk automatically
            ~KeyFile();

            /// @brief copy ctor deleted
            KeyFile(const KeyFile&) = delete;

            /// @brief copy assignment deleted
            KeyFile& operator=(const KeyFile&) = delete;

            /// @brief move ctor, safely transfers ownership
            /// @param other
            KeyFile(KeyFile&&) noexcept;

            /// @brief move assignment, safely transfers ownership
            /// @param other
            /// @return self after assignment
            KeyFile& operator=(KeyFile&&) noexcept;

            /// @brief serialize
            operator std::string();

            /// @brief serialize, calls mousetrap::KeyFile::operator std::string()
            /// @return string
            std::string as_string();

            /// @brief load from file on disk
            /// @param path
            /// @return true if successfull, false otherwise. Prints soft warning for parsing errors
            bool load_from_file(const std::string& path);

            /// @brief load from string
            /// @param string
            /// @return true if successfull, false otherwise. Prints soft warning for parsing erros
            bool load_from_string(const std::string& file);

            /// @brief save to file on disk, creates file but not enclosed folder
            /// @param path
            /// @returns true if successfull, false otherwise
            bool save_to_file(const std::string& path);

            /// @brief get all group ids
            /// @return vector of ids
            std::vector<GroupKey> get_groups() const;

            /// @brief get all keys for a specific group
            /// @param group_id
            /// @return vector of ids
            std::vector<KeyID> get_keys(GroupKey) const;

            /// @brief test whether file has a group of given name and whether that group has a key of given name
            /// @return true if exist, false otherwise
            bool has_key(GroupKey, KeyID);

            /// @brief test whether file has a group of given name
            /// @return true if exist, false otherwise
            bool has_group(GroupKey);

            /// @brief add comment above group id, only one line of comment is possible. Prints soft warning if group doesn't exist
            /// @param group_id,
            /// @param comment
            void add_comment_above(GroupKey, const std::string& comment);

            /// @brief add comment above key in group, only one line of comment is possible. Prints soft warning if group doesn't exist
            /// @param group_id,
            /// @param key_id
            /// @param comment
            void add_comment_above(GroupKey, KeyID, const std::string& comment);

            /// @brief get comment above group label, prints soft warning if group does not exist
            /// @param group_id
            /// @return comment if exist, empty string otherwise
            std::string get_comment_above(GroupKey);

            /// @brief get comment above key, prints soft warning if group or key in group does not exist
            /// @param group_id
            /// @param key_id
            /// @return comment if exists, empty string otherwise
            std::string get_comment_above(GroupKey, KeyID);


            /// @brief get value of key in group as escaped string
            /// @param group_id
            /// @param key_id
            /// @return value as string if exists, empty string otherwise
            /// @note if the value is a string, all control sequences will be escaped. To avoid this, use get_value_as<std::string> instead of get_value
            std::string get_value(GroupKey, KeyID);

            /// @brief get value of key in group as data type, conversion may fail
            /// @param group_id
            /// @param key_id
            template<typename Return_t>
            Return_t get_value_as(GroupKey, KeyID);

            /// @brief set value from string, prints soft warning if group or key in group does not exist
            /// @param group_id
            /// @param key_id
            /// @param string
            void set_value(GroupKey, KeyID, const std::string&);

            /// @brief set value from type, prints soft warning if group or key in group does not exist, or conversion failed
            /// @param group_id
            /// @param key_id
            /// @param value
            template<typename Value_t>
            void set_value_as(GroupKey, KeyID, Value_t);

        private:
            GKeyFile* _native;
    };
}
