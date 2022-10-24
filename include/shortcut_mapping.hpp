// 
// Copyright 2022 Clemens Cords
// Created on 10/24/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/action_map.hpp>

#include <string>
#include <unordered_map>

namespace mousetrap
{
    class ShortcutMapping
    {
        public:
            void load_from(const std::string& file_path);
            ShortcutTriggerID at(ActionID);

        private:
            std::unordered_map<ActionID, ShortcutTriggerID> _mapping;
    };
}

///

#include <fstream>

namespace mousetrap
{
    void ShortcutMapping::load_from(const std::string& file_path)
    {
        std::ifstream file;
        file.open(file_path);

        static const char comment_char = '#';
        static const char namespace_id_open = '[';
        static const char namespace_id_close = ']';
        static const char assignment = '=';

        std::string line;
        size_t line_i = 0;

        std::string current_namespace = "";
        while (getline(file, line))
        {
            std::string non_comment_line;
            for (auto& c : line)
            {
                if (c == comment_char)
                    break;
                else if (c != ' ')
                    non_comment_line.push_back(c);
            }

            if (non_comment_line.empty())
                continue;


        }
    }
}