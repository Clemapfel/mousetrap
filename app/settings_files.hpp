// 
// Copyright 2022 Clemens Cords
// Created on 11/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/config_file.hpp>

namespace mousetrap::state
{
    ConfigFile* settings_file = nullptr;
    ConfigFile* keybindings_file = nullptr;
    ConfigFile* tooltips_file = nullptr;
}