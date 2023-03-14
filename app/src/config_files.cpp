#include <app/config_files.hpp>
#include <app/bubble_log_area.hpp>
#include <map>

namespace mousetrap
{
    void validate_keybindings_file(KeyFile* file)
    {
        std::map<std::string, std::vector<std::string>> map;

        auto notify_invalid_shortcut = [](const std::string& action, const std::string& shortcut)
        {
            std::stringstream str;
            str << "<b>Invalid Shortcut</b>\n" << "Shortcut `" << shortcut << "` for action `" << action << "` is invalid. Ignoring assignment.";
            state::bubble_log->send_message(str.str(), InfoMessageType::ERROR);
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

        std::vector<std::string> valid = {"<Shift>", "<Control>", "<Alt>"};
        auto scroll_scale_active = std::string(gtk_shortcut_trigger_to_string(gtk_shortcut_trigger_parse_string(file->get_value("canvas", "scroll_scale_active").c_str())));
        bool scroll_scale_active_okay = false;

        auto lock_axis_movement = std::string(gtk_shortcut_trigger_to_string(gtk_shortcut_trigger_parse_string(file->get_value("canvas", "lock_axis_movement").c_str())));
        bool lock_axis_movement_okay = false;

        for (auto& v : valid)
        {
            if (scroll_scale_active == v)
                scroll_scale_active_okay = true;

            if (lock_axis_movement == v)
                lock_axis_movement_okay = true;
        }

        if (not scroll_scale_active_okay)
        {
            std::stringstream message;
            std::stringstream sanitized;
            sanitized << "<b>Invalid Keybinding detected</b>\n";
            message << "Key `canvas.scroll_scale_active has to be exactly one of `<Shift>`, `<Control>` or `<Alt>`, but it is set to `"
                    << scroll_scale_active << "`" << std::endl;

            for (auto c : message.str())
            {
                if (c == '<')
                    sanitized << "&lt;";
                else if (c == '>')
                    sanitized << "&gt;";
                else
                    sanitized << c;
            }
            state::bubble_log->send_message(sanitized.str(), InfoMessageType::ERROR);
        }

        if (not lock_axis_movement_okay)
        {
            std::stringstream message;
            std::stringstream sanitized;
            sanitized << "<b>Invalid Keybinding detected</b>\n";
            message << "Key `canvas.lock_axis_movement has to be exactly one of `<Shift>`, `<Control>` or `<Alt>`, but it is set to `"
                    << lock_axis_movement_okay << "`" << std::endl;

            for (auto c : message.str())
            {
                if (c == '<')
                    sanitized << "&lt;";
                else if (c == '>')
                    sanitized << "&gt;";
                else
                    sanitized << c;
            }
            state::bubble_log->send_message(sanitized.str(), InfoMessageType::ERROR);
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
        state::bubble_log->send_message(message.str(), InfoMessageType::ERROR);
    }

    void initialize_config_files()
    {
        state::settings_file = new KeyFile(get_resource_path() + "settings.ini");
        state::keybindings_file = new KeyFile(get_resource_path() + "keybindings.ini");
        state::tooltips_file = new KeyFile(get_resource_path() + "tooltips.ini");
    }
}