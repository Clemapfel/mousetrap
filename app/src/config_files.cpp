#include <app/config_files.hpp>

namespace mousetrap
{
    void validate_keybindings_file(KeyFile* file)
    {
        std::map<std::string, std::vector<std::string>> map;

        auto notify_invalid_shortcut = [](const std::string& action, const std::string& shortcut)
        {
            std::stringstream str;
            str << "<b>Invalid Shortcut</b>\n" << "Shortcut `" << shortcut << "` for action `" << action << "` is invalid. Ignoring assignment.";
            ((BubbleLogArea*) state::bubble_log)->send_message(str.str(), InfoMessageType::ERROR);
        };

        bool conflict = false;
        for (auto group : file->get_groups())
        {
            for (auto key : file->get_keys(group))
            {
                auto value = file->get_value(group, key);

                if (value == "never")
                    continue;

                auto* trigger = gtk_shortcut_trigger_parse_string(value.c_str());

                if (trigger == nullptr)
                {
                    notify_invalid_shortcut(group + "." + key, value);
                    continue;
                }

                auto hash = gtk_shortcut_trigger_to_string(trigger);

                auto it = map.find(hash);
                if (it != map.end())
                {
                    it->second.push_back(group + "." + key);
                    conflict = true;
                }
                else
                    map.insert({hash, {group + "." + key}});
            }
        }

        if (not conflict)
            return;

        std::stringstream message;
        message << "<b>Keybinding conflict detected</b>\n"
                << "(In " << get_resource_path() << "/keybindings.ini)\n\n";

        for (auto& pair : map)
        {
            if (pair.second.size() <= 1)
                continue;

            message << "\tShortcut `<tt>";

            for (auto c : pair.first)
            {
                if (c == '<')
                    message << "&lt;";
                else if (c == '>')
                    message << "&gt;";
                else
                    message << c;
            }

            message << "</tt>` was assigned to all of the following actions:" << std::endl;

            for (size_t i = 0; i < pair.second.size(); ++i)
                message << "\t\t<tt>" << pair.second.at(i) << "</tt>" << std::endl;
        }

        message << "\nPlease resolve these conflicts via the settings menu or by editing keybindings.ini";
        ((BubbleLogArea*) state::bubble_log)->send_message(message.str(), InfoMessageType::ERROR);
    }
}