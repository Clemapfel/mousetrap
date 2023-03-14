//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/22/23
//

#pragma once

#include <app/project_state.hpp>
#include <app/detect_platform.hpp>

namespace mousetrap::state
{
    static void open_uri(const std::string& uri)
    {
        GError* error = nullptr;
        g_app_info_launch_default_for_uri(uri.c_str(), nullptr, &error);
        if (error != nullptr)
        {
            try
            {
                if (CURRENT_PLATFORM == LINUX)
                    std::system(("xdg-open " + uri).c_str());
                else if (CURRENT_PLATFORM == MAC)
                    std::system(("open " + uri).c_str());
                else if (CURRENT_PLATFORM == WINDOWS)
                    std::system(("start " + uri).c_str());

            } catch (...)
            {
                auto error_message = "Unable to open uri at `" + uri + "`: " + error->message;
                state::bubble_log->send_message(error_message, InfoMessageType::ERROR);
                std::cerr << error_message << std::endl;
            }
        }
    }
}
