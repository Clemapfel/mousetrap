// 
// Copyright 2022 Clemens Cords
// Created on 11/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/key_file.hpp>

namespace mousetrap::state
{
    inline KeyFile* settings_file = nullptr;
    inline KeyFile* keybindings_file = nullptr;
    inline KeyFile* tooltips_file = nullptr;
}