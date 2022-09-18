// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//


#include <fstream>
#include <iostream>
#include <sstream>

namespace mousetrap
{
    void ShortcutMap::load_from_file(const std::string& config_file)
    {
        std::ifstream file;
        file.open(config_file);

        std::string line;
        size_t line_i = 0;

        std::string current_region = "";
        while (getline(file, line))
        {
            std::string action_name, trigger_name;
            size_t i = 0;
            bool action_active = true;

            // comment or empty line
            if (line.size() == 0 or line.at(0) == '#')
                continue;

            // region marker
            if (line.at(0) == '[')
            {
                current_region = "";
                size_t i = 1;

                try
                {
                    while (line.at(i) != ']')
                    {
                        current_region.push_back(line.at(i));
                        i += 1;
                    }
                    continue;
                }
                catch (...)
                {
                    goto on_error;
                }
            }

            // assignment
            try
            {
                while (i < line.size())
                {
                    if (line.at(i) == ' ')
                    {
                        if (not action_active)
                            goto on_error;

                        if (line.at(i+1) == '=' and line.at(i+2) == ' ')
                        {
                            action_active = false;
                            i += 3;
                            continue;
                        }
                        else
                            goto on_error;
                    }

                    if (action_active)
                        action_name.push_back(line.at(i));
                    else
                        trigger_name.push_back(line.at(i));

                    i += 1;
                }
            }
            catch (...)
            {
                goto on_error;
            }

            if (trigger_name != "nothing")
            {
                auto* trigger = gtk_shortcut_trigger_parse_string(trigger_name.c_str());

                if (trigger == nullptr)
                    std::cerr << "[ERROR] In ShortcutMap::load_from_file: Key code \"" << trigger_name << "\" in line " << line_i << " is invalid." << std::endl;

                _bindings.insert({
                                         (current_region != "" ?  current_region + "." : "") + action_name,
                                         trigger
                                 });
            }

            line_i += 1;
            continue;

            on_error:
            {
                std::cerr << "In ShortcutMap::load_from_file: Error when parsing " << config_file << " at line " << line_i << ":\n"
                          << line << std::endl;
                return;
            };
        }
    }

    bool ShortcutMap::should_trigger(GdkEvent* event, const std::string& id)
    {
        auto it = _bindings.find(id);
        if (it == _bindings.end())
            return false;

        auto* trigger = it->second;
        if (trigger != nullptr)
            return gtk_shortcut_trigger_trigger(trigger, event, false);
    }

    std::string ShortcutMap::trigger_to_string(GtkShortcutTrigger* trigger)
    {
        if (trigger == nullptr)
            return "";

        std::stringstream out;
        for (char c : std::string(gtk_shortcut_trigger_to_string(trigger)))
        {
            if (c == '<')
                continue;
            else if (c == '>')
                out << "+";
            else
                out << (char) std::toupper(c);
        }

        return out.str();
    }

    std::string ShortcutMap::generate_control_tooltip(const std::string& action_prefix, const std::string& description)
    {
        static auto id_to_cleartext = [](const std::string& in) -> std::string
        {
            std::stringstream out;
            bool capitalize = true;
            for (char c : in)
            {
                if (c == '_')
                {
                    out << ' ';
                    capitalize = true;
                    continue;
                }

                if (capitalize)
                {
                    out << std::string{static_cast<char>(std::toupper(c))};
                    capitalize = false;
                    continue;
                }

                out << c;
            }

            return out.str();
        };

        std::vector<std::string> left;
        std::vector<std::string> right;

        size_t left_max = 0;
        size_t right_max = 0;
        for (auto& pair : _bindings)
        {
            for (size_t i = 0; i < action_prefix.size(); ++i)
                if (i >= pair.first.size() or pair.first.at(i) != action_prefix.at(i))
                    goto skip;

            left.push_back(trigger_to_string(pair.second));
            right.push_back(id_to_cleartext(pair.first.substr(action_prefix.size() + 1, std::string::npos)));

            left_max = std::max(left.back().size(), left_max);
            right_max = std::max(right.back().size(), right_max);
            skip:;
        }

        for (auto& l : left) // equally distributes spaces on both ends
        {
            bool flip_flop = true;
            l.reserve(left_max);
            while (l.size() < left_max)
            {
                if (flip_flop)
                    l.push_back(' ');
                else
                    l = " " + l;

                flip_flop = not flip_flop;
            }
        }

        std::stringstream out;
        out << "<b>" << id_to_cleartext(action_prefix) << "</b>\n\n";

        if (not description.empty())
            out << description << "\n";

        if (left.empty())
            return out.str();

        out << "<span foreground=\"#777777\">";
        for (size_t i = 0; i < left_max; ++i)
            out << "―";

        out << "</span>\n";

        for (size_t i = 0; i < left.size(); ++i)
            out << "<tt><b><span foreground=\"#BBBBBB\">" << left.at(i) << "</span></b></tt><span foreground=\"#777777\"> - " << right.at(i) << "</span>" << (i < left.size() - 1 ? "\n" : "");

        return out.str();
    }

    std::string ShortcutMap::get_shortcut_as_string(const std::string& action_prefix, const std::string& action)
    {
        auto it = _bindings.find(action_prefix + "." + action);

        if (it == _bindings.end())
            return "";
        else
            return trigger_to_string(it->second);
    }

    GtkShortcutTrigger* ShortcutMap::get_shortcut(const std::string& action_prefix, const std::string& action)
    {
        auto it = _bindings.find(action_prefix + "." + action);
        if (it == _bindings.end())
            return nullptr;
        else
            return it->second;
    }
}