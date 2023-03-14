//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/22/23
//

#pragma once

namespace mousetrap
{
    #define LINUX "PLATFORM_LINUX"
    #define MAC "PLATFORM_APPLE"
    #define WINDOWS "PLATFORM_WINDOWS"

    #ifdef __linux__
        #define CURRENT_PLATFORM LINUX
    #elif _WINN32
        #define CURRENT_PLATFORM WINDOWS
    #elif __APPLE__
        #define CURRENTPLATFORM MAC
    #endif
}